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
