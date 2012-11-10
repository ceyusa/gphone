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
 *
 *  @GOPAL_STATUS_CODE_ILLEGALSTATUSCODE
 *  @GOPAL_STATUS_CODE_LOCAL_TRANSPORTERROR
 *  @GOPAL_STATUS_CODE_LOCAL_BADTRANSPORTADDRESS
 *  @GOPAL_STATUS_CODE_LOCAL_TIMEOUT
 *  @GOPAL_STATUS_CODE_INFORMATION_TRYING
 *  @GOPAL_STATUS_CODE_INFORMATION_RINGING
 *  @GOPAL_STATUS_CODE_INFORMATION_CALLFORWARDED
 *  @GOPAL_STATUS_CODE_INFORMATION_QUEUED
 *  @GOPAL_STATUS_CODE_INFORMATION_SESSION_PROGRESS
 *  @GOPAL_STATUS_CODE_SUCCESS_OK
 *  @GOPAL_STATUS_CODE_SUCCESS_ACCEPTED
 *  @GOPAL_STATUS_CODE_REDIR_MULTIPLECHOICES
 *  @GOPAL_STATUS_CODE_REDIR_MOVEDPERMANENTLY
 *  @GOPAL_STATUS_CODE_REDIR_MOVEDTEMPORARILY
 *  @GOPAL_STATUS_CODE_REDIR_USEPROXY
 *  @GOPAL_STATUS_CODE_REDIR_ALTERNATIVESERVICE
 *  @GOPAL_STATUS_CODE_FAIL_BADREQUEST
 *  @GOPAL_STATUS_CODE_FAIL_UNAUTHORISED
 *  @GOPAL_STATUS_CODE_FAIL_PAYMENTREQUIRED
 *  @GOPAL_STATUS_CODE_FAIL_FORBIDDEN
 *  @GOPAL_STATUS_CODE_FAIL_NOTFOUND
 *  @GOPAL_STATUS_CODE_FAIL_METHODNOTALLOWED
 *  @GOPAL_STATUS_CODE_FAIL_NOTACCEPTABLE
 *  @GOPAL_STATUS_CODE_FAIL_PROXYAUTHENTICATIONREQUIRED
 *  @GOPAL_STATUS_CODE_FAIL_REQUESTTIMEOUT
 *  @GOPAL_STATUS_CODE_FAIL_CONFLICT
 *  @GOPAL_STATUS_CODE_FAIL_GONE
 *  @GOPAL_STATUS_CODE_FAIL_LENGTHREQUIRED
 *  @GOPAL_STATUS_CODE_FAIL_REQUESTENTITYTOOLARGE
 *  @GOPAL_STATUS_CODE_FAIL_REQUESTURITOOLONG
 *  @GOPAL_STATUS_CODE_FAIL_UNSUPPORTEDMEDIATYPE
 *  @GOPAL_STATUS_CODE_FAIL_UNSUPPORTEDURISCHEME
 *  @GOPAL_STATUS_CODE_FAIL_BADEXTENSION
 *  @GOPAL_STATUS_CODE_FAIL_EXTENSIONREQUIRED
 *  @GOPAL_STATUS_CODE_FAIL_INTERVALTOOBRIEF
 *  @GOPAL_STATUS_CODE_FAIL_TEMPORARILYUNAVAILABLE
 *  @GOPAL_STATUS_CODE_FAIL_TRANSACTIONDOESNOTEXIST
 *  @GOPAL_STATUS_CODE_FAIL_LOOPDETECTED
 *  @GOPAL_STATUS_CODE_FAIL_TOOMANYHOPS
 *  @GOPAL_STATUS_CODE_FAIL_ADDRESSINCOMPLETE
 *  @GOPAL_STATUS_CODE_FAIL_AMBIGUOUS
 *  @GOPAL_STATUS_CODE_FAIL_BUSYHERE
 *  @GOPAL_STATUS_CODE_FAIL_REQUESTTERMINATED
 *  @GOPAL_STATUS_CODE_FAIL_NOTACCEPTABLEHERE
 *  @GOPAL_STATUS_CODE_FAIL_BADEVENT
 *  @GOPAL_STATUS_CODE_FAIL_REQUESTPENDING
 *  @GOPAL_STATUS_CODE_FAIL_UNDECIPHERABLE
 *  @GOPAL_STATUS_CODE_FAIL_INTERNALSERVERERROR
 *  @GOPAL_STATUS_CODE_FAIL_NOTIMPLEMENTED
 *  @GOPAL_STATUS_CODE_FAIL_BADGATEWAY
 *  @GOPAL_STATUS_CODE_FAIL_SERVICEUNAVAILABLE
 *  @GOPAL_STATUS_CODE_FAIL_SERVERTIMEOUT
 *  @GOPAL_STATUS_CODE_FAIL_SIPVERSIONNOTSUPPORTED
 *  @GOPAL_STATUS_CODE_FAIL_MESSAGETOOLARGE
 *  @GOPAL_STATUS_CODE_GLOBALFAIL_BUSYEVERYWHERE
 *  @GOPAL_STATUS_CODE_GLOBALFAIL_DECLINE
 *  @GOPAL_STATUS_CODE_GLOBALFAIL_DOESNOTEXISTANYWHERE
 *  @GOPAL_STATUS_CODE_GLOBALFAIL_NOTACCEPTABLE
 *  @GOPAL_STATUS_CODE_MAX
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
    guint restore_time;
    GTimeVal min_retry_time;
    GTimeVal max_retry_time;
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
    GopalSIPRegisterCompatibilityModes compatiblity_mode;
};

struct _GopalSIPRegistrationStatus {
    gchar *address_of_record;
    gboolean was_registering;
    gboolean re_registering;
    GopalStatusCodes reason;
};

GType
gopal_sip_ep_get_type                          (void) G_GNUC_CONST;

gboolean
gopal_sip_ep_register                          (GopalSIPEP *self,
						GopalSIPRegisterParams *params,
						const gchar **address_of_record,
						GopalStatusCodes *reason);

gboolean
gopal_sip_ep_start_listeners                   (GopalSIPEP *self,
                                                gchar **interfaces);

G_END_DECLS

#endif /* GGOPAL_SIP_EP_H */
