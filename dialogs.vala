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
		Type[] types = {
			typeof (string),    // domain - sort key
			typeof (Object),    // registrar object
			typeof (string),    // aor
			typeof (bool),      // visible icon
			typeof (uint),      // pulse
			typeof (bool),      // pulse active
			typeof (string),    // icon name
		};

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
				 2, "%s@%s".printf (registrar.user, registrar.domain),
				 3, false,
				 4, 0,
				 5, false,
				 6, null);
		}
	}
}

private class RegistrarsDlg : Dialog {
	public Registrars registrars { construct set; private get; }
	private RegistrarsModel model;
	private uint timeout_handler = 0;
	private Builder builder;

	construct {
		builder = new Builder ();
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
		renderer = new CellRendererSpinner ();
		column.pack_start (renderer, false);
		renderer.size = IconSize.SMALL_TOOLBAR;
		column.set_attributes (renderer, "pulse", 4, "active", 5);

		renderer = new CellRendererText ();
		column.pack_start (renderer, false);
		renderer.ellipsize = Pango.EllipsizeMode.END;
		renderer.ellipsize_set = true;
		renderer.width_chars = 25;
		column.set_attributes (renderer, "markup", 2);

		renderer = new CellRendererPixbuf ();
		column.pack_start (renderer, false);
		renderer.stock_size = IconSize.SMALL_TOOLBAR;
		column.set_attributes (renderer, "visible", 3, "icon-name", 6);

		if (timeout_handler == 0)
			timeout_handler = Timeout.add(80, spinner_timeout);

		TreeIter iter;
		if (model.get_iter_first (out iter) == true)
			treeview.get_selection ().select_iter (iter);

		destroy.connect (() => {
				if (timeout_handler != 0) {
					Source.remove (timeout_handler);
					timeout_handler = 0;
				}
			});

		treeview.get_selection ().changed.connect ((selection) => {
				TreeIter it;

				if (selection.get_selected (null, out it)) {
					Registrar registrar;

					model.get (it, 1, out registrar);
					show_page_account (registrar);
				} else {
					show_page_nothing_selected ();
				}
			});

		content.show_all ();
		show_all ();
	}

	private void show_page (int page) {
		var notebook = builder.get_object ("registrars-notebook") as Notebook;
		notebook.set_current_page (page);
	}

	private void show_page_account (Registrar registrar) {
		show_page (1);

		var active_switch = builder.get_object ("registrar-active") as Switch;
		active_switch.active = registrar.active;

		if (registrar.user != null || registrar.domain != null) {
			var username = builder.get_object ("registrar-userid") as Entry;
			username.text = registrar.user + "@" + registrar.domain;
		}

		var password = builder.get_object ("registrar-password") as Entry;
		password.visibility = false;
		password.set_icon_from_stock (EntryIconPosition.SECONDARY, Stock.CLEAR);
		password.set_icon_sensitive (EntryIconPosition.SECONDARY,
									 registrar.password != null);
		if (registrar.password != null) {
			password.text = registrar.password;
		}

		if (registrar.proxy != null) {
			var proxy_host = builder.get_object ("registrar-proxy") as Entry;
			proxy_host.text = registrar.proxy;
		}

		if (registrar.contact != null) {
			var contact = builder.get_object ("registrar-contact") as Entry;
			contact.text = registrar.contact;
		}

		if (registrar.auth_ID != null) {
			var auth_id = builder.get_object ("registrar-authid") as Entry;
			auth_id.text = registrar.auth_ID;
		}

		var ttl = builder.get_object ("registrar-ttl") as SpinButton;
		ttl.value = registrar.ttl;
	}

	private void show_page_nothing_selected () {
		show_page (0);
	}

	private bool spinner_timeout () {
		TreeIter iter;
		bool valid;
		Object object;
		bool registering;
		uint pulse;
		string icon = null;
		bool ret = false;

		var netmon = NetworkMonitor.get_default ();
		if (!netmon.get_network_available ()) {
			for (valid = model.get_iter_first (out iter);
				 valid;
				 valid = model.iter_next (ref iter)) {
				model.set (iter,
						   3, true,
						   6, "network-offline-symbolic");
			}
			return false;
		}

		for (valid = model.get_iter_first (out iter);
			 valid;
			 valid = model.iter_next (ref iter)) {
			model.get (iter,
					   1, out object,
					   4, out pulse);

			var registrar = object as Registrar;
			if (registrar.active == false) {
				registering = false;
				pulse = 0;
				icon = "user-offline-symbolic";
			} else if (registrar.status == Gopal.StatusCodes.ILLEGALSTATUSCODE) {
				registering = true;
				icon = null;
				if (pulse == uint.MAX)
					pulse = 0;
				else
					pulse++;
				ret = true;
			} else if (registrar.status == Gopal.StatusCodes.SUCCESS_OK) {
				registering = false;
				icon = "emblem-ok-symbolic";
				pulse = 0;
			} else {
				registering = false;
				icon = "dialog-warning-symbolic";
				pulse = 0;
			}

			model.set (iter,
					   3, icon != null,
					   4, pulse,
					   5, registering,
					   6, icon);
		}

		if (ret == false)
			timeout_handler = 0;

		return ret;
	}

	public RegistrarsDlg (Registrars? registrars) {
		Object (registrars: registrars);
	}
}

}
