using Gopal;

public class Phone : Object {
	private Gopal.Manager manager = new Gopal.Manager ();

	public void test () {
		var nat_type = manager.set_stun_server ("stun.ekiga.net");
		print ("NAT type %d %s\n", nat_type, manager.get_stun_server ());

		var sipep = manager.sip_endpoint;
		if (sipep.start_listeners (null)) {
			print ("SIP listening\n");
		} else {
			print ("SIP listening failed\n");
		}

		var params = Gopal.SIPParams ();
		params.remote_address = "200.76.126.129:5060";
		params.address_of_record = "320";
		params.auth_ID = "320";
		params.password = "cul14c4n";
		params.expire = 300;

		var srp = Gopal.SIPRegisterParams ();
		srp.params = params;

		string aor;

		sipep.register (srp, out aor, null);
	}
}

int main (string[] args) {
	Gopal.init (ref args);

	var phone = new Phone();
	phone.test();
	phone = null;

    Gopal.deinit ();

	return 0;
}
