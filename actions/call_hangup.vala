/*
 * Copyright (C) 2013 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

using Gtk;
using Gdk;

namespace GPhone {

private class CallHangupAction : Gtk.Action {
	private ulong handler_id;
	private State _state;

	public View view { construct; private get; }
	public State state {
		set construct { _set_state (value); }
		get { return _state; }
	}

	public enum State {
		PROCESSING,
		TO_CALL,
		TO_HANGUP,
	}

	private Gtk.ActionEntry[] entries = {
		Gtk.ActionEntry () { name = null,
							 stock_id = "process-stop-symbolic",
							 label = _("Processing"),
							 accelerator = null,
							 tooltip = _("Processing"),
							 callback = null },
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

	public CallHangupAction (View view, State state) {
		Object (name: "ViewCombinedCallHangup", view: view, state: state);
	}

	private void _set_state (State state) {
		if (this._state == state && handler_id != 0)
			return;

		this._state = state;

		this.icon_name = entries[state].stock_id;
		this.tooltip = entries[state].tooltip;
		this.label = entries[state].label;

		if (handler_id != 0)
			this.disconnect (handler_id);

		if (state == State.TO_HANGUP) {
			sensitive = true;
			handler_id = this.activate.connect (view.cmd_op_hangup);
		} else if (state == State.TO_CALL) {
			sensitive = true;
			handler_id = this.activate.connect (view.cmd_op_call);
		} else if (state == State.PROCESSING) {
			sensitive = false;
			handler_id = 0;
		} else {
			assert_not_reached ();
		}
	}
}

}
