using Gopal;

namespace GPhone {

public class Controller : GLib.Object {
	private Gopal.Manager manager = new Gopal.Manager ();
	private Gopal.SIPEP sipep;
	private Gopal.PCSSEP pcssep;
	private GLib.KeyFile config = new GLib.KeyFile ();
	private GLib.List<Account> accounts;
	private string call_token;

	public Controller () {
		call_token = null;
		pcssep = manager.pcss_endpoint;
		sipep = manager.sip_endpoint;
		sipep.registration_status.connect (on_registration_status);
		manager.call_established.connect (on_call_established);
		manager.call_cleared.connect (on_call_cleared);
	}

	~Controller () {
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
		accounts = load_accounts ();
		start_accounts ();

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

	private GLib.List<Account> load_accounts () {
		var accounts = new GLib.List<Account> ();
		string[] groups = config.get_groups();

		foreach (string group in groups) {
			if (group.has_prefix ("SIP/Registrars/")) {
				var account = new Account ();
				account.read (config, group);
				accounts.append (account);
			}
		}

		return accounts;
	}

	public void start_accounts () {
		foreach (Account account in accounts) {
			if (!account.start (sipep))
				warning ("Could not register on %s", account.aor);
		}
	}

	public void stop_accounts () {
		foreach (Account account in accounts) {
			account.stop (sipep);
		}
	}

	private void on_registration_status (string aor,
										 bool registering,
										 Gopal.StatusCodes status) {
		message ("Got %sregistration status from %s: %d", registering ? "" : "un",
				 aor, status);

		if (!registering)
			start_accounts ();
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

}
