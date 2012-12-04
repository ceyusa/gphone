using Gtk;

namespace GPhone {

public class PhoneWindow : Gtk.Window {
	private Gtk.Entry url;
	private Gtk.Button action_button;

	construct {
		var vbox = new Gtk.Box (Gtk.Orientation.VERTICAL, 3);
		add (vbox);

		var hbox = new Gtk.Box (Gtk.Orientation.HORIZONTAL, 3);

		url = new Gtk.Entry ();
		url.set_width_chars (25);
		url.text = "sip:";
		url.overwrite_mode = false;
		hbox.pack_start (url, true, true, 5);

		action_button = new Gtk.Button.with_label ("Call");
		action_button.clicked.connect (() => {
				do_action (url.text);
			});
		hbox.pack_start (action_button, false, true, 5);

		var quit_button = new Gtk.Button.from_stock (Gtk.Stock.QUIT);
		quit_button.clicked.connect (() => {
				hide ();
				GLib.Idle.add (_quit);
			});
		hbox.pack_end (quit_button, false, true, 5);

		vbox.pack_start (hbox, true, true, 0);

		set_resizable (false);
		show_all ();
	}

	private bool _quit () {
		quit ();
		return false;
	}

	public void toggle_state () {
		if (action_button.label == "Call") {
			action_button.label = "Hang up";
		} else {
			action_button.label = "Call";
		}
	}

	public signal void do_action (string remote_party);
	public signal void quit ();
}

}
