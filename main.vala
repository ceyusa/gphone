GLib.MainLoop loop;

public void signal_handler (int signal) {
	if (loop.is_running ()) {
		print ("\nQuitting...\n");
		loop.quit ();
	}
}

void run_ui (GPhone.Controller controller, string? remote) {
	var win = new GPhone.PhoneWindow ();

	win.quit.connect (() => {
			if (controller.is_call_established ())
				controller.hangup_call ();
			signal_handler (0);
		});
	win.show ();

	win.do_action.connect ((remote) => {
			if (!controller.is_call_established ()) {
				message ("calling %s\n", remote);
				if (!controller.make_call (remote))
					warning ("call failed!\n");
			} else {
				message ("hanging up\n");
				if (!controller.hangup_call ())
					warning ("hangup failed!\n");
			}
		});

	controller.call_established.connect (() => {
			win.toggle_state ();
		});

	controller.call_hungup.connect (() => {
			win.toggle_state ();
		});

	if (remote != null) {
		debug ("remote: %s", remote);
		win.set_remote_party (remote);
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

	loop = new GLib.MainLoop ();
	var controller = new GPhone.Controller ();

	if (!controller.initialisate ()) {
		warning ("falied to initialisate gphone, bye...");
	} else {
		run_ui (controller, args[1]);
		loop.run ();
	}

	controller = null;

	Gopal.deinit ();

	return 0;
}
