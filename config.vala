/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

namespace GPhone {

public class Config : Object {
	private KeyFile config = new KeyFile ();
	private bool loaded = false;

	public string validate_file (string? path) {
		if (path != null)
			return path;

		return Path.build_filename (Environment.get_user_config_dir (),
									"gphone.conf");
	}

	public bool load (string? path) {
		string file = validate_file (path);

		try {
			loaded = config.load_from_file (file, KeyFileFlags.NONE);
		} catch (Error err) {
			warning ("cannot load config file (%s): %s", config_file, err.message);
		}

		return loaded;
	}

	public async bool save (string? path) {
		var file = File.new_for_path (validate_file (path));
		var backup = file.query_exists ();

		try {
			var ostrm = yield file.replace_async (null, backup, FileCreateFlags.PRIVATE);

			uint8[] data = config.to_data().data;
			ssize_t len = config.to_data().length;
			ssize_t written = 0;

			while (true) {
				written = yield ostrm.write_async (data);
				data = data[written:-1];
				len = len - written;
				if (written == len)
					break;
			}

			yield ostrm.close_async ();
		} catch (Error err) {
			warning ("Unable to replace config file: %s", err.message);
			return false;
		}

		return true;
	}

	public string get_string (string group, string key) {
		string value;

		try {
			value = config.get_string (group, key);
		} catch {
			value = null;
		}

		return value;
	}

	public List<Registrar> get_registrars () {
		var registrars = new List<Registrar> ();
		string[] groups = config.get_groups();

		foreach (string group in groups) {
			if (group.has_prefix ("SIP/Registrars/")) {
				var registrar = new Registrar ();
				registrar.read (config, group);
				registrars.append (registrar);
			}
		}

		return registrars;
	}

	public bool set_registrars (List<Registrar> registrars) {
		int i = 1;
		foreach (Registrar registrar in registrars) {
			registrar.write (config, "SIP/Registrars/%00004d".printf (i++));
		}

		return true;
	}

	public void dump () {
		print ("%s", config.to_data());
	}

}

}