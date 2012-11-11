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

#include <glib-object.h>

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

GopalManager *
gopal_manager_new                               (void);

GopalSTUNClientNatType
gopal_manager_set_stun_server                   (GopalManager *self,
						 const char *server);

const char *
gopal_manager_get_stun_server                   (GopalManager *self);

gboolean
gopal_manager_set_translation_host              (GopalManager *self,
						 const char *host);

GopalSIPEP *
gopal_manager_get_sip_endpoint                  (GopalManager *self);

void
gopal_manager_shutdown_endpoints                (GopalManager *self);


G_END_DECLS

#endif /* GGOPAL_MANAGER_H */
