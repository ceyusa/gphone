GPhone.Controller controller;

static string config_file;
static string[] remote_parties;

const OptionEntry[] entries = {
	{ "config", 'c', 0, OptionArg.FILENAME, ref config_file,
	  "alternative configuration file", "FILE" },
	{ "", 0, 0, OptionArg.STRING_ARRAY, ref remote_parties,
	  null, "[REMOTE PARTY]" },
	{ null }
};

public void signal_handler (int signal) {
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

	try {
		Gopal.init_check (ref args);
		Gst.init_check (ref args);
		Gtk.init_check (ref args);
	} catch (Error error) {
		print ("Failed to init: %s", error.message);
		return -1;
	}

	Posix.signal (Posix.SIGABRT, signal_handler);
	Posix.signal (Posix.SIGINT, signal_handler);
	Posix.signal (Posix.SIGTERM, signal_handler);

	controller = new GPhone.Controller ();
	if (!controller.init (config_file)) {
		warning ("falied to initialisate gphone, bye...");
	} else {
		controller.run (remote_parties[0]);
	}
	controller = null;

	Gopal.deinit ();

	return 0;
}
