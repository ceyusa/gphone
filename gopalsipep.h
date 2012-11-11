/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

#ifndef GOPAL_SIP_EP_H
#define GOPAL_SIP_EP_H

#include <glib-object.h>

G_BEGIN_DECLS

#define GOPAL_TYPE_SIP_EP			\
    (gopal_sip_ep_get_type())
#define GOPAL_SIP_EP(obj)			\
    (G_TYPE_CHECK_INSTANCE_CAST((obj), GOPAL_TYPE_SIP_EP, GopalSIPEP))
#define GOPAL_SIP_EP_CLASS(klass)		\
    (G_TYPE_CHECK_CLASS_CAST((klass),  GOPAL_TYPE_SIP_EP, GopalSIPEPClass))
#define GOPAL_IS_SIP_EP(obj)		\
    (G_TYPE_CHECK_INSTANCE_TYPE((obj), GOPAL_TYPE_SIP_EP))
#define GOPAL_IS_SIP_EP_CLASS(klass)	\
    (G_TYPE_CHECK_CLASS_TYPE((klass),  GOPAL_TYPE_SIP_EP))
#define GOPAL_SIP_EP_GET_CLASS(obj)					\
    (G_TYPE_INSTANCE_GET_CLASS((obj),  GOPAL_TYPE_SIP_EP, GopalSIPEPClass))

typedef struct _GopalSIPEPPrivate GopalSIPEPPrivate;
typedef struct _GopalSIPEP GopalSIPEP;
typedef struct _GopalSIPEPClass GopalSIPEPClass;

struct _GopalSIPEP {
    GObject parent;

    /*< private >*/
    GopalSIPEPPrivate *priv;
};

struct _GopalSIPEPClass {
    GObjectClass parent_class;
};

