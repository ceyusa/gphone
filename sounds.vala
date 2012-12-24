/*
 * Copyright (C) 2012 Igalia S.L.
 *
 * Author: Víctor Manuel Jáquez Leal <vjaquez@igalia.com>
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1, a copy of which is found in LICENSE included in the
 * packaging of this file.
 */

using Canberra;

namespace GPhone {

private class Sounds : Object {
	private Context ctxt = null;
	private uint32 curid = 0;
	private uint srcid = 0;

	public Sounds () {
		var err = Context.create (out ctxt);

		if (err != 0) {
			warning ("cannot create context: %s", Canberra.strerror (err));
			return;
		}

		err = ctxt.change_props (PROP_APPLICATION_NAME, "GPhone", null);

		if (err != SUCCESS)
			warning ("cannot set props: %s", Canberra.strerror (err));
	}

	private void cb (Context c, uint32 id, int code) {
		debug ("callback: id %u, error '%s'\n", id, Canberra.strerror (code));

		if (code == SUCCESS)
			srcid = Idle.add (play_again);
	}

	private bool play_again () {
		if (curid == 1)
			play_outgoing_calling ();

		return true;
	}

	private bool play (uint32 id, Proplist p) {
		return_val_if_fail (ctxt != null, false);

		if (curid > 0)
			return false;

		curid = id;
		var err = ctxt.play_full (id, p, cb);

		if (err != SUCCESS)
			warning ("cannot play sound: %s", Canberra.strerror (err));

		return err == SUCCESS;
	}

	private bool cancel () {
		return_val_if_fail (ctxt != null, false);

		if (curid == 0)
			return true;

		var err = ctxt.cancel (curid);

		if (err != SUCCESS)
			warning ("cannot cancel sound: %s", Canberra.strerror (err));

		curid = 0;

		return err == SUCCESS;
	}

	public bool play_outgoing_calling () {
		Proplist p;
		Proplist.create (out p);

		p.sets (PROP_MEDIA_FILENAME, "/usr/share/sounds/ekiga/dialtone.wav");
		p.sets (PROP_CANBERRA_CACHE_CONTROL, "permanent");

		return play (1, p);
	}

	public bool stop () {
		if (srcid > 0) {
			Source.remove (srcid);
			srcid = 0;
		}
		return cancel ();
	}
}

}
