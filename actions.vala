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

private class CallHangupAction : Gtk.Action {
	private ulong handler_id;
	private State state;

	public View view { construct; private get; }
	public bool calling {
		set construct {
			set_state (value ? State.HANGUP : State.CALL);
		}
		get { return state == State.HANGUP; }
	}

	private enum State {
		CALL,
		HANGUP
	}

	private Gtk.ActionEntry[] entries = {
		Gtk.ActionEntry () { name = null,
							 stock_id = "call-start-symbolic",
							 label = _("Call"),
							 accelerator = null,
							 tooltip = _("Start call"),
							 callback = null },
		Gtk.ActionEntry () { name = null,
							 stock_id = "call-end-symbolic",
							 label = _("Hang up"),
							 accelerator = null,
							 tooltip = _("Hang up call"),
							 callback = null }
	};

	public CallHangupAction (View view, bool calling) {
		Object (name: "ViewCombinedCallHangup", view: view, calling: calling);
	}

	private void set_state (State state) {
		if (this.state == state && handler_id != 0)
			return;

		this.state = state;

		this.icon_name = entries[state].stock_id;
		this.tooltip = entries[state].tooltip;
		this.label = entries[state].label;

		if (handler_id != 0)
			this.disconnect (handler_id);

		if (state == State.HANGUP)
			handler_id = this.activate.connect (on_hangup);
		else if (state == State.CALL)
			handler_id = this.activate.connect (on_call);
	}

	public void on_call () {
		print ("on call!\n");
		calling = true;
	}

	public void on_hangup () {
		print ("on hangup!\n");
		calling = false;
	}
}

}
