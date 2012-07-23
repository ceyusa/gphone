#include <glib.h>

#include "gopal.h"

static gboolean
quit (gpointer data)
{
    GMainLoop *loop;

    loop = data;
    g_main_loop_quit (loop);
    return FALSE;
}

int
main(int argc, char **argv)
{
    GOpalManager *manager;
    GMainLoop *loop;

    gopal_init (&argc, &argv);

    manager = g_object_new (GOPAL_TYPE_MANAGER, NULL);
    loop = g_main_loop_new (NULL, FALSE);

    g_timeout_add_seconds (3, quit, loop);
    g_main_loop_run (loop);

    g_main_loop_unref (loop);
    g_object_unref (manager);

    gopal_deinit ();
}
