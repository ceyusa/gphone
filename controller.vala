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
				show_error (_("Gopal Failure"), "Cannot initialisate Gopal");
			}

			set_called_parties ();

			view.set_default_icon_name ("phone-symbolic");
			view.set_application (this);
			view.show_all ();
		}
	}

	private void show_error (string summary, string? body) {
		var s = _("GPhone") + ": " + summary;
		var notify = new Notify.Notification (s, body, "phone-symbolic");
		notify.set_hint ("transient", new Variant.boolean(true));
		try {
			notify.show ();
		} catch {
			message ("%s: %s", summary, body);
		}
	}

	private void map_signals () {
		view.quit.connect (() => {
				if (model.is_call_established ())
					model.hangup_call ();
				quit ();
			});

		view.call.connect ((remote) => {
				remote_party = remote;
				if (!model.make_call (remote)) {
					var msg = _("Unable to call to %s").printf (remote);
					show_error(_("Call failed"), msg);
				} else {
					call_token = model.call_token;
					history.mark (call_token, remote, History.Direction.OUT);
					sounds.play (Sounds.Type.OUTGOING);
				}
			});

		view.hangup.connect (() => {
				if (!model.hangup_call ()) {
					show_error(_("Hang up failed"), null);
				}
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

		model.network_started.connect (() => {
				if (remote_party != null) {
					Idle.add (() => {
							view.set_remote_party (remote_party);
							return false;
						});
				}
			});
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
			var msg = "%s: %s".printf
			(remote_party, _(Gopal.Manager.get_end_reason_string (reason)));
			show_error (_("Call failed"), msg);
		} else {
			set_called_parties ();
		}

		remote_party = null;
	}

	private bool network_is_available () {
		var netmon = NetworkMonitor.get_default ();
		return netmon.get_network_available ();
	}

	public bool init (string? config_file) {
		if (!config.load (config_file)) {
			show_error (_("Gopal Failure"), "Cannot get configuration");
			return false;
		}

		if (!network_is_available ()) {
			show_error (_("No network"), "Network not available");
			return false;
		}

		return true;
	}

	private void set_called_parties () {
		view.set_called_parties (history.get_called_parties ());
	}
}

}
