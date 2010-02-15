

import JAKE.*;

public class jake_test implements JAKEDebug {


	public void debug(String dbg) {
		System.out.println(dbg);
	}

	public static void main(String[] args) {
		// create an object to receive the events from pressing the button on the SHAKE
		jake_test st = new jake_test();
		
		// the SHAKE object
		JAKEDevice s = new JAKEDevice(new JAKESerialPort());
		if(!s.connect(4)) {
		//SHAKEDevice s = new SHAKEDevice(new SHAKERXTX());
		//if(!s.connect("COM2")) {
			System.out.println("Failed to connect!");
			System.exit(1);
		}
		
		System.out.println("Connected");
		//s.setDebugOut(st);
		
		//System.out.println("Setting power state");
		// turn on button and accelerometer
		if(!s.writeConfiguration(JAKEConstants.JAKE_ACCEL | JAKEConstants.JAKE_MAG | JAKEConstants.JAKE_HEADING | JAKEConstants.JAKE_ACCEL_CALIBRATION | JAKEConstants.JAKE_MAG_CALIBRATION | JAKEConstants.JAKE_ACCEL_FILTER_ORDER_8_16 | JAKEConstants.JAKE_MAG_FILTER_ORDER_4_8))
			System.out.println("Failed to set power state");
		
		System.out.println("Setting sample rate");
		if(!s.writeSampleRate(120))
			System.out.println("Failed to set sample rate");
		
		try {
			for(int i=0;i<100;i++) {
				Thread.sleep(10);
				int[] acc = s.acc();
				int[] mag = s.mag();
				int heading = s.heading();
				System.out.println("Acc: " + acc[0] + ", " + acc[1] + ", " + acc[2] + " | " + s.data_timestamp());
				System.out.println("mag: " + mag[0] + ", " + mag[1] + ", " + mag[2]);
				System.out.println("Heading: " + heading);
				
			}
		} catch (InterruptedException e) {}
		
		System.out.println("Hardware revision: " + s.hardwareRevision());
		System.out.println("Firmware revision: " + s.firmwareRevision());
		System.out.println("Serial number: " + s.serialNumber());

		try {
			for(int i=0;i<5000;i++) {
				Thread.sleep(1);
				int[] acc = s.acc();
				//int[] mag = s.mag();
				//int heading = s.heading();
				System.out.println("Acc: " + acc[0] + ", " + acc[1] + ", " + acc[2] + " | " + s.data_timestamp());
				//System.out.println("mag: " + mag[0] + ", " + mag[1] + ", " + mag[2]);
				//System.out.println("Heading: " + heading);
				
			}
		} catch (InterruptedException e) {}
		
		s.close();
		
	}

}
