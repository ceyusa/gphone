/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

#ifndef SOUND_GST_H
#define SOUND_GST_H

#include "mmbackend.h"

G_BEGIN_DECLS

gboolean
load_sound_channel                              (MmBackend *backend);

G_END_DECLS

#endif /* SOUND_GST_H */
