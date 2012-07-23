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

static gboolean gopal_initialized = FALSE;
static PLibraryProcess *process = NULL;

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

GOptionGroup *
gopal_get_option_group (void)
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
    g_option_group_add_entries (group, args);

    return group;
}

void
gopal_set_debug_level (guint debug_level)
{
    PTrace::Initialise (debug_level, NULL,
			PTrace::Timestamp | PTrace::Thread | PTrace::FileAndLine);
}

void
gopal_init (int *argc, char **argv[])
{
    GOptionGroup *group;
    GOptionContext *ctx;
    gboolean res;
    GError *error;

    if (gopal_initialized)
	goto fail;

    error = NULL;
    ctx = g_option_context_new ("- GOpal initialization");
    g_option_context_set_ignore_unknown_options (ctx, TRUE);
    group = gopal_get_option_group ();
    g_option_context_add_group (ctx, group);
    res = g_option_context_parse (ctx, argc, argv, &error);
    g_option_context_free (ctx);

    gopal_initialized = res;

    if (!res)
	goto fail;

    g_type_init ();

    process = new PLibraryProcess ();

    return;

fail:
    g_print ("Could not initialize GStreamer: %s\n",
	     error ? error->message : "unknown error occurred");
    if (error)
	g_error_free (error);
    exit (1);
}

void
gopal_deinit ()
{
    delete process;
}
