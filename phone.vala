using Gopal;
using Posix;

GLib.MainLoop loop;

private class RegistrationInfo : Object {
	public RegistrationInfo () {
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
		if (active) {
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

			return sipep.register (srp, out aor, null);
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

    private string aor;
}

public class Phone : Object {
	private Gopal.Manager manager = new Gopal.Manager ();
	private Gopal.SIPEP sipep;
	private GLib.KeyFile config = new GLib.KeyFile ();
	private GLib.List<RegistrationInfo> registrations;

	public Phone () {
		sipep = manager.sip_endpoint;
		sipep.registration_status.connect (on_registration_status);
	}

	~Phone () {
		manager.shutdown_endpoints ();
	}

	public bool initialisate () {
		if (!load_config ())
			return false;

		set_nat_handling ();
		start_all_listeners ();
		registrations = load_registrations ();
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

	private GLib.List<RegistrationInfo> load_registrations () {
		var regs = new GLib.List<RegistrationInfo> ();
		string[] groups = config.get_groups();

		foreach (string group in groups) {
			if (group.has_prefix ("SIP/Registrars/")) {
				var registration = new RegistrationInfo ();
				registration.read (config, group);
				regs.append (registration);
			}
		}

		return regs;
	}

	public void start_registrations () {
		foreach (RegistrationInfo registration in registrations) {
			registration.start (sipep);
		}
	}

	public void stop_registrations () {
		foreach (RegistrationInfo registration in registrations) {
			registration.stop (sipep);
		}
	}

	private void on_registration_status (string aor,
										 bool registering,
										 Gopal.StatusCodes status) {
		message ("Got %sregistration status from %s: %d", registering ? "" : "un",
				 aor, status);

		if (!registering)
			start_registrations ();
	}
}

public void signal_handler (int signal) {
	print ("\nQuitting...\n");
	loop.quit ();
}

int main (string[] args) {
	loop = new GLib.MainLoop ();

	Gopal.init (ref args);

	Posix.signal (Posix.SIGABRT, signal_handler);
	Posix.signal (Posix.SIGINT, signal_handler);
	Posix.signal (Posix.SIGTERM, signal_handler);

	var phone = new Phone ();
	if (!phone.initialisate ()) {
		warning ("falied to initialisate gphone, bye...");
	} else {
		loop.run ();
	}

	phone = null;

    Gopal.deinit ();

	return 0;
}
