using Gtk;

namespace GPhone {

public class PhoneWindow : Window {
	private Entry url;
	private Button action_button;

	construct {
		var vbox = new Box (Orientation.VERTICAL, 3);
		add (vbox);

		var hbox = new Box (Orientation.HORIZONTAL, 3);

		url = new Entry ();
		url.set_width_chars (25);
		url.text = "sip:";
		url.overwrite_mode = false;
		hbox.pack_start (url, true, true, 5);

		action_button = new Button.with_label ("Call");
		action_button.clicked.connect (() => {
				do_action (url.text);
			});
		hbox.pack_start (action_button, false, true, 5);

		var quit_button = new Button.from_stock (Stock.QUIT);
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

	public void set_remote_party (string remote) {
		if (!remote.has_prefix ("sip:"))
			url.text = url.text.concat (remote);
		else
			url.text = remote;

		do_action(url.text);
	}

	public signal void do_action (string remote_party);
	public signal void quit ();
}

}
