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

private class Location : Entry {
	public Location () {
		key_press_event.connect (on_key_press);
		key_press_event.connect_after (on_key_press_after);
		set_overwrite_mode (false);
		set_width_chars (25);

		set_location (null);
	}

	private bool on_key_press (EventKey event) {
		uint state = event.state & accelerator_get_default_mod_mask ();

		if (event.keyval == Key.Escape && state == 0)
			set_location (null);

		return false;
	}

	private bool on_key_press_after (EventKey event) {
		uint state = event.state & accelerator_get_default_mod_mask ();

		if ((event.keyval == Key.Return ||
			 event.keyval == Key.KP_Enter ||
			 event.keyval == Key.ISO_Enter) &&
			(state == ModifierType.CONTROL_MASK ||
			 state == (ModifierType.CONTROL_MASK | ModifierType.SHIFT_MASK))) {
			activate ();

			return true;
		}

		return false;
	}

	public void set_location (string? address) {
		if (address != null) {
			if (!address.has_prefix ("sip:")) {
				text = text.concat (address);
			} else {
				text = address;
			}
		} else {
			text = "sip:";
		}

		set_position (-1);
	}
}

private class Toolbar : Gtk.Toolbar {
	public Location location { get; private set; }
	public View view { construct ; private get; }

	public Toolbar (View view) {
		Object (view: view);
	}

	construct {
		// location party and call/hangup
		var item = new ToolItem ();

		// remote party
		location = new Location ();
		item.add (location);
		add (item);

		location.show_all ();

		// call/hangup
		var tool_button = new ToolButton (null, null);
		var action = view.toolbar_action_group.get_action ("ViewCombinedCallHangup");
		tool_button.set_related_action (action);
		add (tool_button);

		tool_button.show_all ();

		// Page Menu
		var tool_item = new ToolItem ();
		var button = new Button ();
		button.set_name ("gphone-page-menu-button");
		// FIXME: apparently we need an image inside the button for
		// the action icon to appear
		button.set_image (new Image ());
//		action = view.toolbar_action_group.get_action ("PageMenu");
//		button.set_related_action (action);
		tool_item.add (button);
		add (tool_item);

		tool_item.show_all ();
	}
}

}