/**
 * GopalStatusCodes:
 * @GOPAL_STATUS_CODE_ILLEGALSTATUSCODE: illegal
 * @GOPAL_STATUS_CODE_LOCAL_TRANSPORTERROR: transport error
 * @GOPAL_STATUS_CODE_LOCAL_BADTRANSPORTADDRESS: bad transport address
 * @GOPAL_STATUS_CODE_LOCAL_TIMEOUT: timeout
 * @GOPAL_STATUS_CODE_INFORMATION_TRYING: trying
 * @GOPAL_STATUS_CODE_INFORMATION_RINGING: ringing
 * @GOPAL_STATUS_CODE_INFORMATION_CALLFORWARDED: call forwarded
 * @GOPAL_STATUS_CODE_INFORMATION_QUEUED: queued
 * @GOPAL_STATUS_CODE_INFORMATION_SESSION_PROGRESS: session progress
 * @GOPAL_STATUS_CODE_SUCCESS_OK: OK
 * @GOPAL_STATUS_CODE_SUCCESS_ACCEPTED: accepted
 * @GOPAL_STATUS_CODE_REDIR_MULTIPLECHOICES: multiplechoices
 * @GOPAL_STATUS_CODE_REDIR_MOVEDPERMANENTLY: moved permanently
 * @GOPAL_STATUS_CODE_REDIR_MOVEDTEMPORARILY: moved temporarily
 * @GOPAL_STATUS_CODE_REDIR_USEPROXY: use proxy
 * @GOPAL_STATUS_CODE_REDIR_ALTERNATIVESERVICE: alternative service
 * @GOPAL_STATUS_CODE_FAIL_BADREQUEST: bad request
 * @GOPAL_STATUS_CODE_FAIL_UNAUTHORISED: unauthorised
 * @GOPAL_STATUS_CODE_FAIL_PAYMENTREQUIRED: payment required
 * @GOPAL_STATUS_CODE_FAIL_FORBIDDEN: forbidden
 * @GOPAL_STATUS_CODE_FAIL_NOTFOUND: not found
 * @GOPAL_STATUS_CODE_FAIL_METHODNOTALLOWED: method not allowed
 * @GOPAL_STATUS_CODE_FAIL_NOTACCEPTABLE: not acceptable
 * @GOPAL_STATUS_CODE_FAIL_PROXYAUTHENTICATIONREQUIRED: proxy authentication required
 * @GOPAL_STATUS_CODE_FAIL_REQUESTTIMEOUT: request timeout
 * @GOPAL_STATUS_CODE_FAIL_CONFLICT: conflict
 * @GOPAL_STATUS_CODE_FAIL_GONE: gone
 * @GOPAL_STATUS_CODE_FAIL_LENGTHREQUIRED: length required
 * @GOPAL_STATUS_CODE_FAIL_REQUESTENTITYTOOLARGE: request entity too large
 * @GOPAL_STATUS_CODE_FAIL_REQUESTURITOOLONG: request URI too long
 * @GOPAL_STATUS_CODE_FAIL_UNSUPPORTEDMEDIATYPE: unsupported media type
 * @GOPAL_STATUS_CODE_FAIL_UNSUPPORTEDURISCHEME: unsupported URI scheme
 * @GOPAL_STATUS_CODE_FAIL_BADEXTENSION: bad extension
 * @GOPAL_STATUS_CODE_FAIL_EXTENSIONREQUIRED: extension required
 * @GOPAL_STATUS_CODE_FAIL_INTERVALTOOBRIEF: interval too brief
 * @GOPAL_STATUS_CODE_FAIL_TEMPORARILYUNAVAILABLE: temporarily unavailable
 * @GOPAL_STATUS_CODE_FAIL_TRANSACTIONDOESNOTEXIST: transaction does not exist
 * @GOPAL_STATUS_CODE_FAIL_LOOPDETECTED: loop detected
 * @GOPAL_STATUS_CODE_FAIL_TOOMANYHOPS: too many hops
 * @GOPAL_STATUS_CODE_FAIL_ADDRESSINCOMPLETE: address incomplete
 * @GOPAL_STATUS_CODE_FAIL_AMBIGUOUS: ambiguous
 * @GOPAL_STATUS_CODE_FAIL_BUSYHERE: busy here
 * @GOPAL_STATUS_CODE_FAIL_REQUESTTERMINATED: request terminated
 * @GOPAL_STATUS_CODE_FAIL_NOTACCEPTABLEHERE: not acceptable here
 * @GOPAL_STATUS_CODE_FAIL_BADEVENT: bad event
 * @GOPAL_STATUS_CODE_FAIL_REQUESTPENDING: request pending
 * @GOPAL_STATUS_CODE_FAIL_UNDECIPHERABLE: undecipherable
 * @GOPAL_STATUS_CODE_FAIL_INTERNALSERVERERROR: internal server error
 * @GOPAL_STATUS_CODE_FAIL_NOTIMPLEMENTED: not implemented
 * @GOPAL_STATUS_CODE_FAIL_BADGATEWAY: bad gateway
 * @GOPAL_STATUS_CODE_FAIL_SERVICEUNAVAILABLE: service unavailable
 * @GOPAL_STATUS_CODE_FAIL_SERVERTIMEOUT: server timeout
 * @GOPAL_STATUS_CODE_FAIL_SIPVERSIONNOTSUPPORTED: SIP version not supported
 * @GOPAL_STATUS_CODE_FAIL_MESSAGETOOLARGE: message too large
 * @GOPAL_STATUS_CODE_GLOBALFAIL_BUSYEVERYWHERE: busy everywhere
 * @GOPAL_STATUS_CODE_GLOBALFAIL_DECLINE: decline
 * @GOPAL_STATUS_CODE_GLOBALFAIL_DOESNOTEXISTANYWHERE: does not exist anywhere
 * @GOPAL_STATUS_CODE_GLOBALFAIL_NOTACCEPTABLE: not accepable
 * @GOPAL_STATUS_CODE_MAX: max code
 *
 * Status code of the Session Initiation Protocol (SIP) message
 */
