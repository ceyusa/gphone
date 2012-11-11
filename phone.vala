using Gopal;

private class RegistrationInfo : Object {
	public RegistrationInfo () {
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
		var srp = Gopal.SIPRegisterParams ();

		if (active) {
			srp.params.address_of_record = user;
			srp.params.remote_address = domain;
			srp.params.auth_ID = auth_ID;
			srp.params.password = password;
			srp.params.expire = ttl;
			srp.compatibility = compatibility;

			return sipep.register (srp, out aor, null);
		}

		return true;
	}

	// @TODO bool stop (Gopal.SIPEP sipep);

	private bool active;
    private string user;
    private string domain;
    private string contact;
    private string auth_ID;
    private string password;
    private int ttl;
    private string proxy;
    private Gopal.SIPRegisterCompatibilityModes compatibility;

    private string aor;
}

public class Phone : Object {
	private Gopal.Manager manager = new Gopal.Manager ();
	private Gopal.SIPEP sipep;
	private GLib.KeyFile config = new GLib.KeyFile ();
	private GLib.List<RegistrationInfo> registrations;

	public Phone () {
		sipep = manager.sip_endpoint;
	}

	public bool initialisate () {
		if (!load_config ())
			return false;

		set_nat_handling ();
		start_all_listeners ();
		load_registrations ();
		start_registrations ();

		return true;
	}

	private bool load_config () {
		bool ret = false;

		string file = GLib.Environment.get_user_config_dir () + "/gphone.conf";

		try {
			ret = config.load_from_file (file, GLib.KeyFileFlags.NONE);
		} catch (GLib.Error err) {
			message ("cannot load config file (%s): %s", file, err.message);
		}

		return ret;
	}

	private void set_nat_handling () {
		string stun_server;
		try { stun_server = config.get_string ("General", "STUNServer"); }
		catch { stun_server = null; }

		var nat_type = manager.set_stun_server (stun_server);
		debug ("NAT type %d %s\n", nat_type, manager.get_stun_server ());
	}

	private void start_all_listeners () {
		if (sipep.start_listeners (null)) {
			debug ("SIP listening\n");
		} else {
			debug ("SIP listening failed\n");
		}
	}

	private void load_registrations () {
		string[] groups = config.get_groups();

		foreach (string group in groups) {
			if (group.has_prefix ("SIP/Registrars/")) {
				var registration = new RegistrationInfo ();
				registration.read (config, group);
				registrations.append (registration);
			}
		}
	}

	private void start_registrations () {
		foreach (RegistrationInfo registration in registrations) {
			registration.start (sipep);
		}
	}
}

int main (string[] args) {
	Gopal.init (ref args);

	var phone = new Phone ();
	if (!phone.initialisate ())
		warning ("falied to initialisate gphone, bye...");
	phone = null;

    Gopal.deinit ();

	return 0;
}
