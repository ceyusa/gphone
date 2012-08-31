/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

#include "gopalmanager.h"
#include "gopalsipep.h"

#include <ptlib.h>
#include <opal/manager.h>

class MyManager : public OpalManager
{
    PCLASSINFO(MyManager, OpalManager);

public:
    MyManager(GopalManager *mgr) : m_manager(mgr) { };

private:
    GopalManager *m_manager;
};

G_BEGIN_DECLS

enum { PROP_SIPEP = 1, PROP_LAST };

struct _GopalManagerPrivate
{
    MyManager *manager;
    GopalSIPEP *sipep;
};

#define GET_PRIVATE(obj)                                                \
    (G_TYPE_INSTANCE_GET_PRIVATE((obj), GOPAL_TYPE_MANAGER, GopalManagerPrivate))

#define MANAGER(obj)                            \
    (GET_PRIVATE((obj))->manager)

G_DEFINE_TYPE(GopalManager, gopal_manager, G_TYPE_OBJECT)

static void
gopal_manager_finalize (GObject *object)
{
    GopalManager *self = GOPAL_MANAGER (object);

    delete self->priv->manager;
    g_object_unref (self->priv->sipep);

    G_OBJECT_CLASS(gopal_manager_parent_class)->finalize(object);
}

static void
gopal_manager_get_property(GObject *object, guint property_id,
                           GValue *value, GParamSpec *pspec)
{
    GopalManager *self;

    self = GOPAL_MANAGER (object);

    switch (property_id) {
    case PROP_SIPEP:
        g_value_set_object (value, self->priv->sipep);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
gopal_manager_class_init (GopalManagerClass *klass)
{
    GObjectClass *gobject_class = (GObjectClass *) klass;

    gobject_class->get_property = gopal_manager_get_property;
    gobject_class->finalize = gopal_manager_finalize;

    g_type_class_add_private (klass, sizeof (GopalManagerPrivate));

    g_object_class_install_property (gobject_class, PROP_SIPEP,
        g_param_spec_object("sip-endpoint", "sipep", "Opal's SIP end-point",
                            GOPAL_TYPE_SIP_EP,
                            GParamFlags (G_PARAM_READABLE |
                                         G_PARAM_STATIC_STRINGS)));
}

static void
gopal_manager_init (GopalManager *self)
{
    self->priv = GET_PRIVATE (self);
    self->priv->manager = new MyManager(self);

    self->priv->sipep = (GopalSIPEP *) g_object_new (GOPAL_TYPE_SIP_EP,
                                                    "manager", self->priv->manager,
                                                     NULL);
}

GopalManager *
gopal_manager_new ()
{
    return GOPAL_MANAGER (g_object_new (GOPAL_TYPE_MANAGER, NULL));
}

GopalSTUNClientNatType
gopal_manager_set_stun_server (GopalManager *self, const char *server)
{
    PString STUNServer;

    STUNServer = (server) ? PString(server) : PString::Empty();
    return (GopalSTUNClientNatType) MANAGER(self)->SetSTUNServer (STUNServer);
}

const char *
gopal_manager_get_stun_server (GopalManager *self)
{
    return MANAGER (self)->GetSTUNServer ();
}

gboolean
gopal_manager_set_translation_host (GopalManager *self, const char *host)
{
    PString NATRouter;

    NATRouter = (host) ? PString(host) : PString::Empty();
    return MANAGER (self)->SetTranslationHost(NATRouter);
}

G_END_DECLS
