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
			registrar.deactivate ();
		}
	}
}

public class Registrar : Object {
	public Registrar () {
		aor = null;
		active = true;
		ttl = 300;
		compatibility = SIPRegisterCompatibilityModes.FULLY_COMPLIANT;
		// we use this status code to identify an un-initialized
		// registrar record
		status = StatusCodes.ILLEGALSTATUSCODE;
	}

	public Registrar.with_data (string _user,
								string _domain,
								string _pwd,
								string? _contact,
								string? _authId,
								string? _proxy,
								int _ttl = 300,
								SIPRegisterCompatibilityModes _compatibility = SIPRegisterCompatibilityModes.FULLY_COMPLIANT,
								bool _active = true) {
		user = _user;
		domain = _domain;
		contact = _contact;
		auth_ID = _authId;
		password = _pwd;
		ttl = _ttl;
		active = _active;
		proxy = _proxy;
		compatibility = _compatibility;

		status = StatusCodes.ILLEGALSTATUSCODE;
		aor = null;
	}

	public bool read (KeyFile config, string group) {
		try {
			active = config.get_boolean (group, "RegistrarUsed");
			user = config.get_string (group, "RegistrarUsername");
			domain = config.get_string (group, "RegistrarDomain");
		} catch {
			return false;
		}

		try { contact = config.get_string (group, "RegistrarContact"); }
		catch { contact = null; }
		try { auth_ID = config.get_string (group, "RegistrarAuthID"); }
		catch { auth_ID = null; }
		try { password = config.get_string (group, "RegistrarPassword"); }
		catch { password = null; }
		try { proxy = config.get_string (group, "RegistrarProxy"); }
		catch { proxy = null; }
		try { ttl = config.get_integer (group, "RegistrarTimeToLive"); }
		catch { ttl = 300; }
		try { compatibility = (SIPRegisterCompatibilityModes) config.get_integer (group, "RegistrarCompatibility"); }
		catch { compatibility = SIPRegisterCompatibilityModes.FULLY_COMPLIANT; }

		return true;
	}

	public void write (KeyFile config, string group) {
		config.set_boolean (group, "RegistrarUsed", active);
		config.set_string (group, "RegistrarUsername", user);
		config.set_string (group, "RegistrarDomain", domain);

		if (contact != null)
			config.set_string (group, "RegistrarContact", contact);
		if (auth_ID != null)
			config.set_string (group, "RegistrarAuthID", auth_ID);
		if (password != null)
			config.set_string (group, "RegistrarPassword", password);
		if (proxy != null)
			config.set_string (group, "RegistrarProxy", proxy);
		config.set_integer (group, "RegistrarTimeToLive", ttl);
		config.set_integer (group, "RegistrarCompatibility", (int) compatibility);
	}

	public bool start (SIPEP sipep) {
		if (!active)
			return true;

		if (!sipep.is_registered (aor, true)) {
			var srp = SIPRegisterParams () {
				compatibility = compatibility
			};

			srp.params.address_of_record = user;
			srp.params.remote_address = domain;
			srp.params.auth_ID = auth_ID;
			srp.params.password = password;
			srp.params.expire = ttl;
			srp.params.restore = 30;
			srp.params.min_retry = { 0, -1 }; // default max interval
			srp.params.max_retry = { 0, -1 }; // default max interval

			string _aor;
			bool ret = sipep.register (srp, out _aor, null);
			aor = _aor;

			return ret;
		}

		return true;
	}

	public bool stop (SIPEP sipep) {
		if (!active || aor == null)
			return false;

		sipep.unregister (aor);
		aor = null;

		return true;
	}

	public void deactivate (SIPEP? sipep = null) {
		if (sipep != null)
			stop(sipep);
		active = false;
	}

	public bool active   { get; private set; }
	public string user   { get; private set; }
	public string domain { get; private set; }
	public string contact { get; private set; }
	public string auth_ID { get; private set; }
	public string password { get; private set; }
	public int ttl { get; private set; }
	public string proxy { get; private set; }
	public SIPRegisterCompatibilityModes compatibility { get; private set; }

	public string aor { get; private set; }
	public StatusCodes status { get; set; }
}

}
