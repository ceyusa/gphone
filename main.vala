/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

GPhone.Controller controller;

static string config_file;
[CCode (array_length = false, array_null_terminated = true)]
static string[] remote_parties;

const OptionEntry[] entries = {
	{ "config", 'c', 0, OptionArg.FILENAME, ref config_file,
	  "alternative configuration file", "FILE" },
	{ "", 0, 0, OptionArg.STRING_ARRAY, ref remote_parties,
	  null, "[REMOTE PARTY]" },
	{ null }
};

void signal_handler (int signal) {
	if (controller.is_running ()) {
		controller.quit ();
	}
}

int main (string[] args) {
	var options = new OptionContext ("- GPhone application");
	options.add_main_entries(entries, null);
	options.add_group (Gtk.get_option_group (true));
	options.add_group (Gst.init_get_option_group ());
	options.add_group (Gopal.init_get_option_group ());
	try {
		options.parse (ref args);
	} catch (OptionError error) {
		print ("Option parsing failed: %s\n", error.message);
		return -1;
	}

	if (!Notify.init ("GPhone")) {
		print ("Failed to init libnotify\n");
		return -1;
	}

	Posix.signal (Posix.SIGABRT, signal_handler);
	Posix.signal (Posix.SIGINT, signal_handler);
	Posix.signal (Posix.SIGTERM, signal_handler);

	if (config_file == null) {
		config_file = Environment.get_user_config_dir () + "/gphone.conf";
	}

	string remote = null;
	if (remote_parties != null) {
		remote = remote_parties[0];
	}

	Environment.set_prgname ("gphone");
	Environment.set_application_name (_("GPhone"));

	controller = new GPhone.Controller ();
	if (!controller.init (config_file)) {
		warning ("falied to initialisate gphone, bye...");
	} else {
		controller.run (remote);
	}
	controller = null;

	Gopal.deinit ();

	if (Notify.is_initted ()) {
		Notify.uninit ();
	}

	return 0;
}
