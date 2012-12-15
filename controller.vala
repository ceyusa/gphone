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

	// this is used only when a remote party is assigned
	// through the command line -- Nasty!
	private string remote_party;

	public Controller () {
		model = new Model ();
		view = new View ();
		loop = new MainLoop ();

		map_signals ();
	}

	private void show_error (string summary, string? body) {
		var notify = new Notify.Notification (summary, body, "phone-symbolic");
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
						show_error("call failed", remote);
					}
				} else {
					if (!model.hangup_call ()) {
						show_error("hang up failed", null);
					}
				}
			});

		model.call_established.connect (() => {
				view.set_ui_state (View.State.CALLING);
			});

		model.call_hungup.connect ((remote, reason) => {
				view.set_ui_state (View.State.IDLE);
				if (reason != Gopal.CallEndReason.LOCALUSER) {
					var message = "%s (%d)".printf (remote, reason);
					show_error ("call failed", message);
				}
			});
	}

	private bool network_is_available () {
		var netmon = NetworkMonitor.get_default ();
		return netmon.get_network_available ();
	}

	public bool init (string config_file) {
		if (network_is_available ()) {
			if (!model.initialisate (config_file)) {
				message ("Falied to initialisate gphone.");
				return false;
			}
		} else {
			return false;
		}

		return true;
	}

	private bool call () {
		view.set_remote_party (remote_party);
		remote_party = null;
		return false;
	}

	public void run (string? _remote_party) {
		if (_remote_party != null) {
			remote_party = _remote_party;
			Idle.add (call);
		}

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