/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

using Gopal;

namespace GPhone {

public class Registrars {
	private List<Registrar> accs;

	public Registrars(Config cfg) {
		accs = cfg.get_registrars ();
	}

	public void write(Config cfg) {
		cfg.set_registrars (accs);
	}

	public uint size {
		get { return accs.length (); }
	}

	public Registrar get (int index) {
		assert (index >= 0 && index < size);
		return accs.nth_data (index);
	}

	public Registrar? find (string aor) {
		foreach (Registrar registrar in accs) {
			if (registrar.aor == aor)
				return registrar;
		}
		return null;
	}

	public void set_status (string aor, StatusCodes status) {
		Registrar reg = find (aor);
		if (reg != null)
			reg.status = status;
	}

	public void deactivate_all () {
		foreach (Registrar registrar in accs) {
			registrar.active = false;
		}
	}
}

}
