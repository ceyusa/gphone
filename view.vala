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
	private EntryCompletion completion;
	private Notebook notebook;

	public enum State {
		INACTIVE, // UA is down and cannot make calls.
		IDLE,     // UA is available but not in use
		RINGING,  // UA has an incoming call
		ALERTING, // Remote party is being alerted
		CALLING,  // Call is established
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

		completion = new EntryCompletion ();
		completion.inline_selection = true;
		completion.set_text_column (0);

		toolbar.location.set_completion (completion);

		vbox.pack_start (toolbar, true, true, 0);

		notebook = new Notebook ();
		notebook.set_show_tabs (false);
		notebook.set_show_border (false);
		notebook.set_scrollable (false);
		notebook.popup_disable ();
		notebook.no_show_all = true;
		vbox.pack_start (notebook, true, true, 0);

		// Page 0
		var dialpad = new DialPad (this);
		dialpad.set_halign (Align.CENTER);
		dialpad.set_valign (Align.START);
		dialpad.show_all ();

		notebook.append_page (dialpad, null);

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
		state = State.INACTIVE;
	}

	private bool quit_cb () {
		quit ();
		return false;
	}

	private void cmd_file_quit () {
		hide ();
		Idle.add (quit_cb);
	}

	private void cmd_help_about () {
		string[] authors = { "Víctor Jáquez" };
		show_about_dialog (this,
						   "program-name", "GPhone",
						   "title", _("About GPhone"),
						   "website", "https://github.com/ceyusa/gphone",
						   "copyright","Copyright 2013 By Igalia S.L.",
						   "authors", authors,
						   "logo-icon-name", "phone-symbolic",
						   "license-type", License.GPL_2_0);
	}

	public void cmd_op_call () {
		if (state == State.IDLE)
			call (toolbar.location.text);
	}

	public void cmd_op_hangup () {
		if (state == State.CALLING)
			hangup ();
	}

	public void set_ui_state (State new_state) {
		if (new_state == state)
			return;

		dynamic Gtk.Action action = toolbar_action_group.get_action
		("ViewCombinedCallHangup");

		if (state == State.INACTIVE && new_state == State.IDLE) {
			action.set_sensitive (true);
			state = new_state;
		} else if (state == State.IDLE && new_state == State.ALERTING) {
			action.calling = true;
			toolbar.location.sensitive = false;
			state = new_state;
		} else if (state == State.IDLE && new_state == State.RINGING) {
			// a remote party is calling us
			// show dialog
			state = new_state;
		} else if (state == State.ALERTING && new_state == State.IDLE) {
			// the call was rejected by remote party
			action.calling = false;
			toolbar.location.sensitive = true;
			state = new_state;
		} else if (state == State.ALERTING && new_state == State.CALLING) {
			// the call was accepted by remote party
			state = new_state;
			show_dialpad ();
		} else if (state == State.RINGING && new_state == State.CALLING) {
			// the call was accepted by us
			state = new_state;
			show_dialpad ();
		} else if (state == State.CALLING && new_state == State.IDLE) {
			// hangup the call
			action.calling = false;
			toolbar.location.sensitive = true;
			hide_controls ();
			state = new_state;
		} else {
			critical ("Undefined state transition!");
		}
	}

	public void set_remote_party (string remote_party) {
		toolbar.location.set_location (remote_party);
		cmd_op_call ();
	}

	public void set_called_parties (List<string>? called_parties) {
		if (called_parties == null) {
			completion.set_model (null);
			return;
		}

		TreeIter iter;
		var model = new ListStore (1, typeof (string));

		foreach (string party in called_parties) {
			model.append (out iter);
			model.set (iter, 0, party);
		}

		completion.set_model (model);
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

	private void show_dialpad () {
		notebook.set_current_page (0);
		notebook.show ();
	}

	private void hide_controls () {
		toolbar.location.set_location (null);
		notebook.hide ();
	}

	public void display_notification (string summary, string? body) {
		var s = _("GPhone") + ": " + summary;
		var notify = new Notify.Notification (s, body, "phone-symbolic");
		notify.set_hint ("transient", new Variant.boolean (true));
		try {
			notify.show ();
		} catch {
			message ("%s: %s", summary, body);
		}
	}

	public signal void call (string remote_party);
	public signal void hangup ();
	public signal void quit ();
	public signal void input_tone (string tone);
	public signal void accept ();
	public signal void reject ();
}

}
