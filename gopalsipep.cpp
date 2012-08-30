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

    virtual void OnRegistrationStatus(const RegistrationStatus & status);

private:
    GOpalSIPEP *m_sipep;
};

void
MySIPEndPoint::OnRegistrationStatus(const RegistrationStatus & status)
{
  SIPEndPoint::OnRegistrationStatus(status);

  if (status.m_reason < 200 || (status.m_reRegistering && status.m_reason < 300))
    return;

  g_signal_emit_by_name (m_sipep, "registration-status",
                         (const char *) status.m_addressofRecord,
                         GOpalStatusCodes (status.m_reason));
}

G_BEGIN_DECLS

static GType
gopal_status_codes_get_type (void)
{
    static volatile gsize g_define_type_id__volatile = 0;

    if (g_once_init_enter (&g_define_type_id__volatile)) {
        static const GEnumValue values[] = {
            { GOPAL_STATUS_CODE_ILLEGALSTATUSCODE, NULL, NULL },
            { GOPAL_STATUS_CODE_LOCAL_TRANSPORTERROR, NULL, NULL },
            { GOPAL_STATUS_CODE_LOCAL_BADTRANSPORTADDRESS, NULL, NULL },
            { GOPAL_STATUS_CODE_LOCAL_TIMEOUT, NULL, NULL },

            { GOPAL_STATUS_CODE_INFORMATION_TRYING, NULL, NULL },
            { GOPAL_STATUS_CODE_INFORMATION_RINGING, NULL, NULL },
            { GOPAL_STATUS_CODE_INFORMATION_CALLFORWARDED, NULL, NULL },
            { GOPAL_STATUS_CODE_INFORMATION_QUEUED, NULL, NULL },
            { GOPAL_STATUS_CODE_INFORMATION_SESSION_PROGRESS, NULL, NULL },

            { GOPAL_STATUS_CODE_SUCCESS_OK, NULL, NULL },
            { GOPAL_STATUS_CODE_SUCCESS_ACCEPTED, NULL, NULL },

            { GOPAL_STATUS_CODE_REDIR_MULTIPLECHOICES, NULL, NULL },
            { GOPAL_STATUS_CODE_REDIR_MOVEDPERMANENTLY, NULL, NULL },
            { GOPAL_STATUS_CODE_REDIR_MOVEDTEMPORARILY, NULL, NULL },
            { GOPAL_STATUS_CODE_REDIR_USEPROXY, NULL, NULL },
            { GOPAL_STATUS_CODE_REDIR_ALTERNATIVESERVICE, NULL, NULL },

            { GOPAL_STATUS_CODE_FAIL_BADREQUEST, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_UNAUTHORISED, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_PAYMENTREQUIRED, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_FORBIDDEN, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_NOTFOUND, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_METHODNOTALLOWED, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_NOTACCEPTABLE, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_PROXYAUTHENTICATIONREQUIRED, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_REQUESTTIMEOUT, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_CONFLICT, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_GONE, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_LENGTHREQUIRED, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_REQUESTENTITYTOOLARGE, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_REQUESTURITOOLONG, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_UNSUPPORTEDMEDIATYPE, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_UNSUPPORTEDURISCHEME, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_BADEXTENSION, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_EXTENSIONREQUIRED, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_INTERVALTOOBRIEF, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_TEMPORARILYUNAVAILABLE, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_TRANSACTIONDOESNOTEXIST, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_LOOPDETECTED, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_TOOMANYHOPS, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_ADDRESSINCOMPLETE, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_AMBIGUOUS, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_BUSYHERE, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_REQUESTTERMINATED, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_NOTACCEPTABLEHERE, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_BADEVENT, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_REQUESTPENDING, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_UNDECIPHERABLE, NULL, NULL },

            { GOPAL_STATUS_CODE_FAIL_INTERNALSERVERERROR, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_NOTIMPLEMENTED, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_BADGATEWAY, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_SERVICEUNAVAILABLE, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_SERVERTIMEOUT, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_SIPVERSIONNOTSUPPORTED, NULL, NULL },
            { GOPAL_STATUS_CODE_FAIL_MESSAGETOOLARGE, NULL, NULL },

            { GOPAL_STATUS_CODE_GLOBALFAIL_BUSYEVERYWHERE, NULL, NULL },
            { GOPAL_STATUS_CODE_GLOBALFAIL_DECLINE, NULL, NULL },
            { GOPAL_STATUS_CODE_GLOBALFAIL_DOESNOTEXISTANYWHERE, NULL, NULL },
            { GOPAL_STATUS_CODE_GLOBALFAIL_NOTACCEPTABLE, NULL, NULL },

            { GOPAL_STATUS_CODE_MAX, NULL, NULL },
            { 0, NULL, NULL }
        };
        GType g_define_type_id =
            g_enum_register_static (g_intern_static_string ("GOpalStatusCodes"),
                                    values);
        g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
    }

    return g_define_type_id__volatile;
}

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

    g_signal_new("registration-status",
                 G_TYPE_FROM_CLASS (klass),
                 GSignalFlags (G_SIGNAL_RUN_LAST |
                               G_SIGNAL_NO_RECURSE |
                               G_SIGNAL_NO_HOOKS),
                 0,
                 NULL, NULL,
                 NULL,
                 G_TYPE_NONE,
                 2,
                 G_TYPE_STRING,
                 gopal_status_codes_get_type ());
}

static void
gopal_sip_ep_init (GOpalSIPEP *self)
{
    self->priv = GET_PRIVATE (self);
}

gboolean
gopal_sip_ep_register (GOpalSIPEP *self,
                       GOpalSIPRegisterParams *params,
                       const gchar **address_of_record)
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

    *address_of_record = (const char *) aor;

    return ret;
}
gboolean
gopal_sip_ep_start_listeners (GOpalSIPEP *self, gchar **listeners)
{
        PStringArray listenerAddresses;

        if (listeners) {
                int i;

                for (i = 0; listeners[i] != NULL; i++);
                listenerAddresses = PStringArray(i, listeners);
        }

        return self->priv->sipep->StartListeners (listenerAddresses);
}


G_END_DECLS
