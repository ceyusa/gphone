/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

#ifndef MM_BACKEND_H
#define MM_BACKEND_H

#include <glib-object.h>

G_BEGIN_DECLS

#define MM_TYPE_BACKEND (mm_backend_get_type())
#define MM_BACKEND(obj)                      \
        (G_TYPE_CHECK_INSTANCE_CAST((obj), MM_TYPE_BACKEND, MmBackend))
#define MM_BACKEND_CLASS(klass)              \
        (G_TYPE_CHECK_CLASS_CAST((klass),  MM_TYPE_BACKEND, MmBackendClass))
#define MM_IS_BACKEND(obj)                   \
        (G_TYPE_CHECK_INSTANCE_TYPE((obj), MM_TYPE_BACKEND))
#define MM_IS_BACKEND_CLASS(klass)           \
        (G_TYPE_CHECK_CLASS_TYPE((klass),  MM_TYPE_BACKEND))
#define MM_BACKEND_GET_CLASS(obj)            \
        (G_TYPE_INSTANCE_GET_CLASS((obj),  MM_TYPE_BACKEND, MmBackendClass))

typedef struct _MmBackend MmBackend;
typedef struct _MmBackendPrivate MmBackendPrivate;
typedef struct _MmBackendClass MmBackendClass;

struct _MmBackend {
    GObject parent;

    /*< private >*/
    MmBackendPrivate *priv;
};

struct _MmBackendClass {
    GObjectClass parent_class;
};

typedef enum {
    MM_BACKEND_DIRECTION_CLOSED = -1,
    MM_BACKEND_DIRECTION_RECORDER,
    MM_BACKEND_DIRECTION_PLAYER
} MmBackendDirection;

GType
mm_backend_get_type                             (void) G_GNUC_CONST;

MmBackend *
mm_backend_new                                  (void);

gboolean
mm_backend_is_playing                           (MmBackend *self);

gboolean
mm_backend_audio_open                           (MmBackend *self,
                                                 const char *dev,
                                                 MmBackendDirection dir,
                                                 guint channels,
                                                 guint rate);

gboolean
mm_backend_audio_is_open                        (MmBackend *self);

gboolean
mm_backend_audio_close                          (MmBackend *self);

void
mm_backend_set_buffer_size                      (MmBackend *self,
                                                 MmBackendDirection dir,
                                                 size_t size);

gboolean
mm_backend_audio_write                          (MmBackend *self,
                                                 const void *buf,
                                                 size_t len,
                                                 size_t *written);

gboolean
mm_backend_audio_read                           (MmBackend *self,
                                                 void *buf,
                                                 size_t len,
                                                 size_t *read);

G_END_DECLS

#endif /* MM_BACKEND_H */
