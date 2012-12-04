using Gopal;

namespace GPhone {

private class Account {
	public Account () {
		aor = null;
		active = true;
		ttl = 300;
		compatibility = Gopal.SIPRegisterCompatibilityModes.FULLY_COMPLIANT;
	}

	public bool read (GLib.KeyFile config, string group) {
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
		try { compatibility = (Gopal.SIPRegisterCompatibilityModes) config.get_integer (group, "RegistrarCompatibility"); }
		catch { compatibility = Gopal.SIPRegisterCompatibilityModes.FULLY_COMPLIANT; }

		return true;
	}

	public bool start (Gopal.SIPEP sipep) {
		if (!active)
			return false;

		if (!sipep.is_registered (aor, true)) {
			var srp = Gopal.SIPRegisterParams () {
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

	public bool stop (Gopal.SIPEP sipep) {
		if (!active || aor == null)
			return false;

		sipep.unregister (aor);
		aor = null;

		return true;
	}

	private bool active;
	private string user;
	private string domain;
	private string contact;
	private string auth_ID;
	private string password;
	private int ttl;
	private string proxy;
	private Gopal.SIPRegisterCompatibilityModes compatibility;

	public string aor { get; private set; }
}

}
