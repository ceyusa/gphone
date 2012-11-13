/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

#ifndef GOPAL_H
#define GOPAL_H

#include "gopalmanager.h"
#include "gopalsipep.h"

G_BEGIN_DECLS

void
gopal_init                                      (int *argc, char **argv[]);

void
gopal_deinit                                    (void);


GOptionGroup *
gopal_init_get_option_group                     (void);

void
gopal_set_debug_level                           (guint debug_level);

G_END_DECLS

#endif /* GOPAL_H */