typedef enum {
    GOPAL_STATUS_CODE_ILLEGALSTATUSCODE,
    GOPAL_STATUS_CODE_LOCAL_TRANSPORTERROR,
    GOPAL_STATUS_CODE_LOCAL_BADTRANSPORTADDRESS,
    GOPAL_STATUS_CODE_LOCAL_TIMEOUT,

    GOPAL_STATUS_CODE_INFORMATION_TRYING               = 100,
    GOPAL_STATUS_CODE_INFORMATION_RINGING              = 180,
    GOPAL_STATUS_CODE_INFORMATION_CALLFORWARDED        = 181,
    GOPAL_STATUS_CODE_INFORMATION_QUEUED               = 182,
    GOPAL_STATUS_CODE_INFORMATION_SESSION_PROGRESS     = 183,

    GOPAL_STATUS_CODE_SUCCESS_OK                       = 200,
    GOPAL_STATUS_CODE_SUCCESS_ACCEPTED		       = 202,

    GOPAL_STATUS_CODE_REDIR_MULTIPLECHOICES            = 300,
    GOPAL_STATUS_CODE_REDIR_MOVEDPERMANENTLY           = 301,
    GOPAL_STATUS_CODE_REDIR_MOVEDTEMPORARILY           = 302,
    GOPAL_STATUS_CODE_REDIR_USEPROXY                   = 305,
    GOPAL_STATUS_CODE_REDIR_ALTERNATIVESERVICE         = 380,

    GOPAL_STATUS_CODE_FAIL_BADREQUEST                  = 400,
    GOPAL_STATUS_CODE_FAIL_UNAUTHORISED                = 401,
    GOPAL_STATUS_CODE_FAIL_PAYMENTREQUIRED             = 402,
    GOPAL_STATUS_CODE_FAIL_FORBIDDEN                   = 403,
    GOPAL_STATUS_CODE_FAIL_NOTFOUND                    = 404,
    GOPAL_STATUS_CODE_FAIL_METHODNOTALLOWED            = 405,
    GOPAL_STATUS_CODE_FAIL_NOTACCEPTABLE               = 406,
    GOPAL_STATUS_CODE_FAIL_PROXYAUTHENTICATIONREQUIRED = 407,
    GOPAL_STATUS_CODE_FAIL_REQUESTTIMEOUT              = 408,
    GOPAL_STATUS_CODE_FAIL_CONFLICT                    = 409,
    GOPAL_STATUS_CODE_FAIL_GONE                        = 410,
    GOPAL_STATUS_CODE_FAIL_LENGTHREQUIRED              = 411,
    GOPAL_STATUS_CODE_FAIL_REQUESTENTITYTOOLARGE       = 413,
    GOPAL_STATUS_CODE_FAIL_REQUESTURITOOLONG           = 414,
    GOPAL_STATUS_CODE_FAIL_UNSUPPORTEDMEDIATYPE        = 415,
    GOPAL_STATUS_CODE_FAIL_UNSUPPORTEDURISCHEME        = 416,
    GOPAL_STATUS_CODE_FAIL_BADEXTENSION                = 420,
    GOPAL_STATUS_CODE_FAIL_EXTENSIONREQUIRED           = 421,
    GOPAL_STATUS_CODE_FAIL_INTERVALTOOBRIEF            = 423,
    GOPAL_STATUS_CODE_FAIL_TEMPORARILYUNAVAILABLE      = 480,
    GOPAL_STATUS_CODE_FAIL_TRANSACTIONDOESNOTEXIST     = 481,
    GOPAL_STATUS_CODE_FAIL_LOOPDETECTED                = 482,
    GOPAL_STATUS_CODE_FAIL_TOOMANYHOPS                 = 483,
    GOPAL_STATUS_CODE_FAIL_ADDRESSINCOMPLETE           = 484,
    GOPAL_STATUS_CODE_FAIL_AMBIGUOUS                   = 485,
    GOPAL_STATUS_CODE_FAIL_BUSYHERE                    = 486,
    GOPAL_STATUS_CODE_FAIL_REQUESTTERMINATED           = 487,
    GOPAL_STATUS_CODE_FAIL_NOTACCEPTABLEHERE           = 488,
    GOPAL_STATUS_CODE_FAIL_BADEVENT                    = 489,
    GOPAL_STATUS_CODE_FAIL_REQUESTPENDING              = 491,
    GOPAL_STATUS_CODE_FAIL_UNDECIPHERABLE              = 493,

    GOPAL_STATUS_CODE_FAIL_INTERNALSERVERERROR         = 500,
    GOPAL_STATUS_CODE_FAIL_NOTIMPLEMENTED              = 501,
    GOPAL_STATUS_CODE_FAIL_BADGATEWAY                  = 502,
    GOPAL_STATUS_CODE_FAIL_SERVICEUNAVAILABLE          = 503,
    GOPAL_STATUS_CODE_FAIL_SERVERTIMEOUT               = 504,
    GOPAL_STATUS_CODE_FAIL_SIPVERSIONNOTSUPPORTED      = 505,
    GOPAL_STATUS_CODE_FAIL_MESSAGETOOLARGE             = 513,

    GOPAL_STATUS_CODE_GLOBALFAIL_BUSYEVERYWHERE        = 600,
    GOPAL_STATUS_CODE_GLOBALFAIL_DECLINE               = 603,
    GOPAL_STATUS_CODE_GLOBALFAIL_DOESNOTEXISTANYWHERE  = 604,
    GOPAL_STATUS_CODE_GLOBALFAIL_NOTACCEPTABLE         = 606,

    GOPAL_STATUS_CODE_MAX                              = 699
} GopalStatusCodes;

