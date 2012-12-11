namespace GPhone {

private class History : Object {
	public enum Direction {
		UNDEF,
		IN,
		OUT,
	}

	private Sqlite.Database db;

	private string token;
	private string remote_party;
	private DateTime started;
	private DateTime ended;
	private Gopal.CallEndReason reason;
	private Direction direction;

	public History () {
		reset ();

		var dbpath = Environment.get_user_data_dir () +
		Path.DIR_SEPARATOR_S + "gphone";

		if (!FileUtils.test (dbpath, FileTest.IS_DIR)) {
			DirUtils.create_with_parents (dbpath, 0775);
		}

		dbpath += Path.DIR_SEPARATOR_S + "history.db";

		var rc = Sqlite.Database.open (dbpath, out db);
        if (rc != Sqlite.OK) {
            critical ("Can't open database: %d, %s", rc, db.errmsg ());
			db = null;
			return;
        }

		var sql = "CREATE TABLE IF NOT EXISTS history (" +
		"remote_party TEXT, " +
		"started DATE, " +
		"ended DATE, " +
		"reason INTEGER, " +
		"direction INTEGER)";

		string err;
		rc = db.exec (sql, null, out err);
		if (rc != Sqlite.OK) {
			critical ("Failed to create table: %s", err);
			db = null;
		}
	}

	public bool mark (string _token, string _remote_party, Direction _direction) {
		if (db == null)
			return false;

		if (token != null)
			reset ();

		token = _token;
		remote_party = _remote_party;
		direction = _direction;
		started = new DateTime.now_local ();

		return true;
	}

	public bool commit (string _token, Gopal.CallEndReason _reason) {
		if (db == null)
			return false;

		if (token != _token) {
			reset ();
			return false;
		}

		reason = _reason;
		ended = new DateTime.now_local ();

		var sql = "INSERT INTO history " +
		"(remote_party, started, ended, reason, direction) " +
		"VALUES (?, ?, ?, ?, ?)";

		Sqlite.Statement stmt = null;
		var rc = db.prepare_v2 (sql, -1, out stmt, null);
		if (rc != Sqlite.OK) {
			critical ("SQL error: %d, %s",  rc, db.errmsg ());
			return false;
		}

		stmt.bind_text (1, remote_party);
		stmt.bind_text (2, started.to_string ());
		stmt.bind_text (3, ended.to_string ());
		stmt.bind_int (4, reason);
		stmt.bind_int (5, direction);

		while ((rc = stmt.step ()) == Sqlite.BUSY);

		reset ();

		return rc == Sqlite.DONE;
	}

	private void reset () {
		token = null;
		remote_party = null;
		direction = Direction.UNDEF;
		started = null;
		ended = null;
		reason = Gopal.CallEndReason.MAX;
	}
}

}