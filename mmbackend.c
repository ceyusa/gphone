/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

#include "mmbackend.h"

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>
#include <gst/base/gstadapter.h>

#include <string.h>

struct _MmBackendPrivate
{
    GstElement *player;
    GstElement *recorder;

    GstAppSrc *appsrc;   /* player */
    GstAppSink *appsink; /* recorder */

    GstAdapter *adapter_sink; /* adapter for appsink */
};

#define GET_PRIVATE(obj) \
	(G_TYPE_INSTANCE_GET_PRIVATE((obj), MM_TYPE_BACKEND, MmBackendPrivate))

GST_DEBUG_CATEGORY(_debug);
#define GST_CAT_DEFAULT _debug

G_DEFINE_TYPE(MmBackend, mm_backend, G_TYPE_OBJECT)

static void
finalize (GObject *object)
{
    MmBackend *self = (MmBackend *) object;

    mm_backend_audio_close (self);

    gst_adapter_clear (self->priv->adapter_sink);
    gst_object_unref (self->priv->adapter_sink);

    G_OBJECT_CLASS (mm_backend_parent_class)->finalize (object);
}

static void
mm_backend_class_init (MmBackendClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize = finalize;

    g_type_class_add_private (klass, sizeof (MmBackendPrivate));
}

inline static void
check_gstreamer ()
{
    GError *error = NULL;

    if (!gst_is_initialized ()) {
        gst_init_check (NULL, NULL, &error);
        if (error)
            g_error ("GStreamer failure: %s", error->message);
    }
}

static void
mm_backend_init (MmBackend *self)
{
    check_gstreamer ();

    GST_DEBUG_CATEGORY_INIT (_debug, "mm", 0, "multimedia backend");
    self->priv = GET_PRIVATE (self);

    self->priv->adapter_sink = gst_adapter_new ();
}

static void
handle_error (GstMessage *msg)
{
  gchar *debug = NULL;
  GError *err = NULL;

  gst_message_parse_error (msg, &err, &debug);
  g_debug ("Debugging info: %s\n", (debug) ? debug : "none");
  g_free (debug);

  g_printerr ("Error: %s\n", err->message);
  g_error_free (err);
}

static gboolean
bus_cb (GstBus *bus, GstMessage *msg, gpointer data)
{
    MmBackend *self = data;

    switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
        g_print ("End-of-stream\n");
        break;
    case GST_MESSAGE_ERROR:
        handle_error (msg);
        break;
    case GST_MESSAGE_STATE_CHANGED:
    {
        if (msg->src == (GstObject *) self->priv->player ||
            msg->src == (GstObject *) self->priv->recorder) {
            GstState new, old;

            gst_message_parse_state_changed (msg, &old, &new, NULL);
            GST_DEBUG ("Element %s state change: %s -> %s",
                       GST_OBJECT_NAME (msg->src),
                       gst_element_state_get_name (old),
                       gst_element_state_get_name (new));
        }
    }
    default:
        break;
    }

    return TRUE;
}

static const gchar *
get_player_desc ()
{
    return "appsrc is-live=true format=time do-timestamp=true name=opal-src "
        "! autoaudiosink name=audio-sink ";
}

static const gchar *
get_recorder_desc ()
{
    return "autoaudiosrc name=audio-src "
        "! appsink name=opal-sink max_buffers=2 drop=true";
}

inline static GstElement *
get_element (GstElement *bin, const char *name)
{
    if (bin)
        return gst_bin_get_by_name (GST_BIN (bin), name);

    return NULL;
}

inline static inline void
set_audio_config (GstChildProxy *proxy,
                  GObject *object,
                  gchar *name,
                  gpointer user_data)
{
    GParamSpec *spec;

    spec = g_object_class_find_property (G_OBJECT_GET_CLASS (object),
                                         "stream-properties");
    if (spec) {
        GstStructure *props;

        props = gst_structure_new ("props",
                                   "media.role", G_TYPE_STRING, "phone",
                                   "filter.want", G_TYPE_STRING, "echo-cancel",
                                   NULL);
        g_object_set (object, "stream-properties", props, NULL);

        gst_structure_free (props);
    }
}

