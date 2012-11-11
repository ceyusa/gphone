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

GType
gopal_status_codes_get_type (void)
{
    static volatile gsize g_define_type_id__volatile = 0;

    if (g_once_init_enter (&g_define_type_id__volatile)) {
        static const GEnumValue values[] = {
            { GOPAL_STATUS_CODE_ILLEGALSTATUSCODE, "GOPAL_STATUS_CODE_ILLEGALSTATUSCODE", "illegalstatuscode" },
            { GOPAL_STATUS_CODE_LOCAL_TRANSPORTERROR, "GOPAL_STATUS_CODE_LOCAL_TRANSPORTERROR", "local-transporterror" },
            { GOPAL_STATUS_CODE_LOCAL_BADTRANSPORTADDRESS, "GOPAL_STATUS_CODE_LOCAL_BADTRANSPORTADDRESS", "local-badtransportaddress" },
            { GOPAL_STATUS_CODE_LOCAL_TIMEOUT, "GOPAL_STATUS_CODE_LOCAL_TIMEOUT", "local-timeout" },
            { GOPAL_STATUS_CODE_INFORMATION_TRYING, "GOPAL_STATUS_CODE_INFORMATION_TRYING", "information-trying" },
            { GOPAL_STATUS_CODE_INFORMATION_RINGING, "GOPAL_STATUS_CODE_INFORMATION_RINGING", "information-ringing" },
            { GOPAL_STATUS_CODE_INFORMATION_CALLFORWARDED, "GOPAL_STATUS_CODE_INFORMATION_CALLFORWARDED", "information-callforwarded" },
            { GOPAL_STATUS_CODE_INFORMATION_QUEUED, "GOPAL_STATUS_CODE_INFORMATION_QUEUED", "information-queued" },
            { GOPAL_STATUS_CODE_INFORMATION_SESSION_PROGRESS, "GOPAL_STATUS_CODE_INFORMATION_SESSION_PROGRESS", "information-session-progress" },
            { GOPAL_STATUS_CODE_SUCCESS_OK, "GOPAL_STATUS_CODE_SUCCESS_OK", "success-ok" },
            { GOPAL_STATUS_CODE_SUCCESS_ACCEPTED, "GOPAL_STATUS_CODE_SUCCESS_ACCEPTED", "success-accepted" },
            { GOPAL_STATUS_CODE_REDIR_MULTIPLECHOICES, "GOPAL_STATUS_CODE_REDIR_MULTIPLECHOICES", "redir-multiplechoices" },
            { GOPAL_STATUS_CODE_REDIR_MOVEDPERMANENTLY, "GOPAL_STATUS_CODE_REDIR_MOVEDPERMANENTLY", "redir-movedpermanently" },
            { GOPAL_STATUS_CODE_REDIR_MOVEDTEMPORARILY, "GOPAL_STATUS_CODE_REDIR_MOVEDTEMPORARILY", "redir-movedtemporarily" },
            { GOPAL_STATUS_CODE_REDIR_USEPROXY, "GOPAL_STATUS_CODE_REDIR_USEPROXY", "redir-useproxy" },
            { GOPAL_STATUS_CODE_REDIR_ALTERNATIVESERVICE, "GOPAL_STATUS_CODE_REDIR_ALTERNATIVESERVICE", "redir-alternativeservice" },
            { GOPAL_STATUS_CODE_FAIL_BADREQUEST, "GOPAL_STATUS_CODE_FAIL_BADREQUEST", "fail-badrequest" },
            { GOPAL_STATUS_CODE_FAIL_UNAUTHORISED, "GOPAL_STATUS_CODE_FAIL_UNAUTHORISED", "fail-unauthorised" },
            { GOPAL_STATUS_CODE_FAIL_PAYMENTREQUIRED, "GOPAL_STATUS_CODE_FAIL_PAYMENTREQUIRED", "fail-paymentrequired" },
            { GOPAL_STATUS_CODE_FAIL_FORBIDDEN, "GOPAL_STATUS_CODE_FAIL_FORBIDDEN", "fail-forbidden" },
            { GOPAL_STATUS_CODE_FAIL_NOTFOUND, "GOPAL_STATUS_CODE_FAIL_NOTFOUND", "fail-notfound" },
            { GOPAL_STATUS_CODE_FAIL_METHODNOTALLOWED, "GOPAL_STATUS_CODE_FAIL_METHODNOTALLOWED", "fail-methodnotallowed" },
            { GOPAL_STATUS_CODE_FAIL_NOTACCEPTABLE, "GOPAL_STATUS_CODE_FAIL_NOTACCEPTABLE", "fail-notacceptable" },
            { GOPAL_STATUS_CODE_FAIL_PROXYAUTHENTICATIONREQUIRED, "GOPAL_STATUS_CODE_FAIL_PROXYAUTHENTICATIONREQUIRED", "fail-proxyauthenticationrequired" },
            { GOPAL_STATUS_CODE_FAIL_REQUESTTIMEOUT, "GOPAL_STATUS_CODE_FAIL_REQUESTTIMEOUT", "fail-requesttimeout" },
            { GOPAL_STATUS_CODE_FAIL_CONFLICT, "GOPAL_STATUS_CODE_FAIL_CONFLICT", "fail-conflict" },
            { GOPAL_STATUS_CODE_FAIL_GONE, "GOPAL_STATUS_CODE_FAIL_GONE", "fail-gone" },
            { GOPAL_STATUS_CODE_FAIL_LENGTHREQUIRED, "GOPAL_STATUS_CODE_FAIL_LENGTHREQUIRED", "fail-lengthrequired" },
            { GOPAL_STATUS_CODE_FAIL_REQUESTENTITYTOOLARGE, "GOPAL_STATUS_CODE_FAIL_REQUESTENTITYTOOLARGE", "fail-requestentitytoolarge" },
            { GOPAL_STATUS_CODE_FAIL_REQUESTURITOOLONG, "GOPAL_STATUS_CODE_FAIL_REQUESTURITOOLONG", "fail-requesturitoolong" },
            { GOPAL_STATUS_CODE_FAIL_UNSUPPORTEDMEDIATYPE, "GOPAL_STATUS_CODE_FAIL_UNSUPPORTEDMEDIATYPE", "fail-unsupportedmediatype" },
            { GOPAL_STATUS_CODE_FAIL_UNSUPPORTEDURISCHEME, "GOPAL_STATUS_CODE_FAIL_UNSUPPORTEDURISCHEME", "fail-unsupportedurischeme" },
            { GOPAL_STATUS_CODE_FAIL_BADEXTENSION, "GOPAL_STATUS_CODE_FAIL_BADEXTENSION", "fail-badextension" },
            { GOPAL_STATUS_CODE_FAIL_EXTENSIONREQUIRED, "GOPAL_STATUS_CODE_FAIL_EXTENSIONREQUIRED", "fail-extensionrequired" },
            { GOPAL_STATUS_CODE_FAIL_INTERVALTOOBRIEF, "GOPAL_STATUS_CODE_FAIL_INTERVALTOOBRIEF", "fail-intervaltoobrief" },
            { GOPAL_STATUS_CODE_FAIL_TEMPORARILYUNAVAILABLE, "GOPAL_STATUS_CODE_FAIL_TEMPORARILYUNAVAILABLE", "fail-temporarilyunavailable" },
            { GOPAL_STATUS_CODE_FAIL_TRANSACTIONDOESNOTEXIST, "GOPAL_STATUS_CODE_FAIL_TRANSACTIONDOESNOTEXIST", "fail-transactiondoesnotexist" },
            { GOPAL_STATUS_CODE_FAIL_LOOPDETECTED, "GOPAL_STATUS_CODE_FAIL_LOOPDETECTED", "fail-loopdetected" },
            { GOPAL_STATUS_CODE_FAIL_TOOMANYHOPS, "GOPAL_STATUS_CODE_FAIL_TOOMANYHOPS", "fail-toomanyhops" },
            { GOPAL_STATUS_CODE_FAIL_ADDRESSINCOMPLETE, "GOPAL_STATUS_CODE_FAIL_ADDRESSINCOMPLETE", "fail-addressincomplete" },
            { GOPAL_STATUS_CODE_FAIL_AMBIGUOUS, "GOPAL_STATUS_CODE_FAIL_AMBIGUOUS", "fail-ambiguous" },
            { GOPAL_STATUS_CODE_FAIL_BUSYHERE, "GOPAL_STATUS_CODE_FAIL_BUSYHERE", "fail-busyhere" },
            { GOPAL_STATUS_CODE_FAIL_REQUESTTERMINATED, "GOPAL_STATUS_CODE_FAIL_REQUESTTERMINATED", "fail-requestterminated" },
            { GOPAL_STATUS_CODE_FAIL_NOTACCEPTABLEHERE, "GOPAL_STATUS_CODE_FAIL_NOTACCEPTABLEHERE", "fail-notacceptablehere" },
            { GOPAL_STATUS_CODE_FAIL_BADEVENT, "GOPAL_STATUS_CODE_FAIL_BADEVENT", "fail-badevent" },
            { GOPAL_STATUS_CODE_FAIL_REQUESTPENDING, "GOPAL_STATUS_CODE_FAIL_REQUESTPENDING", "fail-requestpending" },
            { GOPAL_STATUS_CODE_FAIL_UNDECIPHERABLE, "GOPAL_STATUS_CODE_FAIL_UNDECIPHERABLE", "fail-undecipherable" },
            { GOPAL_STATUS_CODE_FAIL_INTERNALSERVERERROR, "GOPAL_STATUS_CODE_FAIL_INTERNALSERVERERROR", "fail-internalservererror" },
            { GOPAL_STATUS_CODE_FAIL_NOTIMPLEMENTED, "GOPAL_STATUS_CODE_FAIL_NOTIMPLEMENTED", "fail-notimplemented" },
            { GOPAL_STATUS_CODE_FAIL_BADGATEWAY, "GOPAL_STATUS_CODE_FAIL_BADGATEWAY", "fail-badgateway" },
            { GOPAL_STATUS_CODE_FAIL_SERVICEUNAVAILABLE, "GOPAL_STATUS_CODE_FAIL_SERVICEUNAVAILABLE", "fail-serviceunavailable" },
            { GOPAL_STATUS_CODE_FAIL_SERVERTIMEOUT, "GOPAL_STATUS_CODE_FAIL_SERVERTIMEOUT", "fail-servertimeout" },
            { GOPAL_STATUS_CODE_FAIL_SIPVERSIONNOTSUPPORTED, "GOPAL_STATUS_CODE_FAIL_SIPVERSIONNOTSUPPORTED", "fail-sipversionnotsupported" },
            { GOPAL_STATUS_CODE_FAIL_MESSAGETOOLARGE, "GOPAL_STATUS_CODE_FAIL_MESSAGETOOLARGE", "fail-messagetoolarge" },
            { GOPAL_STATUS_CODE_GLOBALFAIL_BUSYEVERYWHERE, "GOPAL_STATUS_CODE_GLOBALFAIL_BUSYEVERYWHERE", "globalfail-busyeverywhere" },
            { GOPAL_STATUS_CODE_GLOBALFAIL_DECLINE, "GOPAL_STATUS_CODE_GLOBALFAIL_DECLINE", "globalfail-decline" },
            { GOPAL_STATUS_CODE_GLOBALFAIL_DOESNOTEXISTANYWHERE, "GOPAL_STATUS_CODE_GLOBALFAIL_DOESNOTEXISTANYWHERE", "globalfail-doesnotexistanywhere" },
            { GOPAL_STATUS_CODE_GLOBALFAIL_NOTACCEPTABLE, "GOPAL_STATUS_CODE_GLOBALFAIL_NOTACCEPTABLE", "globalfail-notacceptable" },
            { GOPAL_STATUS_CODE_MAX, "GOPAL_STATUS_CODE_MAX", "max" },
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

static inline glong
usec_2_msec (glong usec)
{
    if (usec == -1)
        return 0x7fffffff; // PMaxTimeInterval

    if (usec > 0)
        return usec * G_GINT64_CONSTANT (1000) / G_USEC_PER_SEC;

    return 0;
}

/**
 * gopal_sip_ep_register:
 * @self: a #GopalSIPEP instance
 * @params: (in): a #GopalSIPRegisterParams instance
 * @address_of_record: (out) (transfer none) (allow-none): the
 * registered address
 * @reason: (out) (allow-none): if not %NULL the function will be
 * synchronous and its value will be the result code of the operation
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
                       const gchar **address_of_record,
                       GopalStatusCodes *reason)
{
    gboolean ret;
    PString aor;
    SIPRegister::Params sip_params;
    GopalSIPParams *p = &params->params;

    sip_params.m_remoteAddress = p->remote_address;
    sip_params.m_localAddress = p->local_address;
    sip_params.m_proxyAddress = p->proxy_address;
    sip_params.m_addressOfRecord = p->address_of_record;
    sip_params.m_contactAddress = p->contact_address;
    sip_params.m_authID = p->auth_ID;
    sip_params.m_password = p->password;
    sip_params.m_realm = p->realm;
    sip_params.m_expire = p->expire;
    sip_params.m_restoreTime = p->restore;
    sip_params.m_minRetryTime.SetInterval (usec_2_msec (p->min_retry.tv_usec),
					   p->min_retry.tv_sec);
    sip_params.m_maxRetryTime.SetInterval (usec_2_msec (p->max_retry.tv_usec),
					   p->max_retry.tv_sec);
    sip_params.m_userData = p->user_data;
    sip_params.m_registrarAddress = p->remote_address;
    sip_params.m_compatibility = SIPRegister::CompatibilityModes (params->compatibility);

    ret = self->priv->sipep->Register (sip_params, aor, reason);

    *address_of_record = (const gchar *) aor;

    return ret;
}

/**
 * gopal_sip_ep_start_listeners:
 * @self: a #GopalSIPEP instance
 * @interfaces: (array zero-terminated=1) (allow-none):
 *  interfaces to listen
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
