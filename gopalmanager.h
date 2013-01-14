/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

#ifndef GOPAL_MANAGER_H
#define GOPAL_MANAGER_H

#include <gio/gio.h>

G_BEGIN_DECLS

#define GOPAL_TYPE_MANAGER			\
	(gopal_manager_get_type())
#define GOPAL_MANAGER(obj)			\
	(G_TYPE_CHECK_INSTANCE_CAST((obj), GOPAL_TYPE_MANAGER, GopalManager))
#define GOPAL_MANAGER_CLASS(klass)		\
	(G_TYPE_CHECK_CLASS_CAST((klass),  GOPAL_TYPE_MANAGER, GopalManagerClass))
#define GOPAL_IS_MANAGER(obj)			\
	(G_TYPE_CHECK_INSTANCE_TYPE((obj), GOPAL_TYPE_MANAGER))
#define GOPAL_IS_MANAGER_CLASS(klass)		\
	(G_TYPE_CHECK_CLASS_TYPE((klass),  GOPAL_TYPE_MANAGER))
#define GOPAL_MANAGER_GET_CLASS(obj)		\
	(G_TYPE_INSTANCE_GET_CLASS((obj),  GOPAL_TYPE_MANAGER, GopalManagerClass))

typedef struct _GopalSIPEP GopalSIPEP;
typedef struct _GopalManagerPrivate GopalManagerPrivate;
typedef struct _GopalManager GopalManager;
typedef struct _GopalManagerClass GopalManagerClass;

typedef enum {
    GOPAL_STUN_CLIENT_NAT_TYPE_UNKNOWN,
    GOPAL_STUN_CLIENT_NAT_TYPE_OPEN,
    GOPAL_STUN_CLIENT_NAT_TYPE_CONE,
    GOPAL_STUN_CLIENT_NAT_TYPE_RESTRICTED,
    GOPAL_STUN_CLIENT_NAT_TYPE_PORT_RESTRICTED,
    GOPAL_STUN_CLIENT_NAT_TYPE_SYMMETRIC,
    GOPAL_STUN_CLIENT_NAT_TYPE_SYMMETRIC_FIREWALL,
    GOPAL_STUN_CLIENT_NAT_TYPE_BLOCKED,
    GOPAL_STUN_CLIENT_NAT_TYPE_PARTIAL_BLOCKED,
    GOPAL_STUN_CLIENT_NAT_TYPE_END
} GopalSTUNClientNatType;

/**
 * GopalCallEndReason:
 * @GOPAL_CALL_END_REASON_LOCALUSER: Local endpoint application cleared call
 * @GOPAL_CALL_END_REASON_NOACCEPT: Local endpoint did not accept call OnIncomingCall() = false
 * @GOPAL_CALL_END_REASON_ANSWERDENIED: Local endpoint declined to answer call
 * @GOPAL_CALL_END_REASON_REMOTEUSER: Remote endpoint application cleared call
 * @GOPAL_CALL_END_REASON_REFUSAL: Remote endpoint refused call
 * @GOPAL_CALL_END_REASON_NOANSWER: Remote endpoint did not answer in required time
 * @GOPAL_CALL_END_REASON_CALLERABORT: Remote endpoint stopped calling
 * @GOPAL_CALL_END_REASON_TRANSPORTFAIL: Transport error cleared call
 * @GOPAL_CALL_END_REASON_CONNECTFAIL: Transport connection failed to establish call
 * @GOPAL_CALL_END_REASON_GATEKEEPER: Gatekeeper has cleared call
 * @GOPAL_CALL_END_REASON_NOUSER: Call failed as could not find user (in GK)
 * @GOPAL_CALL_END_REASON_NOBANDWIDTH: Call failed as could not get enough bandwidth
 * @GOPAL_CALL_END_REASON_CAPABILITYEXCHANGE: Could not find common capabilities
 * @GOPAL_CALL_END_REASON_CALLFORWARDED: Call was forwarded using FACILITY message
 * @GOPAL_CALL_END_REASON_SECURITYDENIAL: Call failed a security check and was ended
 * @GOPAL_CALL_END_REASON_LOCALBUSY: Local endpoint busy
 * @GOPAL_CALL_END_REASON_LOCALCONGESTION: Local endpoint congested
 * @GOPAL_CALL_END_REASON_REMOTEBUSY: Remote endpoint busy
 * @GOPAL_CALL_END_REASON_REMOTECONGESTION: Remote endpoint congested
 * @GOPAL_CALL_END_REASON_UNREACHABLE: Could not reach the remote party
 * @GOPAL_CALL_END_REASON_NOENDPOINT: The remote party is not running an endpoint
 * @GOPAL_CALL_END_REASON_HOSTOFFLINE: The remote party host off line
 * @GOPAL_CALL_END_REASON_TEMPORARYFAILURE: The remote failed temporarily app may retry
 * @GOPAL_CALL_END_REASON_Q931CAUSE: The remote ended the call with unmapped Q.931 cause code
 * @GOPAL_CALL_END_REASON_DURATIONLIMIT: Call cleared due to an enforced duration limit
 * @GOPAL_CALL_END_REASON_INVALIDCONFERENCEID: Call cleared due to invalid conference ID
 * @GOPAL_CALL_END_REASON_NODIALTONE: Call cleared due to missing dial tone
 * @GOPAL_CALL_END_REASON_NORINGBACKTONE: Call cleared due to missing ringback tone
 * @GOPAL_CALL_END_REASON_OUTOFSERVICE: Call cleared because the line is out of service,
 * @GOPAL_CALL_END_REASON_ACCEPTINGCALLWAITING: Call cleared because another call is answered
 * @GOPAL_CALL_END_REASON_GKADMISSIONFAILED: Call cleared because gatekeeper admission request failed.
 */
