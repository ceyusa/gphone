/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

#include "gopalpcssep.h"

#include <ptlib.h>
#include <opal/pcss.h>

class MyPCSSEndPoint : public OpalPCSSEndPoint
{
    PCLASSINFO(MyPCSSEndPoint, OpalPCSSEndPoint);

public:
    MyPCSSEndPoint(OpalManager & manager,
                   GopalPCSSEP * pcssep)
        : OpalPCSSEndPoint(manager), m_pcssep(pcssep) { };

    virtual PBoolean OnShowIncoming(const OpalPCSSConnection & connection);
    virtual PBoolean OnShowOutgoing(const OpalPCSSConnection & connection);

private:
    GopalPCSSEP *m_pcssep;
};

PBoolean
MyPCSSEndPoint::OnShowIncoming(const OpalPCSSConnection & connection)
{
    const gchar *token = connection.GetToken();
    const gchar *name = connection.GetRemotePartyName();
    const gchar *address = connection.GetRemotePartyAddress();
    g_signal_emit_by_name(m_pcssep, "call-incoming", token, name, address);
    return true;
}


PBoolean
MyPCSSEndPoint::OnShowOutgoing(const OpalPCSSConnection & connection)
{
    const gchar *remote_name = connection.GetRemotePartyName();
    g_signal_emit_by_name(m_pcssep, "call-outgoing", remote_name);
    return true;
}


G_BEGIN_DECLS

enum { PROP_MANAGER = 1, PROP_PCSS, PROP_LAST };

struct _GopalPCSSEPPrivate
{
    MyPCSSEndPoint *pcssep;
};

#define GET_PRIVATE(obj)			\
        (G_TYPE_INSTANCE_GET_PRIVATE((obj), GOPAL_TYPE_PCSS_EP, GopalPCSSEPPrivate))

#define PCSSEP(obj)                            \
    (GET_PRIVATE((obj))->pcssep)

G_DEFINE_TYPE(GopalPCSSEP, gopal_pcss_ep, G_TYPE_OBJECT)

static GObject *
gopal_pcss_ep_constructor (GType type, guint n_properties,
                           GObjectConstructParam *properties)
{
    guint i;
    GObject *obj;
    GopalPCSSEP *self;
    OpalManager *manager;

    obj = G_OBJECT_CLASS (gopal_pcss_ep_parent_class)->constructor (type,
                                                                    n_properties,
                                                                    properties);
    self = GOPAL_PCSS_EP (obj);
    self->priv = GET_PRIVATE (self);

    for (i = 0; i < n_properties; i++) {
        if (strcmp ("manager", g_param_spec_get_name (properties[i].pspec)) == 0) {
                manager = static_cast<OpalManager *>(
                        g_value_get_pointer (properties[i].value)
                    );
                self->priv->pcssep = new MyPCSSEndPoint(*manager, self);

                break;
        }
    }

    g_assert (self->priv->pcssep);
    return obj;
}

