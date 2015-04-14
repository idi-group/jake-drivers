import JAKE.*;

public class jake_device_test {

	public static void main(String[] args) {
		jake_device jd = new jake_device();
		jd.connect(7);

		try { Thread.sleep(3000);	} catch (InterruptedException ie) {}

		System.out.println("Setting sample rate to 60Hz");
		jd.write_sample_rate(jd.SAMPLE_RATE_60);

		// display some sensor data
		int[] xyz = new int[3];
		for(int i=0;i<100;i++) {
			jd.acc(xyz);
			System.out.println("Acc: " + xyz[0] + ", " + xyz[1] + ", " + xyz[2]);
			jd.mag(xyz);
			System.out.println("Mag: " + xyz[0] + ", " + xyz[1] + ", " + xyz[2]);
			System.out.println("Heading: " + jd.heading());		
			try { Thread.sleep(10); } catch (InterruptedException ie2) {}
		}

		int power_source = jd.info_power_source();
		int power_level = jd.info_power_level();
		System.out.print("Power level: " + power_level + "%, power source = ");
		if(power_source == 1)
			System.out.println("USB");
		else
			System.out.println("Battery");

		System.out.println("Firmware: v" + jd.info_firmware_revision());
		System.out.println("Hardware: v" + jd.info_hardware_revision());
		System.out.println("Serial: " + jd.info_serial_number());

		System.out.println("Config register value: " + jd.read_main(0x00));

		System.out.println("Closing connection");
		jd.close();
	}

}
