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
#include "gopalenum.h"

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

  SIPURL aor = status.m_addressofRecord;
  aor.Sanitise(SIPURL::ExternalURI);

  PString aor_str = aor.AsString ();
  char *address_of_record = g_strdup (aor_str);

  g_signal_emit_by_name (m_sipep, "registration-status",
                         address_of_record,
                         gboolean (status.m_wasRegistering),
                         GopalStatusCodes (status.m_reason));
}

G_BEGIN_DECLS

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

    /**
     * GopalSIPEP:registration-status:
     * @aor: address of record
     * @registering: %TRUE if registering is in process
     * @reason: the status of the registration process
     *
     * The ::registration-status signal is emitted each time a
     * registration status is updated.
     */
    g_signal_new("registration-status",
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
                 G_TYPE_BOOLEAN,
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
 * @address_of_record: (out) (transfer full) (allow-none): the
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
                       gchar **address_of_record,
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

    ret = self->priv->sipep->Register (sip_params, aor,
                                       (SIP_PDU::StatusCodes *)reason);

    *address_of_record = g_strdup ((const gchar *) aor);

    return ret;
}

/**
 * gopal_sip_ep_is_registered:
 * @self: a #GopalSIPEP instance
 * @aor: the address of record
 * @offline: ensure if it is unregistered
 *
 * Determine if there is a registration for the entity.
 *
 * The "token" parameter string is typically the string returned by
 * the gopal_sip_ep_register() function which is guaranteed to
 * uniquely identify the specific registration.
 *
 * For backward compatibility, the AOR can also be used, but as it is
 * possible to have two registrations to the same AOR, this should be
 * avoided.
 *
 * The @offline parameter indicates if the caller is interested
 * in if we are, to the best of our knowledge, currently registered
 * (have had recent confirmation) or we are not sure if we are
 * registered or not, but are continually re-trying.
 */
gboolean
gopal_sip_ep_is_registered (GopalSIPEP *self,
                            const gchar *aor,
                            gboolean offline)
{
    PString token = (aor) ? PString(aor) : PString::Empty();
    return self->priv->sipep->IsRegistered (token, offline);
}


/**
 * gopal_sip_ep_unregister:
 * @self: the instance of #GopalSIPEP
 * @aor: the address of record to unregister
 *
 * Unregister the address-of-record from a registrar.
 *
 * The "token" parameter string is typically the string returned by
 * the gopal_sip_ep_register() function which is guaranteed to
 * uniquely identify the specific registration.
 *
 * For backward compatibility, the AOR can also be used, but as it is
 * possible to have two registrations to the same AOR, this should be
 * avoided.
 */
gboolean
gopal_sip_ep_unregister (GopalSIPEP *self, const gchar *aor)
{
    PString token = (aor) ? PString(aor) : PString::Empty();
    return self->priv->sipep->Unregister (token);
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
