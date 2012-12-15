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
#include "gopalpcssep.h"

#include <ptlib.h>
#include <opal/manager.h>

class MyManager : public OpalManager
{
    PCLASSINFO(MyManager, OpalManager);

public:
    MyManager(GopalManager *mgr) : m_manager(mgr) { };

private:
    virtual void OnEstablishedCall(OpalCall & call);
    virtual void OnClearedCall(OpalCall & call);

    GopalManager *m_manager;
};

void
MyManager::OnEstablishedCall(OpalCall & call)
{
    const gchar *token = call.GetToken();
    g_signal_emit_by_name (m_manager, "call-established", token);
}

void MyManager::OnClearedCall(OpalCall & call)
{
    OpalManager::OnClearedCall(call);
    const gchar *token = call.GetToken();
    const gchar *name = call.GetPartyB().IsEmpty() ? call.GetPartyA() : call.GetPartyB();
    OpalConnection::CallEndReason endreason = call.GetCallEndReason();
    GopalCallEndReason reason = GopalCallEndReason(endreason.code);
    g_signal_emit_by_name (m_manager, "call-cleared", token, name, reason);
}

G_BEGIN_DECLS

GType
gopal_call_end_reason_get_type (void)
{
    static volatile gsize g_define_type_id__volatile = 0;

    if (g_once_init_enter (&g_define_type_id__volatile)) {
        static const GEnumValue values[] = {
            { GOPAL_CALL_END_REASON_LOCALUSER,
              "GOPAL_CALL_END_REASON_LOCALUSER",
              "localuser" },
            { GOPAL_CALL_END_REASON_NOACCEPT,
              "GOPAL_CALL_END_REASON_NOACCEPT",
              "noaccept" },
            { GOPAL_CALL_END_REASON_ANSWERDENIED,
              "GOPAL_CALL_END_REASON_ANSWERDENIED",
              "answerdenied" },
            { GOPAL_CALL_END_REASON_REMOTEUSER,
              "GOPAL_CALL_END_REASON_REMOTEUSER",
              "remoteuser" },
            { GOPAL_CALL_END_REASON_REFUSAL,
              "GOPAL_CALL_END_REASON_REFUSAL",
              "refusal" },
            { GOPAL_CALL_END_REASON_NOANSWER,
              "GOPAL_CALL_END_REASON_NOANSWER",
              "noanswer" },
            { GOPAL_CALL_END_REASON_CALLERABORT,
              "GOPAL_CALL_END_REASON_CALLERABORT",
              "callerabort" },
            { GOPAL_CALL_END_REASON_TRANSPORTFAIL,
              "GOPAL_CALL_END_REASON_TRANSPORTFAIL",
              "transportfail" },
            { GOPAL_CALL_END_REASON_CONNECTFAIL,
              "GOPAL_CALL_END_REASON_CONNECTFAIL",
              "connectfail" },
            { GOPAL_CALL_END_REASON_GATEKEEPER,
              "GOPAL_CALL_END_REASON_GATEKEEPER",
              "gatekeeper" },
            { GOPAL_CALL_END_REASON_NOUSER,
              "GOPAL_CALL_END_REASON_NOUSER",
              "nouser" },
            { GOPAL_CALL_END_REASON_NOBANDWIDTH,
              "GOPAL_CALL_END_REASON_NOBANDWIDTH",
              "nobandwidth" },
            { GOPAL_CALL_END_REASON_CAPABILITYEXCHANGE,
              "GOPAL_CALL_END_REASON_CAPABILITYEXCHANGE",
              "capabilityexchange" },
            { GOPAL_CALL_END_REASON_CALLFORWARDED,
              "GOPAL_CALL_END_REASON_CALLFORWARDED",
              "callforwarded" },
            { GOPAL_CALL_END_REASON_SECURITYDENIAL,
              "GOPAL_CALL_END_REASON_SECURITYDENIAL",
              "securitydenial" },
            { GOPAL_CALL_END_REASON_LOCALBUSY,
              "GOPAL_CALL_END_REASON_LOCALBUSY",
              "localbusy" },
            { GOPAL_CALL_END_REASON_LOCALCONGESTION,
              "GOPAL_CALL_END_REASON_LOCALCONGESTION",
              "localcongestion" },
            { GOPAL_CALL_END_REASON_REMOTEBUSY,
              "GOPAL_CALL_END_REASON_REMOTEBUSY",
              "remotebusy" },
            { GOPAL_CALL_END_REASON_REMOTECONGESTION,
              "GOPAL_CALL_END_REASON_REMOTECONGESTION",
              "remotecongestion" },
            { GOPAL_CALL_END_REASON_UNREACHABLE,
              "GOPAL_CALL_END_REASON_UNREACHABLE",
              "unreachable" },
            { GOPAL_CALL_END_REASON_NOENDPOINT,
              "GOPAL_CALL_END_REASON_NOENDPOINT",
              "noendpoint" },
            { GOPAL_CALL_END_REASON_HOSTOFFLINE,
              "GOPAL_CALL_END_REASON_HOSTOFFLINE",
              "hostoffline" },
            { GOPAL_CALL_END_REASON_TEMPORARYFAILURE,
              "GOPAL_CALL_END_REASON_TEMPORARYFAILURE",
              "temporaryfailure" },
            { GOPAL_CALL_END_REASON_Q931CAUSE,
              "GOPAL_CALL_END_REASON_Q931CAUSE",
              "q931cause" },
            { GOPAL_CALL_END_REASON_DURATIONLIMIT,
              "GOPAL_CALL_END_REASON_DURATIONLIMIT",
              "durationlimit" },
            { GOPAL_CALL_END_REASON_INVALIDCONFERENCEID,
              "GOPAL_CALL_END_REASON_INVALIDCONFERENCEID",
              "invalidconferenceid" },
            { GOPAL_CALL_END_REASON_NODIALTONE,
              "GOPAL_CALL_END_REASON_NODIALTONE",
              "nodialtone" },
            { GOPAL_CALL_END_REASON_NORINGBACKTONE,
              "GOPAL_CALL_END_REASON_NORINGBACKTONE",
              "noringbacktone" },
            { GOPAL_CALL_END_REASON_OUTOFSERVICE,
              "GOPAL_CALL_END_REASON_OUTOFSERVICE",
              "outofservice" },
            { GOPAL_CALL_END_REASON_ACCEPTINGCALLWAITING,
              "GOPAL_CALL_END_REASON_ACCEPTINGCALLWAITING",
              "acceptingcallwaiting" },
            { GOPAL_CALL_END_REASON_GKADMISSIONFAILED,
              "GOPAL_CALL_END_REASON_GKADMISSIONFAILED",
              "gkadmissionfailed" },
            { GOPAL_CALL_END_REASON_MAX,
              "GOPAL_CALL_END_REASON_MAX",
              "max" },
            { 0, NULL, NULL }
        };
        GType g_define_type_id =
            g_enum_register_static (g_intern_static_string
                                    ("GopalCallEndReason"), values);
        g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
    }

    return g_define_type_id__volatile;
}

