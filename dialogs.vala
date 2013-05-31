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

private class RegistrarsModel : ListStore {
	public Registrars registrars { construct; get;  }

	public RegistrarsModel (Registrars registrars) {
		Object (registrars: registrars);
	}

	public override void constructed () {
		Type[] types = { typeof (string),
						 typeof (Object),
						 typeof (bool),
						 typeof (string),
						 typeof (Icon) };

		set_column_types (types);
		set_sort_column_id (0, SortType.ASCENDING);

		init_model ();

		base.constructed ();
	}

	private void init_model () {
		foreach (Registrar registrar in registrars) {
			TreeIter iter;
			append (out iter);
			set (iter,
				 0, registrar.domain,
				 1, registrar as Object,
				 2, registrar.active,
				 3, "%s@%s".printf (registrar.user, registrar.domain),
				 4, null);
		}
	}
}

private class RegistrarsDlg : Dialog {
	public Registrars registrars { construct set; private get; }
	private RegistrarsModel model;

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

		var treeview = builder.get_object("registrars-tree-treeview") as TreeView;
		model = new RegistrarsModel (registrars);
		treeview.set_model (model);

		var column = new TreeViewColumn ();
		treeview.append_column (column);

		dynamic CellRenderer renderer;
		renderer = new CellRendererPixbuf ();
		column.pack_start (renderer, false);
		renderer.follow_state = true;
		renderer.stock_size = IconSize.DIALOG;
		column.set_attributes (renderer, "gicon", 4);

		renderer = new CellRendererText ();
		column.pack_start (renderer, false);
		renderer.ellipsize = Pango.EllipsizeMode.END;
		renderer.ellipsize_set = true;
		renderer.width_chars = 30;
		column.set_attributes (renderer, "markup", 3);

		renderer = new CellRendererPixbuf ();
		column.pack_start (renderer, false);
		renderer.icon_name = "dialog-warning-symbolic";
		column.set_attributes (renderer, "visible", 2);

		TreeIter iter;
		if (model.get_iter_first (out iter) == true)
			treeview.get_selection ().select_iter (iter);

		content.show_all();
		show_all ();
	}

	public RegistrarsDlg (Registrars? registrars) {
		Object (registrars: registrars);
	}
}

}
