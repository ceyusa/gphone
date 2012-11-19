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
#include "gopalmanager.h"

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
