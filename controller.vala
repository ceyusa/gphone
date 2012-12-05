namespace GPhone {

private class Controller : Object {
	private Model model;
	private View view;
	private MainLoop loop;

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

	public bool init (string config_file) {
		if (!model.initialisate (config_file)) {
			warning ("Falied to initialisate gphone.");
			return false;
		}

		return true;
	}

	public void run (string? remote_party) {
		if (remote_party != null) {
			view.set_remote_party (remote_party);
		}

		view.show ();
		loop.run ();
	}

	public void quit () {
		message ("\nQuitting...\n");
		loop.quit ();
	}

	public bool is_running () {
		return loop.is_running ();
	}
}

}