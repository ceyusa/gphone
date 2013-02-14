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

	public enum Type {
		OUTGOING = 1,
		HANGUP,
		INCOMING
	}

	private struct Description {
		public uint32 id;
		public string eventid;
		public string description;
		public uint interval;
		public uint srcid;

		public Description (uint32 id, string eventid,
							string description, uint interval) {
			this.id = id;
			this.eventid = eventid;
			this.description = description;
			this.interval = interval;
			this.srcid = 0;
		}
	}

	private Description[] descriptions = {
		Description (Type.OUTGOING, "phone-outgoing-calling", _("Outgoing call"), 500),
		Description (Type.HANGUP, "phone-outgoing-busy", _("Call ended"), 0),
		Description (Type.INCOMING, "phone-incoming-call", _("Incoming call"), 500)
	};

	public Sounds () {
		var err = Context.create (out ctxt);

		if (err != 0) {
			warning ("cannot create context: %s", Canberra.strerror (err));
			return;
		}

		err = ctxt.change_props (PROP_APPLICATION_NAME, _("GPhone"),
								 PROP_CANBERRA_XDG_THEME_NAME, "freedesktop",
								 null);

		if (err != SUCCESS)
			warning ("cannot set props: %s", Canberra.strerror (err));

		cache_sounds ();
	}

	~Sounds () {
		stop ();
	}

	private void cache_sounds () {
		return_if_fail (ctxt != null);

		foreach (Description d in descriptions) {
			ctxt.cache (PROP_EVENT_ID, d.eventid,
						PROP_CANBERRA_CACHE_CONTROL, "permanent",
						null);
		}
	}

	private void cb (Context c, uint32 id, int code) {
		debug ("callback: id %u, error '%s'\n", id, Canberra.strerror (code));

		if (code == SUCCESS) {
			Type t = (Type) id;
			var d = get_description (t);
			if (d.interval > 0) {
				debug ("queuing effect %u", id);
				d.srcid = Timeout.add (d.interval, play_again);
			}
		} else {
			curid = 0;
		}
	}

	private bool play_again () {
		if (curid == 0) {
			warning ("FIXME: curid is zero. Why?");
			return false;
		}

		Type t = (Type) curid;
		var d = get_description (t);
		d.srcid = 0;
		curid = 0;
		play_internal (d);

		return false;
	}

	private bool play_internal (Description d) {
		return_val_if_fail (ctxt != null, false);

		if (curid > 0)
			return false;

		Proplist p;
		Proplist.create (out p);
		p.sets (PROP_EVENT_ID, d.eventid);
		p.sets (PROP_EVENT_DESCRIPTION, d.description);

		var err = ctxt.play_full (d.id, p, cb);
		if (err != SUCCESS) {
			warning ("cannot play sound: %s", Canberra.strerror (err));
		} else {
			curid = d.id;
		}

		return err == SUCCESS;
	}

	private bool cancel () {
		return_val_if_fail (ctxt != null, false);

		if (curid == 0)
			return true;

		var err = ctxt.cancel (curid);
		if (err != SUCCESS) {
			warning ("cannot cancel sound: %s", Canberra.strerror (err));
		} else {
			curid = 0;
		}

		return err == SUCCESS;
	}

	public bool stop () {
		foreach (Description d in descriptions) {
			if (d.srcid > 0) {
				Source.remove (d.srcid);
				d.srcid = 0;
			}
		}
		return cancel ();
	}

	public bool play (Type t) {
		debug ("effect type %d", t);
		return play_internal (get_description (t));
	}

	private Description get_description (Type t) {
		uint idx = (uint) t - 1;
		return descriptions[idx];
	}
}

}
