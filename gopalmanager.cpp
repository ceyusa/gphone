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
    bool MakeCall(const PString & local, const PString & remote);
    void Close();

private:
    GopalManager *m_manager;
    PSafePtr<OpalCall> m_activeCall;
};

void
MyManager::Close()
{
    m_activeCall.SetNULL();
    ShutDownEndpoints();
}

bool
MyManager::MakeCall(const PString & local,
                    const PString & remote)
{
    if (remote.IsEmpty())
        return false;

    PString from = local;
    if (from.IsEmpty())
        from = "pc:*";

    m_activeCall = SetUpCall(from, remote, NULL, 0, NULL);

    return m_activeCall != NULL;
}

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
    MANAGER (self)->Close ();
}

/**
 * gopal_manager_make_call:
 * @self: #GopalManager instance
 * @party_a: (allow-none): the address of the initiator of the call
 * @party_b: the address of the remote system being called
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
gopal_manager_make_call(GopalManager *self,
                        const gchar *party_a,
                        const gchar *party_b)
{
    PString partyA, partyB;

    partyA = (party_a) ? PString(party_a) : PString::Empty();
    partyB = (party_b) ? PString(party_b) : PString::Empty();

    return MANAGER (self)->MakeCall (partyA, partyB);
}


G_END_DECLS
