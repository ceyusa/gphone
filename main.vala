GLib.MainLoop loop;

public void signal_handler (int signal) {
	if (loop.is_running ()) {
		print ("\nQuitting...\n");
		loop.quit ();
	}
}

void run_ui (GPhone.Model model, string? remote) {
	var win = new GPhone.View ();

	win.quit.connect (() => {
			if (model.is_call_established ())
				model.hangup_call ();
			signal_handler (0);
		});
	win.show ();

	win.do_action.connect ((remote) => {
			if (!model.is_call_established ()) {
				message ("calling %s\n", remote);
				if (!model.make_call (remote))
					warning ("call failed!\n");
			} else {
				message ("hanging up\n");
				if (!model.hangup_call ())
					warning ("hangup failed!\n");
			}
		});

	model.call_established.connect (() => {
			win.toggle_state ();
		});

	model.call_hungup.connect (() => {
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
	var model = new GPhone.Model ();

	if (!model.initialisate ()) {
		warning ("falied to initialisate gphone, bye...");
	} else {
		run_ui (model, args[1]);
		loop.run ();
	}

	model = null;

	Gopal.deinit ();

	return 0;
}
