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

private class IncomingCall : Box {

	private Label msg;

	public View view { construct ; private get; }

	construct {
		set_orientation (Orientation.VERTICAL);

		var image = new Image.from_icon_name ("dialog-question-symbolic",
											  IconSize.DIALOG);
		image.set_halign (Align.CENTER);
		image.set_valign (Align.START);

		var label = new Label (null);
		var str = ("<b>%s</b>").printf (_("Incoming call"));
		label.set_markup (str);
		label.set_line_wrap (true);
		label.set_selectable (true);
		label.set_halign (Align.CENTER);
		label.set_valign (Align.START);
		label.set_alignment (0.5f, 0.5f);

		msg = new Label (null);
		msg.set_line_wrap (true);
		msg.set_selectable (true);
		msg.set_halign (Align.CENTER);
		msg.set_valign (Align.CENTER);
		msg.set_alignment (0.5f, 0.5f);

		var msg_area = new Box (Orientation.VERTICAL, 12);
		msg_area.pack_start (label, false, false, 0);
		msg_area.pack_start (msg, true, true, 0);

		var hbox = new Box (Orientation.HORIZONTAL, 12);
		hbox.set_border_width (5);
		hbox.pack_start (image, false, false);
		hbox.pack_start (msg_area, true, true, 0);

		pack_start (hbox, false, false, 0);

		var action_area = new ButtonBox (Orientation.HORIZONTAL);
		action_area.set_layout (ButtonBoxStyle.SPREAD);

		image = new Image.from_icon_name ("call-start-symbolic",
										  IconSize.BUTTON);
		var button = new Button ();
		button.set_image (image);
		button.clicked.connect (() => {
				view.accept ();
			});
		action_area.pack_end (button, false, true, 0);

		image = new Image.from_icon_name ("call-end-symbolic",
										  IconSize.BUTTON);
		button = new Button ();
		button.set_image (image);
		button.clicked.connect (() => {
				view.reject ();
			});
		action_area.pack_end (button, false, true, 0);

		action_area.set_spacing (10);
		action_area.set_border_width (5);

		pack_start (action_area, false, true, 0);

		show_all ();
	}

	public void set_location (string remote) {
		msg.set_label (remote);
	}

	public IncomingCall (View view) {
		Object (view: view);
	}
}

}
