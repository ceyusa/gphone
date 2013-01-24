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
using Gdk;

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
		set_sensitive (false);
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
			handler_id = this.activate.connect (view.cmd_op_hangup);
		else if (state == State.CALL)
			handler_id = this.activate.connect (view.cmd_op_call);
	}
}

private class PageMenuAction : Gtk.Action {
	private Widget menu;
	private Widget button;

	public View view { construct; private get; }

	public PageMenuAction (View view) {
		Object (name: "PageMenu", view: view, icon_name: "emblem-system-symbolic");
	}

	public override void activate () {
		unowned GLib.SList<Gtk.Widget> l = get_proxies ();
		if (l.data is Button) {
			var ev = EventButton () { button = 1, time = 0 };
			var src = (Button) l.data;
			on_button_press_event (src, ev);
		}
	}

	public override void connect_proxy (Widget proxy) {
		if (proxy is Button)
			proxy.button_press_event.connect (on_button_press_event);

		base.connect_proxy (proxy);
	}

	public override void disconnect_proxy (Widget proxy) {
		if (proxy is Button)
			proxy.button_press_event.disconnect (on_button_press_event);

		base.disconnect_proxy (proxy);
	}

	private bool on_button_press_event (Widget src, Gdk.EventButton ev) {
		if (menu == null) {
			menu = view.manager.get_widget ("/ui/PagePopup");
			menu.notify["visible"].connect (on_visible);
			button = src;
		}

		(menu as Gtk.Menu).popup (null, null, menu_position_func,
								  ev.button, ev.time);

		return true;
	}

	private void on_visible (ParamSpec pspec) {
		if (menu.get_visible ())
			button.get_style_context ().add_class ("active-menu");
		else
			button.get_style_context ().remove_class ("active-menu");
	}

	private void menu_position_func (Gtk.Menu menu,
									 out int x,
									 out int y,
									 out bool push_in) {
		Requisition req;
		Allocation allocation;

		var toplevel = menu.get_toplevel ();
		(toplevel as Gtk.Window).set_type_hint (WindowTypeHint.DROPDOWN_MENU);

		menu.get_preferred_size (out req, null);

		var direction = button.get_direction ();
		var window = button.get_window ();

		button.get_allocation (out allocation);

		window.get_origin (out x, out y);
		x = x + allocation.x;
		y = y + allocation.y + allocation.height;

		if (direction == TextDirection.LTR)
			x = x + allocation.width - req.width;

		push_in = false;
	}
}

}
