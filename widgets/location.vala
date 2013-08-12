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

		if (event.keyval == Key.Down || event.keyval == Key.KP_Down) {
			// If we are focusing the entry, with the cursor at the
			// end of it we emit the changed signal, so that the
			// completion popup appears
			var string = get_text ();
			if (get_position () == string.length) {
				changed ();
				return true;
			}
		}

		return false;
	}

	public void set_location (string? address) {
		if (address != null) {
			if (!address.has_prefix ("sip:")) {
				text = "sip:" + address;
			} else {
				text = address;
			}
		} else {
			text = "sip:";
		}

		set_position (-1);
	}
}

}
