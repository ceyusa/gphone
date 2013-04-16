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
		add_buttons (Gtk.Stock.ADD, 1,
					 Gtk.Stock.DELETE, 2,
					 Gtk.Stock.CLOSE, Gtk.ResponseType.CLOSE);

		show_all ();
	}

	public RegistrarsDlg (Registrars? regs) {
		registrars = regs;
	}
}

}
