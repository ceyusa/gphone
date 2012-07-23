/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

#include "gopalmanager.h"

#include <ptlib.h>
#include <opal/manager.h>

class MyManager : public OpalManager
{
    PCLASSINFO(MyManager, OpalManager);

public:
    MyManager(GOpalManager *mgr) : m_manager(mgr) { };

private:
    GOpalManager *m_manager;
};

struct _GOpalManagerPrivate
{
    MyManager *manager;
};

#define GET_PRIVATE(obj)						\
        (G_TYPE_INSTANCE_GET_PRIVATE((obj), GOPAL_TYPE_MANAGER, GOpalManagerPrivate))

G_DEFINE_TYPE(GOpalManager, gopal_manager, G_TYPE_OBJECT)

static void
gopal_manager_finalize (GObject *object)
{
    GOpalManager *self = (GOpalManager *) object;

    delete self->priv->manager;

    G_OBJECT_CLASS(gopal_manager_parent_class)->finalize(object);
}

static void
gopal_manager_class_init (GOpalManagerClass *klass)
{
    GObjectClass *gobject_class = (GObjectClass *) klass;

    gobject_class->finalize = gopal_manager_finalize;

    g_type_class_add_private (klass, sizeof (GOpalManagerPrivate));
}

static void
gopal_manager_init (GOpalManager *self)
{
    self->priv = GET_PRIVATE (self);
    self->priv->manager = new MyManager(self);
}
