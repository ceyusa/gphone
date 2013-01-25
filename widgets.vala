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
		action = view.toolbar_action_group.get_action ("PageMenu");
		button.set_related_action (action);
		tool_item.add (button);
		add (tool_item);

		tool_item.show_all ();
	}
}

private class DialPad : Box {
	private struct Key {
		public string number;
		public string letters;
		public uint code;

		public Key (string number, string letters, uint code) {
			this.number = number;
			this.letters = letters;
			this.code = code;
		}
	}

	private Key[] keys = { Key ("1", "", Gdk.Key.KP_1),
						   Key ("2", _("abc"),  Gdk.Key.KP_2),
						   Key ("3", _("def"),  Gdk.Key.KP_3),
						   Key ("4", _("ghi"),  Gdk.Key.KP_4),
						   Key ("5", _("jkl"),  Gdk.Key.KP_5),
						   Key ("6", _("mno"),  Gdk.Key.KP_6),
						   Key ("7", _("pqrs"), Gdk.Key.KP_7),
						   Key ("8", _("tuv"),  Gdk.Key.KP_8),
						   Key ("9", _("wkyz"), Gdk.Key.KP_9),
						   Key ("*", "", Gdk.Key.KP_Multiply),
						   Key ("0", "", Gdk.Key.KP_0),
						   Key ("#", "", Gdk.Key.numbersign) };

	private Button[] buttons;
	public AccelGroup accel_group;

	public View view { construct ; private get; }

	construct {
		// the dialpad is LTR even for RTL languages
		set_direction (TextDirection.LTR);

		set_orientation (Orientation.VERTICAL);
		set_spacing (3);


		accel_group = new AccelGroup ();
		buttons = new Button[keys.length];

		var grid = new Grid ();
		grid.set_column_homogeneous (true);
		grid.set_row_homogeneous (true);

		for (int i = 0; i < keys.length; i++) {
			var k = keys[i];

			var box = new Box (Orientation.VERTICAL, 0);
			box.homogeneous = false;
			var label = new Label (null);
			var str = ("<span size='x-large'>%s</span>").printf (k.number);
			label.set_markup (str);
			box.pack_start (label, true, true, 0);

			label = new Label (null);
			if (k.letters.length > 0) {
				str = ("<span foreground='#555555'>%s</span>").printf (k.letters);
				label.set_markup (str);
			}
			box.pack_start (label, false, false, 0);

			buttons[i] = new Button ();
			buttons[i].set_border_width (0);
			buttons[i].add (box);
			buttons[i].set_data ("number", k.number);

			grid.attach (buttons[i], i % 3, i / 3, 1, 1);

			buttons[i].clicked.connect ((button) => {
					string number = button.get_data ("number");
					view.input_tone (number);
				});

			buttons[i].add_accelerator ("clicked", accel_group, k.code, 0, 0);
		}

		pack_start (grid, false, false, 3);

		view.add_accel_group (accel_group);
	}

	public DialPad (View view) {
		Object (view: view);
	}
}

}