static void
gopal_pcss_ep_set_property(GObject *object, guint property_id,
                          const GValue *value, GParamSpec *pspec)
{
    switch (property_id) {
    case PROP_MANAGER:
        /* Do nothing */
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
gopal_pcss_ep_get_property(GObject *object, guint property_id,
			   GValue *value, GParamSpec *pspec)
{
    GopalPCSSEP *self;

    self = GOPAL_PCSS_EP (object);

    switch (property_id) {
    case PROP_PCSS:
        g_value_set_pointer (value, self->priv->pcssep);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
gopal_pcss_ep_finalize (GObject *object)
{
    G_OBJECT_CLASS(gopal_pcss_ep_parent_class)->finalize(object);
}

static void
gopal_pcss_ep_class_init (GopalPCSSEPClass *klass)
{
    GObjectClass *gobject_class = (GObjectClass *) klass;

    gobject_class->finalize = gopal_pcss_ep_finalize;
    gobject_class->set_property = gopal_pcss_ep_set_property;
    gobject_class->get_property = gopal_pcss_ep_get_property;
    gobject_class->constructor = gopal_pcss_ep_constructor;

    g_type_class_add_private (klass, sizeof (GopalPCSSEPPrivate));

    g_object_class_install_property (gobject_class, PROP_MANAGER,
        g_param_spec_pointer("manager", "mgr", "Opal's Manager",
                             GParamFlags (G_PARAM_WRITABLE |
                                          G_PARAM_CONSTRUCT_ONLY |
                                          G_PARAM_STATIC_NAME)));

    g_object_class_install_property (gobject_class, PROP_PCSS,
        g_param_spec_pointer("pcss", "pcss", "PC Sound System",
                             GParamFlags (G_PARAM_READABLE |
                                          G_PARAM_STATIC_NAME)));

    /**
     * GopalPCSSEP:call-incoming:
     * @token: the token for this connection
     *
     * The ::call-incoming signal is emitted each time a
     * connection request is coming (ringing).
     */
    g_signal_new("call-incoming",
                 G_TYPE_FROM_CLASS (klass),
                 GSignalFlags (G_SIGNAL_RUN_LAST |
                               G_SIGNAL_NO_RECURSE |
                               G_SIGNAL_NO_HOOKS),
                 0,
                 NULL, NULL,
                 NULL,
                 G_TYPE_NONE,
                 3,
                 G_TYPE_STRING,
                 G_TYPE_STRING,
                 G_TYPE_STRING);

    /**
     * GopalPCSSEP:call-outgoing:
     * @remote_name: the name of the remote party
     *
     * The ::call-outgoing signal is emitted each time a
     * connection to a remote party is requested (ringing).
     */
    g_signal_new("call-outgoing",
                 G_TYPE_FROM_CLASS (klass),
                 GSignalFlags (G_SIGNAL_RUN_LAST |
                               G_SIGNAL_NO_RECURSE |
                               G_SIGNAL_NO_HOOKS),
                 0,
                 NULL, NULL,
                 NULL,
                 G_TYPE_NONE,
                 1,
                 G_TYPE_STRING);
}

static void
gopal_pcss_ep_init (GopalPCSSEP *self)
{
}

/**
 * gopal_pcss_ep_set_soundchannel_play_device:
 * @self: #GopalPCSSEP instance
 * @name: the play device name to use
 *
 * Returns: %TRUE if the devices suitable for use; %FALSE otherwise
 *
 * Set the name for the sound channel to be used for output.
 *
 * If the name is not suitable for use with the PSoundChannel class
 * then the function will return false and not change the device.
 *
 * This defaults to the value of the PSoundChannel::GetDefaultDevice()
 * function.
 */
gboolean
gopal_pcss_ep_set_soundchannel_play_device (GopalPCSSEP *self, const gchar *name)
{
    PString devname = (name) ? PString(name) : PString::Empty();
    return PCSSEP(self)->SetSoundChannelPlayDevice(devname);
}

/**
 * gopal_pcss_ep_get_soundchannel_play_device:
 * @self: #GopalPCSSEP instance
 *
 * Returns: the name of the play device
 *
 * Get the name for the sound channel to be used for output.
 *
 * This defaults to the value of the PSoundChannel::GetDefaultDevice()
 * function.
 */
const gchar *
gopal_pcss_ep_get_soundchannel_play_device (GopalPCSSEP *self)
{
    return PCSSEP(self)->GetSoundChannelPlayDevice();
}

/**
 * gopal_pcss_ep_set_soundchannel_record_device:
 * @self: #GopalPCSSEP instance
 * @name: the record device name to use
 *
 * Returns: %TRUE if the devices suitable for use; %FALSE otherwise
 *
 * Set the name for the sound channel to be used for input.
 *
 * If the name is not suitable for use with the PSoundChannel class
 * then the function will return false and not change the device.
 *
 * This defaults to the value of the PSoundChannel::GetDefaultDevice()
 * function.
 */
gboolean
gopal_pcss_ep_set_soundchannel_record_device (GopalPCSSEP *self, const gchar *name)
{
    PString devname = (name) ? PString(name) : PString::Empty();
    return PCSSEP(self)->SetSoundChannelRecordDevice(devname);
}

/**
 * gopal_pcss_ep_get_soundchannel_record_device:
 * @self: #GopalPCSSEP instance
 *
 * Returns: the name of the record device
 *
 * Get the name for the sound channel to be used for input.
 *
 * This defaults to the value of the PSoundChannel::GetDefaultDevice()
 * function.
 */
const gchar *
gopal_pcss_ep_get_soundchannel_record_device (GopalPCSSEP *self)
{
    return PCSSEP(self)->GetSoundChannelRecordDevice();
}

/**
 * gopal_pcss_ep_set_soundchannel_buffer_time:
 * @self: #GopalPCSSEP instance
 * @depth: buffer depth in milliseconds
 *
 * Set the default sound channel buffer time in milliseconds.
 *
 * Note the largest of the depth in frames and the depth in
 * milliseconds as returned by GetSoundBufferTime() is used.
 */
void
gopal_pcss_ep_set_soundchannel_buffer_time (GopalPCSSEP *self, guint depth)
{
    PCSSEP(self)->SetSoundChannelBufferTime(depth);
}

/**
 * gopal_pcss_ep_get_soundchannel_buffer_time:
 * @self: #GopalPCSSEP instance
 *
 * Returns: the buffer time depth
 *
 * Get the default sound channel buffer time in milliseconds.
 */
guint
gopal_pcss_ep_get_soundchannel_buffer_time (GopalPCSSEP *self)
{
    return PCSSEP(self)->GetSoundChannelBufferTime();
}

/**
 * gopal_pcss_ep_accept_incoming_connection:
 * @self: #GopalPCSSEP instance
 * @token: the connection token
 *
 * Accept the incoming connection.
 *
 * Returns: %FALSE if the connection token does not correspond to a
 * valid connection.
 */
gboolean
gopal_pcss_ep_accept_incoming_connection (GopalPCSSEP *self, const gchar *token)
{
    PString contoken = (token) ? PString(token) : PString::Empty();
    return PCSSEP(self)->AcceptIncomingConnection(token);
}

/**
 * gopal_pcss_ep_reject_incoming_connection:
 * @self: #GopalPCSSEP instance
 * @token: the connection token
 * @reason: the reason to reject the call
 *
 * Reject the incoming connection.
 *
 * Returns: %FALSE if the connection token does not correspond to a
 * valid connection.
 */
gboolean
gopal_pcss_ep_reject_incoming_connection (GopalPCSSEP *self,
                                          const gchar *token,
                                          GopalCallEndReason reason)
{
    PString contoken = (token) ? PString(token) : PString::Empty();
    return PCSSEP(self)->RejectIncomingConnection(
        token,
        OpalConnection::CallEndReason(reason)
        );
}

G_END_DECLS
