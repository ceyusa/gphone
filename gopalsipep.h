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

enum _GopalStatusCodes {
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
};

typedef enum _GopalStatusCodes GopalStatusCodes;
typedef struct _GopalSIPRegisterParams GopalSIPRegisterParams;

struct _GopalSIPRegisterParams {
    const gchar *domain;
    const gchar *user;
    const gchar *auth_name;
    const gchar *password;
    const gchar *proxy;
    guint ttl;
};

GType
gopal_sip_ep_get_type                          (void) G_GNUC_CONST;

gboolean
gopal_sip_ep_register                          (GopalSIPEP *self,
						GopalSIPRegisterParams *params,
						const gchar **address_of_record);

gboolean
gopal_sip_ep_start_listeners                   (GopalSIPEP *self,
                                                gchar **listeners);

G_END_DECLS

#endif /* GGOPAL_SIP_EP_H */
