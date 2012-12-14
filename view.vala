/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

using Gtk;

namespace GPhone {

public class View : Window {
	private Entry remote;
	private State state;

	public enum State {
		IDLE,
		CALLING
	}

	construct {
		var vbox = new Box (Orientation.VERTICAL, 0);
		add (vbox);

		var toolbar = new Toolbar ();
		toolbar.show_arrow = false;
		toolbar.expand = true;

		var item = new ToolItem ();
		remote = new Entry ();
		remote.set_width_chars (25);
		remote.text = "sip:";
		remote.overwrite_mode = false;
		item.add (remote);
		toolbar.add (item);

		var call_button = new ToolButton(null, null);
		call_button.icon_name = "call-start-symbolic";
		call_button.clicked.connect (() => {
				do_action (remote.text);
			});
		toolbar.add (call_button);

		var menu_button = new ToolButton (null, _("Menu"));
		menu_button.icon_name = "emblem-system-symbolic";
		menu_button.clicked.connect (() => {
				hide ();
				GLib.Idle.add (_quit);
			});
		toolbar.add (menu_button);

		vbox.pack_start (toolbar, true, true, 0);

		decorated = false;
		modal = true;
		set_resizable (false);
		set_keep_above (true);
	}

	public View () {
		state = State.IDLE;
	}

	private bool _quit () {
		quit ();
		return false;
	}

	public void toggle_state () {
		// if (action_button.label == "Call") {
		// 	action_button.label = "Hang up";
		// } else {
		// 	action_button.label = "Call";
		// }
	}

	public void set_remote_party (string remote_party) {
		if (!remote_party.has_prefix ("sip:"))
			remote.text = remote.text.concat (remote_party);
		else
			remote.text = remote_party;

		do_action (remote.text);
	}

	public signal void do_action (string remote_party);
	public signal void quit ();
}

}
