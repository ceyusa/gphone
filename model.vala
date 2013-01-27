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

private class Model : Object {
	private Manager manager = new Manager ();
	private SIPEP sipep;
	private PCSSEP pcssep;
	private List<Account> accounts;
	public string call_token { get; private set; default = null; }
	public Config config { get; set; }

	public Model () {
		pcssep = manager.pcss_endpoint;
		sipep = manager.sip_endpoint;

		manager.set_product_info ("GPhone", "Igalia, S.L.", "0.1");

		pcssep.call_incoming.connect (on_call_incoming);

		sipep.registration_status.connect (on_registration_status);

		manager.call_established.connect (on_call_established);
		manager.call_cleared.connect (on_call_cleared);
	}

	~Model () {
		manager.shutdown_endpoints ();
	}

	public bool init () {
		if (!pcssep.set_soundchannel_play_device ("Gst") ||
			!pcssep.set_soundchannel_record_device ("Gst"))
			return false;

		pcssep.set_soundchannel_buffer_time (20);

		if (!manager.set_video_output_device ("NULL"))
			return false;

		accounts = config.get_accounts ();
		setup_networking ();

		return true;
	}

	private void setup_networking_cont () {
		start_all_listeners ();
		start_accounts ();

		manager.add_route_entry ("pc:.* = sip:<da>");
		manager.add_route_entry ("sip:.* = pc:");

		network_started ();
	}

	private void setup_networking () {
		string stun_server = config.get_string ("Networking", "STUNServer");

		if (stun_server != null) {
			manager.set_stun_server_async.begin (stun_server, null, (obj, res) => {
					var nat_type = manager.set_stun_server_async.end (res);
					debug ("NAT type %d %s\n", nat_type, manager.get_stun_server ());
					setup_networking_cont ();
				});
		} else {
			setup_networking_cont ();
		}
	}

	private void start_all_listeners () {
		if (sipep.start_listeners (null)) {
			debug ("SIP listening\n");
		} else {
			debug ("SIP listening failed\n");
		}
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
										 StatusCodes status) {
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
			if (ret) {
				call_token = token;
			}

			return ret;
		}

		return false;
	}

	private void on_call_established (string? token) {
		assert (call_token == token);
		call_established ();
	}

	private void on_call_cleared (string? token,
								  string party,
								  CallEndReason reason) {
		if (call_token == null)
			return; // ignore this: perhaps the call setup failed

		assert (call_token == token);
		call_token = null;
		call_hungup (party, reason);
	}

	public bool is_call_established () {
		if (call_token == null)
			return false;

		return manager.is_call_established (call_token);
	}

	public bool hangup_call () {
		return manager.clear_call (call_token, CallEndReason.LOCALUSER);
	}

	private void on_call_incoming (string token, string name, string address) {
		if (call_token == null) {
			call_incoming (token, name, address);
		}
	}

	public void send_input_tone (string tone) {
		if (call_token == null)
			return;

		manager.send_user_input_tone (call_token, tone[0]);
	}

	public void accept_incoming_call (string token) {
		call_token = token;
		if (!pcssep.accept_incoming_connection (token)) {
			critical ("Accept incoming connection failed!");
			call_token = null;
		}
	}

	public void reject_incoming_call (string token,
									  Gopal.CallEndReason reason = CallEndReason.NOACCEPT) {
		if (!pcssep.reject_incoming_connection (token, reason)) {
			critical ("Reject incoming connection failed!");
		}
	}

	public signal void call_incoming (string token, string name, string address);
	public signal void call_established ();
	public signal void call_hungup (string party, CallEndReason reason);
	public signal void network_started ();
}

}
