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
	public Gtk.ActionGroup toolbar_action_group { get; private set; }
	public UIManager manager { get; private set; }

	private Gtk.ActionGroup action_group;
	private State state;
	private Toolbar toolbar;

	public enum State {
		IDLE,
		CALLING
	}

	private Gtk.ActionEntry[] entries = {
		Gtk.ActionEntry () { name = "FileQuit",
							 stock_id = null,
							 label = _("_Quit"),
							 accelerator = "<control>Q",
							 tooltip = null,
							 callback = cmd_file_quit },
		Gtk.ActionEntry () { name = "HelpAbout",
							 stock_id = null,
							 label = _("_About"),
							 accelerator = null,
							 tooltip = null,
							 callback = cmd_help_about },
	};

	construct {
		setup_ui_manager ();

		var vbox = new Box (Orientation.VERTICAL, 0);
		add (vbox);

		toolbar = new Toolbar (this);
		toolbar.show_arrow = false;
		toolbar.expand = true;

		toolbar.location.activate.connect (cmd_op_call);

		vbox.pack_start (toolbar, true, true, 0);

		decorated = false;
		modal = true;
		set_resizable (false);
		set_keep_above (true);

		try {
			manager.add_ui_from_resource ("/org/gnome/gphone/gphone-ui.xml");
		} catch (Error err) {
			warning ("Could not merge gphone-ui.xml: %s", err.message);
		}
	}

	public View () {
		state = State.IDLE;
	}

	private bool quit_cb () {
		quit ();
		return false;
	}

	private bool set_calling_state_cb () {
		dynamic Gtk.Action action =
			toolbar_action_group.get_action ("ViewCombinedCallHangup");
		action.calling = true;

		toolbar.location.sensitive = false;

		return false;
	}

	private bool set_idle_state_cb () {
		dynamic Gtk.Action action =
			toolbar_action_group.get_action ("ViewCombinedCallHangup");
		action.calling = false;

		toolbar.location.sensitive = true;

		return false;
	}

	private void cmd_file_quit () {
		hide ();
		Idle.add (quit_cb);
	}

	private void cmd_help_about () {
		debug ("About");
	}

	public void cmd_op_call () {
		do_action (toolbar.location.text);
	}

	public void cmd_op_hangup () {
		do_action (toolbar.location.text);
	}

	public void set_ui_state (State new_state) {
		if (new_state == state)
			return;

		if (state == State.IDLE && new_state == State.CALLING) {
			Idle.add (set_calling_state_cb);
			state = new_state;
		} else if (state == State.CALLING && new_state == State.IDLE) {
			Idle.add (set_idle_state_cb);
			state = new_state;
		}
	}

	public void set_remote_party (string remote_party) {
		toolbar.location.set_location (remote_party);
		do_action (toolbar.location.text);
	}

	private void setup_ui_manager () {
		manager = new UIManager ();

		action_group = new Gtk.ActionGroup ("WindowActions");
		action_group.add_actions (entries, this);
		manager.insert_action_group (action_group, 0);

		toolbar_action_group = new Gtk.ActionGroup ("SpecialToolbarActions");

		Gtk.Action action = new CallHangupAction (this, false);
		toolbar_action_group.add_action (action);

		action = new PageMenuAction (this);
		toolbar_action_group.add_action_with_accel (action, "<alt>E");

		manager.insert_action_group (toolbar_action_group, 0);
		add_accel_group (manager.get_accel_group ());
	}

	public signal void do_action (string remote_party);
	public signal void quit ();
}

}
