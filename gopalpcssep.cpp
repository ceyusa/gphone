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

    virtual PSoundChannel *CreateSoundChannel(const OpalPCSSConnection & connection,
                                              const OpalMediaFormat & mediaFormat,
                                              PBoolean isSource);

private:
    virtual PBoolean OnShowIncoming(const OpalPCSSConnection & connection);
    virtual PBoolean OnShowOutgoing(const OpalPCSSConnection & connection);

    GopalPCSSEP *m_pcssep;
};

PBoolean
MyPCSSEndPoint::OnShowIncoming(const OpalPCSSConnection & connection)
{
    const gchar *token = connection.GetCall().GetToken();
    g_signal_emit_by_name(m_pcssep, "call-incoming", token);
    return true;
}


PBoolean MyPCSSEndPoint::OnShowOutgoing(const OpalPCSSConnection & connection)
{
    const gchar *remote_name = connection.GetRemotePartyName();
    g_signal_emit_by_name(m_pcssep, "call-outgoing", remote_name);
    return true;
}


PSoundChannel *
MyPCSSEndPoint::CreateSoundChannel(const OpalPCSSConnection & connection,
                                   const OpalMediaFormat & mediaFormat,
                                   PBoolean isSource)
{
  PSoundChannel *channel = OpalPCSSEndPoint::CreateSoundChannel(connection,
                                                                mediaFormat,
                                                                isSource);
  if (channel != NULL)
      return channel;

  // handle error:
  //
  // LogWindow << "Could not open ";
  // if (isSource)
  //   LogWindow << "record device \"" << connection.GetSoundChannelRecordDevice();
  // else
  //   LogWindow << "player device \"" << connection.GetSoundChannelPlayDevice();
  // LogWindow << '"' << endl;

  return NULL;
}

G_BEGIN_DECLS

enum { PROP_MANAGER = 1, PROP_LAST };

struct _GopalPCSSEPPrivate
{
    MyPCSSEndPoint *pcssep;
};

#define GET_PRIVATE(obj)						\
        (G_TYPE_INSTANCE_GET_PRIVATE((obj), GOPAL_TYPE_PCSS_EP, GopalPCSSEPPrivate))

G_DEFINE_TYPE(GopalPCSSEP, gopal_pcss_ep, G_TYPE_OBJECT)

static void
construct (GopalPCSSEP *self, gpointer ptr)
{
    OpalManager *manager;

    manager = static_cast<OpalManager *> (ptr);
    self->priv->pcssep = new MyPCSSEndPoint(*manager, self);
}

static void
gopal_pcss_ep_set_property(GObject *object, guint property_id,
                          const GValue *value, GParamSpec *pspec)
{
    GopalPCSSEP *self;

    self = GOPAL_PCSS_EP (object);

    switch (property_id) {
    case PROP_MANAGER:
        if (!self->priv->pcssep)
            construct (self, g_value_get_pointer (value));
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

    g_type_class_add_private (klass, sizeof (GopalPCSSEPPrivate));

    g_object_class_install_property (gobject_class, PROP_MANAGER,
        g_param_spec_pointer("manager", "mgr", "Opal's Manager",
                             GParamFlags (G_PARAM_WRITABLE |
                                          G_PARAM_CONSTRUCT_ONLY |
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
                 1,
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
    self->priv = GET_PRIVATE (self);
}

G_END_DECLS