gboolean
mm_backend_audio_open (MmBackend *self,
                       const char *dev,
                       MmBackendDirection dir,
                       guint channels,
                       guint rate)
{
    GST_INFO ("opening device %s / direction = %d", dev, dir);

    GstBus *bus;
    GError *error = NULL;
    GstStateChangeReturn ret;
    const gchar *desc, *name;
    GstElement *pipe, *app, *audio;

    if ((dir == MM_BACKEND_DIRECTION_PLAYER && self->priv->player) ||
        (dir == MM_BACKEND_DIRECTION_RECORDER && self->priv->recorder)) {
        GST_INFO("player / recorder already exists");
        return TRUE;
    }

    if (dir == MM_BACKEND_DIRECTION_PLAYER) {
        name = "audio-player";
        desc = get_player_desc ();
    } else if (dir == MM_BACKEND_DIRECTION_RECORDER) {
        name = "audio-recorder";
        desc = get_recorder_desc ();
    } else {
        return FALSE;
    }

    pipe = gst_parse_launch (desc, &error);
    if (error) {
        GST_ERROR ("Pipeline parsing error: %s\n", error->message);
        g_error_free (error);
        return FALSE;
    }

    gst_element_set_name (pipe, name);

    if (dir == MM_BACKEND_DIRECTION_PLAYER) {
        app = get_element (pipe, "opal-src");
        if (!app)
            return FALSE;

        audio = get_element (pipe, "audio-sink");
        if (!audio)
            return FALSE;

        self->priv->player = pipe;
        self->priv->appsrc = (GstAppSrc *) app;
    } else {
        app = get_element (pipe, "opal-sink");
        if (!app)
            return FALSE;

        audio = get_element (pipe, "audio-src");
        if (!audio)
            return FALSE;

        self->priv->recorder = pipe;
        self->priv->appsink = (GstAppSink *) app;
    }

    {
        GstCaps *caps;

        caps = gst_caps_new_simple ("audio/x-raw",
                                    "layout", G_TYPE_STRING, "interleaved",
                                    "format", G_TYPE_STRING, "S16LE",
                                    "rate", G_TYPE_INT, rate,
                                    "channels", G_TYPE_INT, channels,
                                    NULL);
        g_object_set (app, "caps", caps, NULL);
        gst_caps_unref (caps);
    }

    g_signal_connect (audio, "child-added", G_CALLBACK (set_audio_config), self);

    bus = gst_element_get_bus (pipe);
    gst_bus_add_watch (bus, bus_cb, self);
    g_object_unref (bus);

    ret = gst_element_set_state (pipe, GST_STATE_PLAYING);

    GST_INFO ("change state is %s",
              gst_element_state_change_return_get_name(ret));

    return (ret == GST_STATE_CHANGE_SUCCESS || ret == GST_STATE_CHANGE_ASYNC);
}

gboolean
mm_backend_audio_is_open (MmBackend *self)
{
    return self->priv->player || self->priv->recorder;
}

MmBackend *
mm_backend_new (void)
{
    return g_object_new (MM_TYPE_BACKEND, NULL);
}

static gboolean
shutdown_pipeline (GstElement *pipe, GstElement *app)
{
    GstStateChangeReturn ret;

    gst_object_unref (app);
    ret = gst_element_set_state (pipe, GST_STATE_NULL);
    gst_object_unref (pipe);

    return ret == GST_STATE_CHANGE_SUCCESS || ret == GST_STATE_CHANGE_ASYNC;
}

gboolean
mm_backend_audio_close (MmBackend *self)
{
    if (self->priv->player) {
        shutdown_pipeline (self->priv->player,
                           (GstElement *) self->priv->appsrc);
        self->priv->player = NULL;
        self->priv->appsrc = NULL;
    }

    if (self->priv->recorder) {
        shutdown_pipeline (self->priv->recorder,
                           (GstElement *) self->priv->appsink);
        self->priv->recorder = NULL;
        self->priv->appsink = NULL;
    }

    return TRUE;
}

void
mm_backend_set_buffer_size (MmBackend *self,
                            MmBackendDirection dir,
                            size_t size)
{
    g_return_if_fail (MM_IS_BACKEND (self));

    GstElement *el;

    if (dir == MM_BACKEND_DIRECTION_PLAYER)
        el = (GstElement *) self->priv->appsrc;
    else if (dir == MM_BACKEND_DIRECTION_RECORDER)
        el = (GstElement *) self->priv->appsink;
    else
        return;

    g_object_set (el, "blocksize", size, NULL);
}

gboolean
mm_backend_audio_write (MmBackend *self,
                        const void *buf,
                        size_t len,
                        size_t *written)
{
    g_return_val_if_fail (MM_IS_BACKEND (self), FALSE);

    GstBuffer *buffer;
    {
        GstMapInfo map;
        buffer = gst_buffer_new_and_alloc (len);

        if (!gst_buffer_map (buffer, &map, GST_MAP_WRITE)) {
            gst_buffer_unref (buffer);
            return FALSE;
        }

        memcpy (map.data, buf, len);
        gst_buffer_unmap (buffer, &map);
    }

    GST_DEBUG ("write %ld", len);
    GST_MEMDUMP ("write: ", buf, len);
    GstFlowReturn ret = gst_app_src_push_buffer (self->priv->appsrc, buffer);

    *written = len;

    return ret == GST_FLOW_OK;
}

gboolean
mm_backend_audio_read (MmBackend *self,
                       void *buf,
                       size_t len,
                       size_t *read)
{
    g_return_val_if_fail (MM_IS_BACKEND (self), FALSE);

    GST_DEBUG ("to read %ld", len);
    GstSample *sample = gst_app_sink_pull_sample (self->priv->appsink);
    if (sample) {
        GstBuffer *buffer = gst_buffer_copy (gst_sample_get_buffer (sample));
        gst_adapter_push (self->priv->adapter_sink, buffer);
        gst_sample_unref (sample);
    }

    *read = MIN (len, gst_adapter_available (self->priv->adapter_sink));
    GST_DEBUG ("read %ld", *read);

    if (*read > 0) {
        gst_adapter_copy (self->priv->adapter_sink, buf, 0, *read);
        gst_adapter_flush (self->priv->adapter_sink, *read);
        GST_MEMDUMP ("read: ", buf, *read);
    }

    return *read > 0;
}
