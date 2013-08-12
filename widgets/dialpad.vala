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
