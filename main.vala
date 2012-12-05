GPhone.Controller controller;

public void signal_handler (int signal) {
	if (controller.is_running ()) {
		controller.quit ();
	}
}

int main (string[] args) {
	var options = new OptionContext ("");
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

	if (!controller.init (null)) {
		warning ("falied to initialisate gphone, bye...");
	} else {
		controller.run (args[1]);
	}

	controller = null;

	Gopal.deinit ();

	return 0;
}
