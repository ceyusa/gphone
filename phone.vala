using Gopal;
using Posix;
using Gst;

namespace GPhone {

GLib.MainLoop loop;

private class RegistrationInfo : GLib.Object {
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

public class Phone : GLib.Object {
	private Gopal.Manager manager = new Gopal.Manager ();
	private Gopal.SIPEP sipep;
	private Gopal.PCSSEP pcssep;
	private GLib.KeyFile config = new GLib.KeyFile ();
	private GLib.List<RegistrationInfo> registrations;
	private string call_token;

	public Phone () {
		call_token = null;
		pcssep = manager.pcss_endpoint;
		sipep = manager.sip_endpoint;
		sipep.registration_status.connect (on_registration_status);
		manager.call_established.connect (on_call_established);
		manager.call_cleared.connect (on_call_cleared);
	}

	~Phone () {
		manager.shutdown_endpoints ();
	}

	public bool initialisate () {
		if (!load_config ())
			return false;

		if (!pcssep.set_soundchannel_play_device ("PulseAudio") ||
			!pcssep.set_soundchannel_record_device ("PulseAudio"))
			return false;

		pcssep.set_soundchannel_buffer_time (40);

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

		if (stun_server != null) {
			var nat_type = manager.set_stun_server (stun_server);
			debug ("NAT type %d %s\n", nat_type, manager.get_stun_server ());
		}
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
			if (!registration.start (sipep))
				warning ("Could not register on %s", registration.aor);
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

	public bool make_call (string remote_party) {
		string token;
		bool ret;

		if (call_token == null) {
			ret = manager.setup_call (null, remote_party, out token, 0, null);
			if (ret)
				call_token = token;

			return ret;
		}

		return false;
	}

	public void on_call_established (string? token) {
		GLib.assert (call_token == token);
		call_established ();
	}

	public void on_call_cleared (string? token) {
		if (call_token == null)
			return; // ignore this: perhaps the call setup failed

		GLib.assert (call_token == token);
		call_token = null;
		call_hungup ();
	}

	public bool is_call_established () {
		if (call_token == null)
			return false;

		return manager.is_call_established (call_token);
	}

	public bool hangup_call () {
		return manager.clear_call (call_token, CallEndReason.LOCALUSER);
	}

	public signal void call_established ();
	public signal void call_hungup ();
}

public void signal_handler (int signal) {
	if (loop.is_running ()) {
		print ("\nQuitting...\n");
		loop.quit ();
	}
}

void run_ui (Phone phone) {
	var win = new PhoneWindow ();
	win.quit.connect (() => {
			if (phone.is_call_established ())
				phone.hangup_call ();
			signal_handler (0);
		});
	win.show ();

	win.do_action.connect ((remote) => {
			if (!phone.is_call_established ()) {
				message ("calling %s\n", remote);
				if (!phone.make_call (remote))
					warning ("call failed!\n");
			} else {
				message ("hanging up\n");
				if (!phone.hangup_call ())
					warning ("hangup failed!\n");
			}
		});

	phone.call_established.connect (() => {
			win.toggle_state ();
		});

	phone.call_hungup.connect (() => {
			win.toggle_state ();
		});
}

int main (string[] args) {
	var options = new OptionContext ("");
	options.add_group (Gtk.get_option_group (true));
	options.add_group (Gst.init_get_option_group ());
	options.add_group (Gopal.init_get_option_group ());
	try {
		options.parse (ref args);
	} catch (OptionError error) {
		print ("Option parsing failed: %s\n", error.message);
		return -1;
	}

	try {
		Gopal.init_check (ref args);
		Gst.init_check (ref args);
		Gtk.init_check (ref args);
	} catch (Error error) {
		print ("Failed to init: %s", error.message);
		return -1;
	}

	Posix.signal (Posix.SIGABRT, signal_handler);
	Posix.signal (Posix.SIGINT, signal_handler);
	Posix.signal (Posix.SIGTERM, signal_handler);

	loop = new GLib.MainLoop ();
	var phone = new Phone ();

	if (!phone.initialisate ()) {
		warning ("falied to initialisate gphone, bye...");
	} else {
		run_ui (phone);
		loop.run ();
	}

	phone = null;

	Gopal.deinit ();

	return 0;
}

}