typedef struct _GopalSIPRegistrationStatus GopalSIPRegistrationStatus;
typedef struct _GopalSIPParams GopalSIPParams;
typedef struct _GopalSIPRegisterParams GopalSIPRegisterParams;

struct _GopalSIPParams {
    gchar *remote_address;
    gchar *local_address;
    gchar *proxy_address;
    gchar *address_of_record;
    gchar *contact_address;
    /* @TODO: SIP MIME info container */
    gchar *auth_ID;
    gchar *password;
    gchar *realm;
    guint expire;
    guint restore;
    GTimeVal min_retry;
    GTimeVal max_retry;
    gpointer user_data;
};

/**
 * GopalSIPRegisterCompatibilityModes:
 * @GOPAL_SIP_REGISTER_COMPAT_MODE_FULLY_COMPLIANT: Registrar is fully
 * compliant, we will register all listeners of all types (e.g. sip,
 * sips etc) in the Contact field.
 * @GOPAL_SIP_REGISTER_COMPAT_MODE_CANNOT_REGISTER_MULTIPLE_CONTACTS:
 * Registrar refuses to register more than one contact field. Correct
 * behaviour is to return a contact with the fields it can accept in
 * the 200 OK
 * @GOPAL_SIP_REGISTER_COMPAT_MODE_CANNOT_REGISTER_PRIVATE_CONTACTS:
 * Registrar refuses to register any RFC contact field. Correct
 * behaviour is to return a contact with the fields it can accept in
 * the 200 OK
 * @GOPAL_SIP_REGISTER_COMPAT_MODE_HAS_APPLICATION_LAYER_GATEWAY:
 * Router has Application Layer Gateway code that is doing address
 * transations, so we do not try to do it ourselves as well or it goes
 * horribly wrong.
 *
 * Registrar compatibility mode.
 *
 */
typedef enum {
    GOPAL_SIP_REGISTER_COMPAT_MODE_FULLY_COMPLIANT,
    GOPAL_SIP_REGISTER_COMPAT_MODE_CANNOT_REGISTER_MULTIPLE_CONTACTS,
    GOPAL_SIP_REGISTER_COMPAT_MODE_CANNOT_REGISTER_PRIVATE_CONTACTS,
    GOPAL_SIP_REGISTER_COMPAT_MODE_HAS_APPLICATION_LAYER_GATEWAY
} GopalSIPRegisterCompatibilityModes;


struct _GopalSIPRegisterParams {
    GopalSIPParams params;
    gchar *registrar_address;
    GopalSIPRegisterCompatibilityModes compatibility;
};

struct _GopalSIPRegistrationStatus {
    gchar *address_of_record;
    gboolean was_registering;
    gboolean re_registering;
    GopalStatusCodes reason;
};

GType
gopal_sip_ep_get_type                          (void) G_GNUC_CONST;

GType
gopal_status_codes_get_type                    (void) G_GNUC_CONST;

gboolean
gopal_sip_ep_register                          (GopalSIPEP *self,
						GopalSIPRegisterParams *params,
						const gchar **address_of_record,
						GopalStatusCodes *reason);

gboolean
gopal_sip_ep_unregister                        (GopalSIPEP *self,
                                                const gchar *aor);

gboolean
gopal_sip_ep_start_listeners                   (GopalSIPEP *self,
                                                gchar **interfaces);

G_END_DECLS

#endif /* GGOPAL_SIP_EP_H */
