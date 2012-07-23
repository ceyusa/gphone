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
    GOpalSTUNClientNatType nat_type;

    gopal_init (&argc, &argv);

    manager = g_object_new (GOPAL_TYPE_MANAGER, NULL);
    nat_type = gopal_manager_set_stun_server (manager, "stun.ekiga.net");
    g_print ("NAT type = %d %s\n", nat_type,
	     gopal_manager_get_stun_server (manager));

    loop = g_main_loop_new (NULL, FALSE);

    g_timeout_add_seconds (3, quit, loop);
    g_main_loop_run (loop);

    g_main_loop_unref (loop);
    g_object_unref (manager);

    gopal_deinit ();
}
