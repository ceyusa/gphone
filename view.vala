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
	public UIManager manager { get; private set; }

	private Entry remote;
	private ToolButton call_button;
	private ToolButton hang_button;
	private State state;

	public enum State {
		IDLE,
		CALLING
	}

	construct {
		setup_ui_manager ();

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

		call_button = new ToolButton(null, _("Call"));
		call_button.icon_name = "call-start-symbolic";
		call_button.set_tooltip_text (_("Call"));
		call_button.clicked.connect (() => {
				do_action (remote.text);
			});
		toolbar.add (call_button);

		hang_button = new ToolButton(null, _("Hang up"));
		hang_button.icon_name = "call-end-symbolic";
		hang_button.set_tooltip_text (_("Hang up"));
		hang_button.clicked.connect (() => {
				do_action (remote.text);
			});
		toolbar.add (hang_button);
		hang_button.no_show_all = true;

		var menu_button = new ToolButton (null, _("Quit"));
		menu_button.icon_name = "emblem-system-symbolic";
		menu_button.set_tooltip_text (_("Quit"));
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

	public void set_ui_state (State new_state) {
		if (new_state == state)
			return;

		if (state == State.IDLE && new_state == State.CALLING) {
			call_button.hide ();
			hang_button.show ();
			state = new_state;
		} else if (state == State.CALLING && new_state == State.IDLE) {
			call_button.show ();
			hang_button.hide ();
			state = new_state;
		}
	}

	public void set_remote_party (string remote_party) {
		if (!remote_party.has_prefix ("sip:"))
			remote.text = remote.text.concat (remote_party);
		else
			remote.text = remote_party;

		do_action (remote.text);
	}

	private void setup_ui_manager () {
		manager = new UIManager ();
	}

	public signal void do_action (string remote_party);
	public signal void quit ();
}

}
