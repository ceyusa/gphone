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

namespace GPhone {

private class RegistrarsDlg : Dialog {
	private Registrars registrars;

	construct {
		var builder = new Builder ();
		try {
			builder.add_from_resource ("/org/gnome/gphone/registrars.ui");
		} catch (Error err) {
			warning ("unable to load registrars.ui: %s", err.message);
			return;
		}

		var content = builder.get_object("gphone-top-widget") as Box;
		return_val_if_fail (content != null, null);

		get_content_area ().add (content);
		add_buttons (Stock.CLOSE, ResponseType.CLOSE);

		content.show_all();
		show_all ();
	}

	public RegistrarsDlg (Registrars? regs) {
		registrars = regs;
	}
}

}
