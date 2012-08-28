/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

#include "gopalsipep.h"

#include <ptlib.h>
#include <sip/sip.h>

class MySIPEndPoint : public SIPEndPoint
{
    PCLASSINFO(MySIPEndPoint, SIPEndPoint);

public:
    MySIPEndPoint(OpalManager & manager,
                  GOpalSIPEP * sipep)
        : SIPEndPoint (manager), m_sipep(sipep) { };

private:
    GOpalSIPEP *m_sipep;
};

G_BEGIN_DECLS

enum { PROP_MANAGER = 1, PROP_LAST };

struct _GOpalSIPEPPrivate
{
    MySIPEndPoint *sipep;
};

#define GET_PRIVATE(obj)						\
        (G_TYPE_INSTANCE_GET_PRIVATE((obj), GOPAL_TYPE_SIP_EP, GOpalSIPEPPrivate))

G_DEFINE_TYPE(GOpalSIPEP, gopal_sip_ep, G_TYPE_OBJECT)

static void
construct (GOpalSIPEP *self, gpointer ptr)
{
    OpalManager *manager;

    manager = static_cast<OpalManager *> (ptr);
    self->priv->sipep = new MySIPEndPoint(*manager, self);
}

static void
gopal_sip_ep_set_property(GObject *object, guint property_id,
                          const GValue *value, GParamSpec *pspec)
{
    GOpalSIPEP *self;

    self = GOPAL_SIP_EP (object);

    switch (property_id) {
    case PROP_MANAGER:
        if (!self->priv->sipep)
            construct (self, g_value_get_pointer (value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}


static void
gopal_sip_ep_finalize (GObject *object)
{
    G_OBJECT_CLASS(gopal_sip_ep_parent_class)->finalize(object);
}

static void
gopal_sip_ep_class_init (GOpalSIPEPClass *klass)
{
    GObjectClass *gobject_class = (GObjectClass *) klass;

    gobject_class->finalize = gopal_sip_ep_finalize;
    gobject_class->set_property = gopal_sip_ep_set_property;

    g_type_class_add_private (klass, sizeof (GOpalSIPEPPrivate));

    g_object_class_install_property (gobject_class, PROP_MANAGER,
        g_param_spec_pointer("manager", "mgr", "Opal's Manager",
                             GParamFlags (G_PARAM_WRITABLE |
                                          G_PARAM_CONSTRUCT_ONLY |
                                          G_PARAM_STATIC_NAME)));
}

static void
gopal_sip_ep_init (GOpalSIPEP *self)
{
    self->priv = GET_PRIVATE (self);
}

gboolean
gopal_sip_ep_register (GOpalSIPEP *self,
                       GOpalSIPRegisterParams *params,
                       gchar **address_of_record)
{
    gboolean ret;
    PString aor;
    SIPRegister::Params sip_params;

    sip_params.m_addressOfRecord = params->user;
    sip_params.m_registrarAddress = params->domain;
    sip_params.m_authID = params->auth_name;
    sip_params.m_password = params->password;
    sip_params.m_expire = params->ttl;

    ret = self->priv->sipep->Register (sip_params, aor);

    *address_of_record = strdup((const char *) aor);

    return ret;
}

G_END_DECLS
