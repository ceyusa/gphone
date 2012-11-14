/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

#ifndef GOPAL_PCSS_EP_H
#define GOPAL_PCSS_EP_H

#include <glib-object.h>

G_BEGIN_DECLS

#define GOPAL_TYPE_PCSS_EP		\
    (gopal_pcss_ep_get_type())
#define GOPAL_PCSS_EP(obj)		\
    (G_TYPE_CHECK_INSTANCE_CAST((obj), GOPAL_TYPE_PCSS_EP, GopalPCSSEP))
#define GOPAL_PCSS_EP_CLASS(klass)	\
    (G_TYPE_CHECK_CLASS_CAST((klass),  GOPAL_TYPE_PCSS_EP, GopalPCSSEPClass))
#define GOPAL_IS_PCSS_EP(obj)		\
    (G_TYPE_CHECK_INSTANCE_TYPE((obj), GOPAL_TYPE_PCSS_EP))
#define GOPAL_IS_PCSS_EP_CLASS(klass)	\
    (G_TYPE_CHECK_CLASS_TYPE((klass),  GOPAL_TYPE_PCSS_EP))
#define GOPAL_PCSS_EP_GET_CLASS(obj)	\
    (G_TYPE_INSTANCE_GET_CLASS((obj),  GOPAL_TYPE_PCSS_EP, GopalPCSSEPClass))

typedef struct _GopalPCSSEPPrivate GopalPCSSEPPrivate;
typedef struct _GopalPCSSEP GopalPCSSEP;
typedef struct _GopalPCSSEPClass GopalPCSSEPClass;

struct _GopalPCSSEP {
    GObject parent;

    /*< private >*/
    GopalPCSSEPPrivate *priv;
};

struct _GopalPCSSEPClass {
    GObjectClass parent_class;
};

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

GType
gopal_pcss_ep_get_type                         (void) G_GNUC_CONST;

gboolean
gopal_pcss_ep_set_soundchannel_play_device     (GopalPCSSEP *self,
						const gchar *name);

const gchar *
gopal_pcss_ep_get_soundchannel_play_device     (GopalPCSSEP *self);

gboolean
gopal_pcss_ep_set_soundchannel_record_device   (GopalPCSSEP *self,
						const gchar *name);

const gchar *
gopal_pcss_ep_get_soundchannel_record_device   (GopalPCSSEP *self);

void
gopal_pcss_ep_set_soundchannel_buffer_time     (GopalPCSSEP *self,
						guint depth);

guint
gopal_pcss_ep_get_soundchannel_buffer_time     (GopalPCSSEP *self);

gboolean
gopal_pcss_ep_accept_incoming_connection       (GopalPCSSEP *self,
                                                const gchar *token);

gboolean
gopal_pcss_ep_reject_incoming_connection       (GopalPCSSEP *self,
                                                const gchar *token,
                                                GopalCallEndReason reason);


G_END_DECLS

#endif /* GOPAL_PCSS_EP_H */