typedef enum {
    GOPAL_CALL_END_REASON_LOCALUSER,
    GOPAL_CALL_END_REASON_NOACCEPT,
    GOPAL_CALL_END_REASON_ANSWERDENIED,
    GOPAL_CALL_END_REASON_REMOTEUSER,
    GOPAL_CALL_END_REASON_REFUSAL,
    GOPAL_CALL_END_REASON_NOANSWER,
    GOPAL_CALL_END_REASON_CALLERABORT,
    GOPAL_CALL_END_REASON_TRANSPORTFAIL,
    GOPAL_CALL_END_REASON_CONNECTFAIL,
    GOPAL_CALL_END_REASON_GATEKEEPER,
    GOPAL_CALL_END_REASON_NOUSER,
    GOPAL_CALL_END_REASON_NOBANDWIDTH,
    GOPAL_CALL_END_REASON_CAPABILITYEXCHANGE,
    GOPAL_CALL_END_REASON_CALLFORWARDED,
    GOPAL_CALL_END_REASON_SECURITYDENIAL,
    GOPAL_CALL_END_REASON_LOCALBUSY,
    GOPAL_CALL_END_REASON_LOCALCONGESTION,
    GOPAL_CALL_END_REASON_REMOTEBUSY,
    GOPAL_CALL_END_REASON_REMOTECONGESTION,
    GOPAL_CALL_END_REASON_UNREACHABLE,
    GOPAL_CALL_END_REASON_NOENDPOINT,
    GOPAL_CALL_END_REASON_HOSTOFFLINE,
    GOPAL_CALL_END_REASON_TEMPORARYFAILURE,
    GOPAL_CALL_END_REASON_Q931CAUSE,
    GOPAL_CALL_END_REASON_DURATIONLIMIT,
    GOPAL_CALL_END_REASON_INVALIDCONFERENCEID,
    GOPAL_CALL_END_REASON_NODIALTONE,
    GOPAL_CALL_END_REASON_NORINGBACKTONE,
    GOPAL_CALL_END_REASON_OUTOFSERVICE,
    GOPAL_CALL_END_REASON_ACCEPTINGCALLWAITING,
    GOPAL_CALL_END_REASON_GKADMISSIONFAILED,
    GOPAL_CALL_END_REASON_MAX
} GopalCallEndReason;

struct _GopalManager {
    GObject parent;

    /*< private >*/
    GopalManagerPrivate *priv;
};

struct _GopalManagerClass {
    GObjectClass parent_class;
};

GType
gopal_manager_get_type                          (void) G_GNUC_CONST;

GType
gopal_call_end_reason_get_type                  (void) G_GNUC_CONST;

GopalManager *
gopal_manager_new                               (void);

GopalSTUNClientNatType
gopal_manager_set_stun_server                   (GopalManager *self,
						 const char *server);

void
gopal_manager_set_stun_server_async             (GopalManager *self,
						 const char *server,
                                                 GCancellable *cancellable,
                                                 GAsyncReadyCallback callback,
                                                 gpointer user_data);

GopalSTUNClientNatType
gopal_manager_set_stun_server_finish            (GopalManager *self,
                                                 GAsyncResult *result,
                                                 GError **error);

const char *
gopal_manager_get_stun_server                   (GopalManager *self);

gboolean
gopal_manager_set_translation_host              (GopalManager *self,
						 const char *host);

GopalSIPEP *
gopal_manager_get_sip_endpoint                  (GopalManager *self);

void
gopal_manager_shutdown_endpoints                (GopalManager *self);

gboolean
gopal_manager_setup_call                        (GopalManager *self,
						 const gchar *party_a,
						 const gchar *party_b,
						 char **token,
						 guint connection_options,
						 gpointer *user_data);

gboolean
gopal_manager_is_call_established              (GopalManager *self,
						const char *token);

gboolean
gopal_manager_clear_call                       (GopalManager *self,
						const char *token,
						GopalCallEndReason reason);

const char *
gopal_manager_get_end_reason_string            (GopalCallEndReason reason);

void
gopal_manager_get_product_info                 (GopalManager *self,
                                                gchar **name,
                                                gchar **vendor,
                                                gchar **version);

void
gopal_manager_set_product_info                 (GopalManager *self,
                                                const gchar *name,
                                                const gchar *vendor,
                                                const gchar *version);

gboolean
gopal_manager_add_route_entry                  (GopalManager *self,
                                                const char *spec);

G_END_DECLS

#endif /* GGOPAL_MANAGER_H */
