/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

#include "gopal.h"

#include <ptlib.h>
#include <ptlib/pprocess.h>

#include "soundgst.h"

static gboolean gopal_initialized = FALSE;
static PLibraryProcess *process = NULL;
static MmBackend *mmbackend = NULL;

static gboolean
parse_goption_arg (const char *opt,
		   const char *arg,
		   gpointer data,
		   GError **err)
{
    if (!strcmp (opt, "--gopal-debug-level") ||
	!strcmp (opt, "-d")) {
	long int debug_level = 0;

	debug_level = strtol (arg, NULL, 0);
	if ((guint) debug_level < 9) {
	    gopal_set_debug_level (debug_level);
	}
    } else {
	g_set_error (err, G_OPTION_ERROR, G_OPTION_ERROR_UNKNOWN_OPTION,
		     "Unknown option");
	return FALSE;
    }

    return TRUE;
}

static gboolean
init_pre (GOptionContext *ctxt,
	  GOptionGroup *group,
	  gpointer data,
	  GError **error)
{
    if (gopal_initialized) {
	g_debug ("already initialized");
	return TRUE;
    }

#if !GLIB_CHECK_VERSION(2, 35, 0)
    g_type_init ();
#endif

    return TRUE;
}

static gboolean
load_plugins ()
{
    mmbackend = mm_backend_new ();
    if (!load_sound_channel (mmbackend))
	return FALSE;

    return TRUE;
}

static gboolean
init_post (GOptionContext *ctxt,
	   GOptionGroup *group,
	   gpointer data,
	   GError **error)
{
    if (gopal_initialized) {
	g_debug ("already initialized");
	return TRUE;
    }

    if (!load_plugins ())
	return FALSE;

    process = new PLibraryProcess ();

    gopal_initialized = TRUE;

    return TRUE;
}


/**
 * gopal_init_get_option_group: (skip)
 *
 * Returns a #GOptionGroup with GOpal's argument specifications. The
 * group is set up to use standard GOption callbacks, so when using
 * this group in combination with GOption parsing methods, all
 * argument parsing and initialization is automated.
 *
 * This function is useful if you want to integrate GOpal with other
 * libraries that use GOption (see g_option_context_add_group() ).
 *
 * If you use this function, you should make sure you initialise the GLib
 * threading system as one of the very first things in your program
 * (see the example at the beginning of this section).
 *
 * Returns: (transfer full): a pointer to GOpal's option group.
 */
GOptionGroup *
gopal_init_get_option_group (void)
{
    GOptionGroup *group;

    static const GOptionEntry args[] = {
	{ "gopal-debug-level", 'd', 0, G_OPTION_ARG_CALLBACK,
	  (gpointer) parse_goption_arg,
	  "Default debug level from 1 (only error) to 8 (anything) or "
	  "0 for no output",
	  "LEVEL" },
	{ NULL, 0, 0, G_OPTION_ARG_NONE, NULL, NULL, NULL, }
    };

    group = g_option_group_new ("gopal", "GOpal Options",
				"Show GOpal Options", NULL, NULL);
    g_option_group_set_parse_hooks (group, init_pre, init_post);
    g_option_group_add_entries (group, args);

    return group;
}

/**
 * gopal_set_debug_level:
 * @debug_level: Opal's debug level
 *
 * Sets the debug level for the internal Opal's trace system.
 *
 * It is send to default output, and formatted with timestam, thread
 * id and file+line which throws the log message.
 */
void
gopal_set_debug_level (guint debug_level)
{
    PTrace::Initialise (debug_level, NULL,
			PTrace::Timestamp | PTrace::Thread | PTrace::FileAndLine);
}

/**
 * gopal_init_check:
 * @argc: (inout) (allow-none): pointer to application's argc
 * @argv: (inout) (array length=argc) (allow-none): pointer to application's argv
 * @error: pointer to a #GError to which a message will be posted on error
 *
 * Initializes the GOpal library, setting up internal path lists,
 * registering built-in elements, and loading standard plugins.
 *
 * This function should be called before calling any other GLib functions. If
 * this is not an option, your program must initialise the GLib thread system
 * using g_thread_init() before any other GLib functions are called.
 *
 * <note><para>
 * This function will terminate your program if it was unable to initialize
 * GOpal for some reason.
 * </para></note>
 *
 * WARNING: This function does not work in the same way as corresponding
 * functions in other glib-style libraries, such as gtk_init().  In
 * particular, unknown command line options cause this function to
 * abort program execution.
 */
gboolean
gopal_init_check (int *argc, char **argv[], GError **error)
{
    GOptionGroup *group;
    GOptionContext *ctx;
    gboolean res;

    if (gopal_initialized)
	return TRUE;

    error = NULL;
    ctx = g_option_context_new ("- GOpal initialization");
    g_option_context_set_ignore_unknown_options (ctx, TRUE);
    group = gopal_init_get_option_group ();
    g_option_context_add_group (ctx, group);
    res = g_option_context_parse (ctx, argc, argv, error);
    g_option_context_free (ctx);

    gopal_initialized = res;

    return res;
}

/**
 * gopal_deinit:
 *
 * Clean up any resources created by Gopal in gopal_init().
 *
 * It is normally not needed to call this function in a normal application
 * as the resources will automatically be freed when the program terminates.
 * This function is therefore mostly used by testsuites and other memory
 * profiling tools.
 *
 * After this call Gopal (including this method) should not be used anymore.
 */
void
gopal_deinit ()
{
    if (mmbackend)
	g_object_unref (mmbackend);

    if (process)
	delete process;
}
