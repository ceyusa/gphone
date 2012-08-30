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
    (G_TYPE_CHECK_INSTANCE_CAST((obj), GOPAL_TYPE_SIP_EP, GOpalSIPEP))
#define GOPAL_SIP_EP_CLASS(klass)		\
    (G_TYPE_CHECK_CLASS_CAST((klass),  GOPAL_TYPE_SIP_EP, GOpalSIPEPClass))
#define GOPAL_IS_SIP_EP(obj)		\
    (G_TYPE_CHECK_INSTANCE_TYPE((obj), GOPAL_TYPE_SIP_EP))
#define GOPAL_IS_SIP_EP_CLASS(klass)	\
    (G_TYPE_CHECK_CLASS_TYPE((klass),  GOPAL_TYPE_SIP_EP))
#define GOPAL_SIP_EP_GET_CLASS(obj)					\
    (G_TYPE_INSTANCE_GET_CLASS((obj),  GOPAL_TYPE_SIP_EP, GOpalSIPEPClass))

typedef struct _GOpalSIPEPPrivate GOpalSIPEPPrivate;
typedef struct _GOpalSIPEP GOpalSIPEP;
typedef struct _GOpalSIPEPClass GOpalSIPEPClass;

struct _GOpalSIPEP {
    GObject parent;

    /*< private >*/
    GOpalSIPEPPrivate *priv;
};

struct _GOpalSIPEPClass {
    GObjectClass parent_class;
};

typedef struct _GOpalSIPRegisterParams GOpalSIPRegisterParams;

struct _GOpalSIPRegisterParams {
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
gopal_sip_ep_register                          (GOpalSIPEP *self,
						GOpalSIPRegisterParams *params,
						const gchar **address_of_record);

G_END_DECLS

#endif /* GGOPAL_SIP_EP_H */
