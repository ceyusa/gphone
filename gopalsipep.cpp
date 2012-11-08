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
                  GopalSIPEP * sipep)
        : SIPEndPoint (manager), m_sipep(sipep) { };

    virtual void OnRegistrationStatus(const RegistrationStatus & status);

private:
    GopalSIPEP *m_sipep;
};

void
MySIPEndPoint::OnRegistrationStatus(const RegistrationStatus & status)
{
  SIPEndPoint::OnRegistrationStatus(status);

  if (status.m_reason < 200 || (status.m_reRegistering && status.m_reason < 300))
    return;

  g_signal_emit_by_name (m_sipep, "registration-status",
                         (const char *) status.m_addressofRecord,
                         GopalStatusCodes (status.m_reason));
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
            g_enum_register_static (g_intern_static_string ("GopalStatusCodes"),
                                    values);
        g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
    }

    return g_define_type_id__volatile;
}

enum { PROP_MANAGER = 1, PROP_LAST };

struct _GopalSIPEPPrivate
{
    MySIPEndPoint *sipep;
};

#define GET_PRIVATE(obj)						\
        (G_TYPE_INSTANCE_GET_PRIVATE((obj), GOPAL_TYPE_SIP_EP, GopalSIPEPPrivate))

G_DEFINE_TYPE(GopalSIPEP, gopal_sip_ep, G_TYPE_OBJECT)

static void
construct (GopalSIPEP *self, gpointer ptr)
{
    OpalManager *manager;

    manager = static_cast<OpalManager *> (ptr);
    self->priv->sipep = new MySIPEndPoint(*manager, self);
}

static void
gopal_sip_ep_set_property(GObject *object, guint property_id,
                          const GValue *value, GParamSpec *pspec)
{
    GopalSIPEP *self;

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
gopal_sip_ep_class_init (GopalSIPEPClass *klass)
{
    GObjectClass *gobject_class = (GObjectClass *) klass;

    gobject_class->finalize = gopal_sip_ep_finalize;
    gobject_class->set_property = gopal_sip_ep_set_property;

    g_type_class_add_private (klass, sizeof (GopalSIPEPPrivate));

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
gopal_sip_ep_init (GopalSIPEP *self)
{
    self->priv = GET_PRIVATE (self);
}

/**
 * gopal_sip_ep_register:
 * @self: a #GopalSIPEP instance
 * @params: a #GopalSIPRegisterParams instance
 * @address_of_record: (out): the registered address
 *
 * Register an entity to a registrar.
 *
 * This function is asynchronous to permit several registrations to
 * occur at the same time. It can be called several times for
 * different hosts and users.
 *
 * The @params.m_address_of_record field is the only field required,
 * though typically @params.password is also required. A registration
 * for the user part of @params.address_of_record is made to the a
 * registrar associated with the domain part of the field. The
 * authentication identity is the same as the user field, though this
 * may be set to soemthing different via the @params.auth_ID field.
 *
 * The @params.registrar_address may indicate the specific hostname to
 * use for the registrar rather than using the domain part of
 * @params.address_of_record field.
 *
 * To aid in flexibility if the @params.address_of_record does not
 * contain a domain and the @params.registrar_address does, the the
 * @address_of_record is constructed from them.
 *
 * The @params.realm can be specified when registering, this will
 * allow to find the correct authentication information when being
 * requested. If no realm is specified, authentication will occur with
 * the "best guess" of authentication parameters.
 *
 * The Contact address is normally constructed from the listeners
 * active on the #GopalSIPEP. This may be overridden to an explicit
 * value via the @params.contact_address field.
 *
 * The returned "token" is a string that can be used in functions such
 * as gopal_sip_ep_unregister() or gopal_sip_ep_is_registered(). While
 * it possible to use the @address_of_record for those functions, it
 * is not recommended as
 *
 * a) there may be more than one registration for an @address_of_record and
 *
 * b) the @address_of_record may be constructed from
 *
 */
gboolean
gopal_sip_ep_register (GopalSIPEP *self,
                       GopalSIPRegisterParams *params,
                       const gchar **address_of_record)
{
    gboolean ret;
    PString aor;
    SIPRegister::Params sip_params;
    GopalSIPParams *p = &params->params;

    sip_params.m_addressOfRecord = p->address_of_record;
    sip_params.m_registrarAddress = params->registrar_address;
    sip_params.m_authID = p->auth_ID;
    sip_params.m_password = p->password;
    sip_params.m_expire = p->expire;

    ret = self->priv->sipep->Register (sip_params, aor);

    *address_of_record = (const gchar *) aor;

    return ret;
}

/**
 * gopal_sip_ep_start_listeners:
 * @self: a #GopalSIPEP instance
 * @interfaces: (array zero-terminated=1): interfaces to listen
 *
 * Add a set of listeners to the endoint.
 *
 * This allows for the automatic creating of incoming call
 * connections. If the list is empty then
 * gopal_sip_ep_get_default_listeners() is used.
 *
 * Note: while the @interfaces parameter is a string array, each
 * element of the array should be compatible with
 * OpalTransportAddress.
 *
 * See OpalTransportAddress for more details on the syntax of an
 * interface definition.
 */

gboolean
gopal_sip_ep_start_listeners (GopalSIPEP *self, gchar **interfaces)
{
        PStringArray listenerAddresses;

        if (interfaces) {
                int i;
                for (i = 0; interfaces[i] != NULL; i++)
			listenerAddresses = PStringArray(i, interfaces);
        }

        return self->priv->sipep->StartListeners (listenerAddresses);
}


G_END_DECLS
