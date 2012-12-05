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

	private void map_signals () {
		view.quit.connect (() => {
				if (model.is_call_established ())
					model.hangup_call ();
				quit ();
			});

		view.do_action.connect ((remote) => {
				if (!model.is_call_established ()) {
					message ("calling %s\n", remote);
					if (!model.make_call (remote))
						warning ("call failed!\n");
				} else {
					message ("hanging up\n");
					if (!model.hangup_call ())
						warning ("hangup failed!\n");
				}
			});

		model.call_established.connect (() => {
				view.toggle_state ();
			});

		model.call_hungup.connect (() => {
				view.toggle_state ();
			});
	}

	private bool network_is_available () {
		var netmon = NetworkMonitor.get_default ();
		return netmon.get_network_available ();
	}

	public bool init (string config_file) {
		if (network_is_available ()) {
			if (!model.initialisate (config_file)) {
				warning ("Falied to initialisate gphone.");
				return false;
			}
		} else {
			return false;
		}

		return true;
	}

	private bool call () {
		view.set_remote_party (remote_party);
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