enum { PROP_SIPEP = 1, PROP_PCSSEP, PROP_LAST };

struct _GopalManagerPrivate
{
    MyManager *manager;
    GopalSIPEP *sipep;
    GopalPCSSEP *pcssep;
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
    g_object_unref (self->priv->pcssep);

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
    case PROP_PCSSEP:
        g_value_set_object (value, self->priv->pcssep);
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

    g_object_class_install_property (gobject_class, PROP_PCSSEP,
        g_param_spec_object("pcss-endpoint", "pcssep", "Opal's PCSS end-point",
                            GOPAL_TYPE_PCSS_EP,
                            GParamFlags (G_PARAM_READABLE |
                                         G_PARAM_STATIC_STRINGS)));

    /**
     * GopalManager::call-established:
     * @self: the #GopalManager instance
     * @token: (transfer none) (allow-none): the call's token
     *
     * A call back function whenever a call is completed.
     *
     * In telephony terminology a completed call is one where there is
     * an established link between two parties.
     *
     * This called from the OpalCall::OnEstablished() function.
     *
     * The default behaviour does nothing.
     */
    g_signal_new("call-established",
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
     * GopalManager::call-cleared:
     * @self: the #GopalManager instance
     * @token: (transfer none) (allow-none): the call's token
     * @reason: the end reason id
     *
     * A call back function whenever a call is cleared.
     *
     * A call is cleared whenever there is no longer any connections
     * attached to it. This function is called just before the call is
     * deleted. However, it may be used to display information on the
     * call after completion, eg the call parties and duration.
     *
     * Note that there is not a one to one relationship with the
     * OnEstablishedCall() function. This function may be called
     * without that function being called. For example if
     * MakeConnection() was used but the call never completed.
     *
     * The default behaviour removes the call from the activeCalls
     * dictionary.
     */
    g_signal_new("call-cleared",
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
                 gopal_call_end_reason_get_type ());
}

static void
gopal_manager_init (GopalManager *self)
{
    self->priv = GET_PRIVATE (self);
    self->priv->manager = new MyManager(self);

    self->priv->sipep = (GopalSIPEP *) g_object_new (GOPAL_TYPE_SIP_EP,
                                                    "manager", self->priv->manager,
                                                     NULL);

    self->priv->pcssep = (GopalPCSSEP *) g_object_new (GOPAL_TYPE_PCSS_EP,
                                                       "manager", self->priv->manager,
                                                       NULL);
}

GopalManager *
gopal_manager_new ()
{
    return GOPAL_MANAGER (g_object_new (GOPAL_TYPE_MANAGER, NULL));
}

/**
 * gopal_manager_set_stun_server:
 * @self: #GopalManager instance
 * @server: STUN server address
 *
 * Set the STUN server address, is of the form host[:port] Note that
 * if the STUN server is found then the translation Address is
 * automatically set to the router address as determined by STUN.
 *
 * Returns: the NAT type #OpalSTUNClientNatType
 */
GopalSTUNClientNatType
gopal_manager_set_stun_server (GopalManager *self, const char *server)
{
    PString STUNServer;

    STUNServer = (server) ? PString(server) : PString::Empty();
    return (GopalSTUNClientNatType) MANAGER(self)->SetSTUNServer (STUNServer);
}

/**
 * gopal_manager_get_stun_server:
 * @self: #GopalManager instance
 *
 * Returns: the current host name and optional port for the STUN
 * server.
 */
const char *
gopal_manager_get_stun_server (GopalManager *self)
{
    return MANAGER (self)->GetSTUNServer ();
}

static void
set_stun_server_thread (GTask *task,
                        gpointer source_object,
                        gpointer task_data,
                        GCancellable *cancellable)
{
    GopalManager *self = (GopalManager *) source_object;
    char *server = (char *) task_data;
    GopalSTUNClientNatType type;

    type = gopal_manager_set_stun_server (self, server);
    g_task_return_int (task, type);
}

/**
 * gopal_manager_set_stun_server_async:
 * @self: a #GopalManager instance
 * @server: STUN server address
 * @cancellable (allow-none): a #GCancellable instance
 * @callback: the function callback
 * @user_data: data
 *
 * Set asynchronously the STUN server.
 */
void
gopal_manager_set_stun_server_async (GopalManager *self,
                                     const char *server,
                                     GCancellable *cancellable,
                                     GAsyncReadyCallback callback,
                                     gpointer user_data)
{
    char *data = g_strdup (server);
    GTask *task = g_task_new (self, cancellable, callback, user_data);
    g_task_set_task_data (task, data, (GDestroyNotify) g_free);
    g_task_run_in_thread (task, set_stun_server_thread);
}

/**
 * gopal_manager_set_stun_server_finish:
 * @self: a #GopalManager instance
 * @result: a #GAsyncResult container
 * @error (allow-none): a possible #GError
 *
 * returns the NAT type recognized by the STUN server
 */
GopalSTUNClientNatType
gopal_manager_set_stun_server_finish (GopalManager *self,
                                      GAsyncResult *result,
                                      GError **error)
{
    g_return_val_if_fail (g_task_is_valid (result, self),
                          GOPAL_STUN_CLIENT_NAT_TYPE_UNKNOWN);

    return GopalSTUNClientNatType (g_task_propagate_int (G_TASK (result), error));
}

/**
 * gopal_manager_set_translation_host:
 * @self: #GopalManager instance
 * @host: address of the NAT router
 *
 * Set the translation host to use for TranslateIPAddress().
 *
 * Returns: true if the can set the translation host
 */
gboolean
gopal_manager_set_translation_host (GopalManager *self, const char *host)
{
    PString NATRouter;

    NATRouter = (host) ? PString(host) : PString::Empty();
    return MANAGER (self)->SetTranslationHost(NATRouter);
}

/**
 * gopal_manager_get_sip_endpoint:
 * @self: #GopalManager instance
 *
 * An SIP end-point is always instanced to be used.
 *
 * Returns: (transfer full) the internal SIP end-point instance
 */
GopalSIPEP *
gopal_manager_get_sip_endpoint (GopalManager *self)
{
    return GOPAL_SIP_EP (g_object_ref (self->priv->sipep));
}

/**
 * gopal_manager_get_pcss_endpoint:
 * @self: #GopalManager instance
 *
 * An PC Sound System end-point is always instanced to be used.
 *
 * Returns: (transfer full) the internal PCSS end-point instance
 */
GopalPCSSEP *
gopal_manager_get_pcss_endpoint (GopalManager *self)
{
    return GOPAL_PCSS_EP (g_object_ref (self->priv->pcssep));
}


/**
 * gopal_manager_shutdown_endpoints:
 * @self: #GopalManager instance
 *
 * Shut down all of the endpoints, clearing all calls.  This is
 * synchronous and will wait till everything is shut down.  This will
 * also assure no new calls come in whilein the process of shutting
 * down.
 */
void
gopal_manager_shutdown_endpoints (GopalManager *self)
{
    MANAGER (self)->ShutDownEndpoints ();
}

/**
 * gopal_manager_setup_call:
 * @self: #GopalManager instance
 * @party_a: (allow-none): the address of the initiator of the call
 * @party_b: the address of the remote system being called
 * @token: (out) (transfer full) (allow-none): the token of the call
 * @connection_options: connection options
 * @user_data: user data to pass to call and connection
 *
 * Set up a call between two parties.
 *
 * This is used to initiate a call. Incoming calls are "answered"
 * using a different mechanism.
 *
 * The A party and B party strings indicate the protocol and address
 * of the party to call in the style of a URL. The A party is the
 * initiator of the call and the B party is the remote system being
 * called.
 *
 * The token returned is a unique identifier for the call that allows
 * an application to gain access to the call at later time. This is
 * necesary as any pointer being returned could become invalid (due to
 * being deleted) at any time due to the multithreaded nature of the
 * OPAL system.
 */
gboolean
gopal_manager_setup_call (GopalManager *self,
                          const gchar *party_a,
                          const gchar *party_b,
                          char **token,
                          uint connection_options,
                          gpointer *user_data)
{
    PString partyA, partyB, tok;

    partyA = (party_a) ? PString(party_a) : PString::Empty(); // local
    partyB = (party_b) ? PString(party_b) : PString::Empty(); // remote

    if (partyB.IsEmpty())
        return FALSE;

    PString from = partyA;
    if (from.IsEmpty())
        from = "pc:*";

    bool ret = MANAGER (self)->SetUpCall (from,
                                          partyB,
                                          tok,
                                          user_data,
                                          connection_options,
                                          NULL);

    *token = g_strdup((const gchar *) tok);

    return ret;
}

/**
 * gopal_manager_is_call_established:
 * @self: #GopalManager instance
 * @token: the token of the call
 *
 * Determine if a call is established.
 *
 * Return true if there is an active call with the specified token and
 * that call has at least two parties with media flowing between
 * them.
 *
 * Note that the call could clear any time (even milliseconds)
 * after this function returns true.
 */
gboolean
gopal_manager_is_call_established (GopalManager *self, const char *token)
{
    PString tok = (token) ? PString (token) : PString::Empty ();
    return MANAGER (self)->IsCallEstablished (tok);
}

/**
 * gopal_manager_clear_call:
 * @self: #GopalManager instance
 * @token: the token of the call
 * @reason: reason for call clearing
 *
 * Clear a call.
 *
 * This finds the call by using the token then calls the
 * OpalCall::Clear() function on it. All connections are released, and
 * the connections and call are disposed of. Note that this function
 * returns quickly and the disposal happens at some later time in a
 * background thread. It is safe to call this function from anywhere.
 */
gboolean
gopal_manager_clear_call (GopalManager *self,
                          const char *token,
                          GopalCallEndReason reason)
{
    PString tok = (token) ? PString (token) : PString::Empty ();
    return MANAGER (self)->ClearCall (tok, OpalConnection::CallEndReason(reason));
}

const struct {
    GopalCallEndReason reason;
    const char *msg;
} reason_desc[] = {
    { GOPAL_CALL_END_REASON_LOCALUSER,
      "Local party cleared call" },
    { GOPAL_CALL_END_REASON_NOACCEPT,
      "Local party did not accept call" },
    { GOPAL_CALL_END_REASON_ANSWERDENIED,
      "Local party declined to answer call" },
    { GOPAL_CALL_END_REASON_REMOTEUSER,
      "Remote party cleared call" },
    { GOPAL_CALL_END_REASON_REFUSAL,
      "Remote party refused call" },
    { GOPAL_CALL_END_REASON_NOANSWER,
      "Remote party did not answer in required time" },
    { GOPAL_CALL_END_REASON_CALLERABORT,
      "Remote party stopped calling" },
    { GOPAL_CALL_END_REASON_TRANSPORTFAIL,
      "Call failed due to a transport error" },
    { GOPAL_CALL_END_REASON_CONNECTFAIL,
      "Connection to remote failed" },
    { GOPAL_CALL_END_REASON_GATEKEEPER,
      "Gatekeeper has cleared call" },
    { GOPAL_CALL_END_REASON_NOUSER,
      "Call failed as could not find user" },
    { GOPAL_CALL_END_REASON_NOBANDWIDTH,
      "Call failed due to insufficient bandwidth" },
    { GOPAL_CALL_END_REASON_CAPABILITYEXCHANGE,
      "Call failed as could not find common media capabilities" },
    { GOPAL_CALL_END_REASON_CALLFORWARDED,
      "Call was forwarded" },
    { GOPAL_CALL_END_REASON_SECURITYDENIAL,
      "Call failed security check" },
    { GOPAL_CALL_END_REASON_LOCALBUSY,
      "Local party busy" },
    { GOPAL_CALL_END_REASON_LOCALCONGESTION,
      "Local party congested" },
    { GOPAL_CALL_END_REASON_REMOTEBUSY,
      "Remote party busy" },
    { GOPAL_CALL_END_REASON_REMOTECONGESTION,
      "Remote switch congested" },
    { GOPAL_CALL_END_REASON_UNREACHABLE,
      "Remote party could not be reached" },
    { GOPAL_CALL_END_REASON_NOENDPOINT,
      "Remote party application is not running" },
    { GOPAL_CALL_END_REASON_HOSTOFFLINE,
      "Remote party host is off line" },
    { GOPAL_CALL_END_REASON_TEMPORARYFAILURE,
      "Remote system failed temporarily" },
    { GOPAL_CALL_END_REASON_Q931CAUSE,
      "Call cleared with Q.931 cause code" },
    { GOPAL_CALL_END_REASON_DURATIONLIMIT,
      "Call cleared due to an enforced duration limit" },
    { GOPAL_CALL_END_REASON_INVALIDCONFERENCEID,
      "Call cleared due to invalid conference ID" },
    { GOPAL_CALL_END_REASON_NODIALTONE,
      "Call cleared due to missing dial tone" },
    { GOPAL_CALL_END_REASON_NORINGBACKTONE,
      "Call cleared due to missing ringback tone" },
    { GOPAL_CALL_END_REASON_OUTOFSERVICE,
      "Call cleared because the line is out of service" },
    { GOPAL_CALL_END_REASON_ACCEPTINGCALLWAITING,
      "Call cleared because another call is answered" },
    { GOPAL_CALL_END_REASON_GKADMISSIONFAILED,
      "Call cleared because gatekeeper admission request failed." },
};

/**
 * gopal_manager_get_reason_string:
 * reason: call end reason code
 *
 * returns: a string with the description of the call end reason
 */
const char *
gopal_manager_get_reason_string (GopalCallEndReason reason)
{
    for (unsigned int i = 0; i < sizeof (reason_desc) / sizeof (reason_desc[0]); i++)
        if (reason_desc[i].reason == reason)
            return reason_desc[i].msg;

    return NULL;
}

G_END_DECLS
