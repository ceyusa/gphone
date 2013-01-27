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

private class Controller :  Gtk.Application {
	private Model model;
	private View view;
	private Sounds sounds;
	private History history;
	private Config config;

	// this is used only when a remote party is assigned
	// through the command line -- Nasty!
	public string remote_party { set; get; default = null; }
	private string call_token = null;

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
			model = new Model ();
			view = new View ();
			sounds = new Sounds ();
			history = new History ();

			map_signals ();

			model.config = config;
			if (!model.init ()) {
				view.display_notification (_("Gopal Failure"),
										   "Cannot initialisate Gopal");
			}

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
					view.display_notification (_("Hang up failed"), null);
				}
			});

		view.input_tone.connect ((tone) => {
				debug ("tone code: %s", tone);
				model.send_input_tone (tone);
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
						view.set_ui_state (View.State.IDLE);
						if (remote_party != null) {
							view.set_remote_party (remote_party);
						}
						return false;
					});
			});
	}

	private void on_call_request (string remote) {
		remote_party = remote;

		if (!model.make_call (remote)) {
			var msg = _("Unable to call to %s").printf (remote);
			view.display_notification (_("Call failed"), msg);
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
			view.display_notification (_("Call failed"), msg);
		} else {
			view.set_called_parties (history.get_called_parties ());
		}

		remote_party = null;
	}

	private void on_call_incoming (string token, string name, string address) {
		sounds.play (Sounds.Type.INCOMING);
		// view.incoming_dialog (remote);
		view.set_ui_state (View.State.ALERTING);
	}

	private bool network_is_available () {
		var netmon = NetworkMonitor.get_default ();
		return netmon.get_network_available ();
	}

	public bool init (string? config_file) {
		if (!config.load (config_file)) {
			view.display_notification (_("Gopal Failure"),
									   "Cannot get configuration");
			return false;
		}

		if (!network_is_available ()) {
			view.display_notification (_("No network"), "Network not available");
			return false;
		}

		return true;
	}
}

}
