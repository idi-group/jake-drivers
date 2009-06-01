package JAKEJ2METest;

import java.io.IOException;
import java.io.PrintStream;

import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;
import javax.microedition.lcdui.List;
import javax.bluetooth.*;

import java.io.OutputStream;
import java.util.Vector;
import java.util.Hashtable;

import javax.microedition.lcdui.*;
import javax.microedition.io.file.*;

import JAKE.*;

public class JAKEJ2METest extends MIDlet implements DiscoveryListener, CommandListener, Runnable {
	Command exitCommand = null;
	Command connectCommand = null;
	
	JAKEDevice jake = null;
 	List list = null;
    FileConnection fc;
	OutputStream os = null;
	LocalDevice ld = null;
	DiscoveryAgent da = null;

	Hashtable devices = null;
	Thread thr = null;

	int mmh_sample_rate = 0; //; 10; // default sample rate in Hz at which raw acc data is sent to MMH
	int mmh_sender = 1;

	TextBox tb = null;

	boolean recognising = false;
        int gesture_events_sent = 0;

	public JAKEJ2METest() {
		exitCommand = new Command("Exit", Command.EXIT, 1);
		connectCommand = new Command("Connect", Command.ITEM, 2);
		
		devices = new Hashtable();

               
	}

	protected void destroyApp(boolean arg0) throws MIDletStateChangeException {

	}

	protected void pauseApp() {
	}

	protected void startApp() throws MIDletStateChangeException {

		list = new List("SHAKE MIDlet", List.IMPLICIT);
		list.addCommand(connectCommand);
		list.addCommand(exitCommand);
              
		list.setCommandListener(this);
		list.setSelectCommand(List.SELECT_COMMAND);
		Display display = Display.getDisplay(this);
		display.setCurrent(list);

	}

	public void deviceDiscovered(RemoteDevice arg0, DeviceClass arg1) {
		try {
			list.append(arg0.getFriendlyName(false), null);
			devices.put(arg0.getFriendlyName(false), arg0.getBluetoothAddress());
         
		} catch (IOException e) {
		}
	}

	public void inquiryCompleted(int arg0) {
	}

	public void serviceSearchCompleted(int arg0, int arg1) {
	}

	public void servicesDiscovered(int arg0, ServiceRecord[] arg1) {
	}

	public void commandAction(Command arg0, Displayable arg1) {
		if(arg0 == List.SELECT_COMMAND && jake == null) {
			String device = list.getString(list.getSelectedIndex());
			list.append("Connecting...", null);
			if(da != null) da.cancelInquiry(this);
			jake = new JAKEDevice(new JAKEJ2MESerialPort());
			try {
				//shake.setDebugOut(this);
				if(jake.connect((String)devices.get(device))) {
					list.deleteAll();
					list.append(device, null);
					
					thr = new Thread(this);
	
					list.append("Running...", null);

					thr.start();
				} else {
                    list.append("Connect failed!", null);
                }
			} catch (Exception e) {
				list.append(e.getMessage(), null);
			}
		} else if(arg0.equals(exitCommand)) {
                        list.append("exitCommand", null);
			if(da != null) da.cancelInquiry(this);
                        list.append("cancelInquiry", null);

			if(jake != null) {
                                list.append("closing shake", null);

                                try {
                                    jake.close();

                                } catch (Exception e) {
                                    list.append(e.getMessage(), null);
                                }
				jake = null;
			}
                        list.append("shake closed", null);
                        try {
                            list.append("closing os&fc", null);
                            if(os != null) os.close();
                            if(fc != null) fc.close();
                        } catch (IOException ie) {}

                        list.append("destroyApp", null);
			try {
				destroyApp(false);
			} catch (MIDletStateChangeException e) {
			}

			notifyDestroyed();
		} else if (arg0.equals(connectCommand)) {
			list.deleteAll();
			try {
				ld = LocalDevice.getLocalDevice();
				da = ld.getDiscoveryAgent();
				devices.clear();
				da.startInquiry(DiscoveryAgent.GIAC, this);
			} catch (BluetoothStateException ex) {
				list.append("exception in inquiry", null);
			}
		} 
	}

	

	public void run() {
		int[] acc = null;

		while(jake != null) {

            acc = jake.acc();
			list.append(acc[0] + " " + acc[1] + " " + acc[2], null);

			try {
				Thread.sleep(20);//33);
				mmh_sender++;
			} catch (InterruptedException e) {
			}
		}
	}
}
