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

public class Controller :  Gtk.Application {
	private Registrars registrars;
	private Model model;
	private View view;
	private Sounds sounds;
	private History history;
	private Config config;

	// this is used only when a remote party is assigned
	// through the command line -- Nasty!
	public string remote_party { set; get; default = null; }
	public bool no_login { set; get; default = false; }

	private string call_token = null;
	private string config_file = null;

	public Controller () {
		Object (application_id: "org.gnome.GPhone",
				flags: ApplicationFlags.FLAGS_NONE);

		config = new Config ();
	}

	public override void activate () {
		unowned List<weak Gtk.Window> list = get_windows ();

		if (list != null) {
			view.present ();
		} else {
			registrars = new Registrars (config);
			model = new Model (config, registrars);
			view = new View ();
			sounds = new Sounds ();
			history = new History ();

			map_signals ();

			if (!model.init ()) {
				View.display_notification (_("Gopal Failure"),
										   "Cannot initialisate Gopal");
			}

			if (network_is_available ())
				model.start_networking ();

			view.set_called_parties (history.get_called_parties ());
			view.set_icon_name ("phone-symbolic");
			view.set_application (this);
			view.show_all ();
		}
	}

	private void map_signals () {
		view.quit.connect (() => {
				if (model.is_call_established ())
					model.hangup_call ();
				quit ();
			});

		view.call.connect (on_call_request);

		view.hangup.connect (() => {
				if (!model.hangup_call ()) {
					View.display_notification (_("Hang up failed"), null);
				}
			});

		view.input_tone.connect ((tone) => {
				debug ("tone code: %s", tone);
				model.send_input_tone (tone);
			});

		view.accept.connect (() => {
				sounds.stop ();
				model.accept_incoming_call (call_token);
				view.set_ui_state (View.State.CALLING);
			});

		view.reject.connect (() => {
				sounds.stop ();
				history.commit (call_token,
								Gopal.CallEndReason.NOACCEPT);
				model.reject_incoming_call (call_token,
											Gopal.CallEndReason.NOACCEPT);
				call_token = null;
				view.set_ui_state (View.State.IDLE);
			});

		view.handle_registrars.connect (() => {
				var regs = new RegistrarsDlg (registrars);
				int result = regs.run ();
				if (result != Gtk.ResponseType.CLOSE)
					warning ("unexpected dialog result");
				regs.destroy ();
			});

		model.stun_error.connect ((nat_type) => {
				Idle.add (() => {
						View.display_notification (
							_("STUN error"),
							_("STUN client did not find a NAT compatible configuration."));
						return false;
					});
			});

		model.call_established.connect (() => {
				Idle.add (() => {
						on_call_established ();
						return false;
					});
			});

		model.call_hungup.connect ((remote, reason) => {
				Idle.add (() => {
						on_call_hungup (remote, reason);
						return false;
					});
			});

		model.call_incoming.connect ((token, name, address) => {
				Idle.add (() => {
						on_call_incoming (token, name, address);
						return false;
					});
			});

		model.network_started.connect (() => {
				Idle.add (() => {
						if (!no_login)
							model.start_registrars ();
						view.set_ui_state (View.State.IDLE);
						if (remote_party != null) {
							view.set_remote_party (remote_party);
						}
						return false;
					});
			});

		var netmon = NetworkMonitor.get_default ();
		netmon.network_changed.connect ((available) => {
				if (network_is_available ()) {
					debug ("available");
					model.start_networking ();
				} else {
					debug ("not available");
					model.stop_networking ();
				}
			});
	}

	private void on_call_request (string remote) {
		remote_party = remote;

		if (!model.make_call (remote)) {
			var msg = _("Unable to call to %s").printf (remote);
			View.display_notification (_("Call failed"), msg);
			remote_party = null;
		} else {
			call_token = model.call_token;
			history.mark (call_token, remote, History.Direction.OUT);
			view.set_ui_state (View.State.ALERTING);
		}
	}

	private void on_call_established () {
		sounds.stop ();
		view.set_ui_state (View.State.CALLING);
	}

	private void on_call_hungup (string remote, Gopal.CallEndReason reason) {
		sounds.stop ();
		view.set_ui_state (View.State.IDLE);
		history.commit (call_token, reason);

		if (reason != Gopal.CallEndReason.LOCALUSER &&
			reason != Gopal.CallEndReason.REMOTEUSER) {
			sounds.play (Sounds.Type.HANGUP);
			var why = _(Gopal.Manager.get_end_reason_string (reason));
			var msg = "%s: %s".printf (remote, why);
			View.display_notification (_("Call failed"), msg);
		} else {
			view.set_called_parties (history.get_called_parties ());
		}

		call_token = null;
		remote_party = null;
	}

	private void on_call_incoming (string token, string name, string address) {
		history.mark (token, address, History.Direction.IN);

		if (call_token == null) {
			call_token = token;
			sounds.play (Sounds.Type.INCOMING);
			view.set_ui_state (View.State.RINGING);
			view.show_incoming_call (name, address);
		} else {
			// @TODO: handle on hold
			model.reject_incoming_call (token, Gopal.CallEndReason.LOCALBUSY);
			history.commit (call_token, Gopal.CallEndReason.LOCALBUSY);

			var why = _(Gopal.Manager.get_end_reason_string (Gopal.CallEndReason.LOCALBUSY));
			var msg = "%s: %s".printf (address, why);
			View.display_notification (_("Rejected call"), msg);
		}
	}

	private bool network_is_available () {
		var netmon = NetworkMonitor.get_default ();
		return netmon.get_network_available ();
	}

	public void init (string? config_file) {
		this.config_file = config_file;

		if (!config.load (config_file))
			View.display_notification (_("Gopal Failure"), "Cannot get configuration");

		if (!network_is_available ())
			View.display_notification (_("No network"), "Network not available");
	}
}

}
