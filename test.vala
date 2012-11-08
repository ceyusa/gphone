using Gopal;

public class Phone : Object {
	private Gopal.Manager manager = new Gopal.Manager ();

	public void test () {
		var nat_type = manager.set_stun_server ("stun.ekiga.net");
		print ("NAT type %d %s\n", nat_type, manager.get_stun_server ());

		var sipep = manager.sip_endpoint;
		sipep.start_listeners ({ "127.0.0.1" });
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
