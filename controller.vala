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

private class Controller : Object {
	private Model model;
	private View view;
	private MainLoop loop;
	private Sounds sounds;
	private History history;

	// this is used only when a remote party is assigned
	// through the command line -- Nasty!
	public string remote_party { set; get; default = null; }
	private string call_token = null;

	public Controller () {
		model = new Model ();
		view = new View ();
		sounds = new Sounds ();
		history = new History ();
		loop = new MainLoop ();

		map_signals ();
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

		view.do_action.connect ((remote) => {
				if (!model.is_call_established ()) {
					if (!model.make_call (remote)) {
						var msg = _("Unable to call to") + " " + remote;
						show_error(_("Call failed"), msg);
					} else {
						call_token = model.call_token;
						history.mark (call_token, remote, History.Direction.OUT);
						sounds.play (Sounds.Type.OUTGOING);
					}
				} else {
					if (!model.hangup_call ()) {
						show_error(_("Hang up failed"), null);
					}
				}
			});

		model.call_established.connect (() => {
				sounds.stop ();
				view.set_ui_state (View.State.CALLING);
			});

		model.call_hungup.connect ((remote, reason) => {
				sounds.stop ();
				view.set_ui_state (View.State.IDLE);
				history.commit (call_token, reason);
				if (reason != Gopal.CallEndReason.LOCALUSER) {
					sounds.play (Sounds.Type.HANGUP);
					var message = "%s: %s".printf
					(remote, _(Gopal.Manager.get_end_reason_string (reason)));
					show_error (_("Call failed"), message);
				}
			});

		model.network_started.connect (() => {
				if (remote_party != null)
					Idle.add (call);
			});
	}

	private bool network_is_available () {
		var netmon = NetworkMonitor.get_default ();
		return netmon.get_network_available ();
	}

	public bool init (string config_file) {
		if (network_is_available ()) {
			if (!model.initialisate (config_file)) {
				show_error (_("Gopal Failure"), "Cannot initialisate Gopal");
				return false;
			}
		} else {
			show_error (_("No network"), "Network not available");
			return false;
		}

		return true;
	}

	private bool call () {
		view.set_remote_party (remote_party);
		remote_party = null;
		return false;
	}

	public void run () {
		view.show_all ();
		loop.run ();
	}

	public void quit () {
		message ("Quitting...");
		loop.quit ();
	}

	public bool is_running () {
		return loop.is_running ();
	}
}

}
