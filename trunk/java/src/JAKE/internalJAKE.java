package JAKE;

import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;
import java.lang.Runnable;

/* This class contains the code for parsing and updating sensor data etc */

public class internalJAKE implements Runnable {
	protected JAKESerialInterface port = null;
	private boolean closed = true;

	protected boolean checksum = false;
	private boolean waiting_for_ack = false;
	private boolean lastack = false;
	protected int lastval = -1;
	protected int lastaddr = -1;
	private boolean waiting_for_ack_signal = false;
	protected boolean wait_for_acks = false;
	
	protected int[] acc = new int[3];
	protected int[] mag = new int[3];
	protected int heading ;
	protected int power, rssi;
	
    protected int timestamp = 0;

	protected boolean info_retrieved = false;
    protected String serial;
	protected float fwrev, hwrev;
	
	protected int lastevent;
	protected String lasttid;
	
	protected JAKEDebug dbg = null;
	
    private byte[] sendMsg = null;
        
	protected Thread parser = null;

	private static final int JAKE_ASCII_READ_OK = 1;
	private static final int JAKE_ASCII_READ_ERROR = -1;
	private static final int JAKE_ASCII_READ_CONTINUE = 2;
	
	private static final int JAKE_RAW_READ_OK = 1;
	private static final int JAKE_RAW_READ_ERROR = -1;
	private static final int JAKE_RAW_READ_CONTINUE = 2;

	public internalJAKE(JAKESerialInterface _port) {
		JAKEPackets.initialiseHeaders();
		port = _port;
	}
	
	public boolean openConnection(String btaddress) {

		if(!port.openConnection(btaddress))
			return false;
		closed = false;

		parser = new Thread(this);
		parser.start();
		
		return true;
	}
	
	public boolean openConnection(int com_port) {

		if(!port.openConnection(com_port))
			return false;
		closed = false;

		parser = new Thread(this);
		parser.start();
		
		return true;
	}
	
	public boolean closeConnection() {
		closed = true;
		if(!port.closeConnection())
			return false;
		
		return true;
	}
	
	public int readMainRegister(int address) {
		if(closed)
			return JAKEConstants.JAKE_ERROR;
		
		byte[] readReg = JAKERegister.getMainReadString(address);
		port.writeBytes(readReg, readReg.length, 0);
		
		if(dbg != null) dbg.debug("readReg wrote: " + readReg);
		
		this.waiting_for_ack_signal = true;
		this.waiting_for_ack = true;
		int timeout = 250;
		
		while(this.waiting_for_ack_signal) {
			try {
				Thread.sleep(1);
                Thread.yield();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			--timeout;
			if(timeout == 0)
				break;
		}

		this.waiting_for_ack = false;
		if(!this.lastack) {
			return JAKEConstants.JAKE_ERROR;
		}
		
		return this.lastval;
	}
	
	public boolean writeMainRegister(int reg, int val) {
            if(closed)
                return false;
	
            byte[] writeReg = JAKERegister.getMainWriteString(reg, val);
            if(dbg != null) dbg.debug("writeReg start: " + writeReg);
         
            port.writeBytes(writeReg, writeReg.length, 0);
			if(!wait_for_acks)
				return true;
            this.waiting_for_ack_signal = true;
            this.waiting_for_ack = true;
            
            if(dbg != null) dbg.debug("writeReg end: " + writeReg);

            int timeout = 150;

            while(this.waiting_for_ack_signal) {
                try {
                        Thread.sleep(1);
                } catch (InterruptedException e) {
                        e.printStackTrace();
                }
                --timeout;
                if(timeout == 0)
                        break;
            }

            if(dbg != null) dbg.debug("writeReg wfas: " + this.waiting_for_ack_signal);

            this.waiting_for_ack = false;
            if(!this.lastack) {
                return false;
            }

            return true;
	}
	
	private static int JAKE_HEADER = 0;
	private static int JAKE_COPYRIGHT = 1;
	private static int JAKE_FIRMWARE_REV = 2; 
	private static int JAKE_HARDWARE_REV = 3;
	private static int JAKE_SERIAL_NUMBER = 4;


    private int readBytesWrapper(byte[] buf, int len, int offset) {
            if(dbg != null) dbg.debug("rBW 1: want to read " + len + " bytes");
            int bytes_read = 0;
            
            if(dbg != null) dbg.debug("rBW 4: reading " + len + " bytes, bytes_read = " + bytes_read);
            int retries = 0;
            while(retries < 30 && bytes_read != len) {
                bytes_read += port.readBytes(buf, len - bytes_read, offset + bytes_read);
                if(dbg != null) dbg.debug("rBW 5: bytes_read = " + bytes_read);
                retries++;
                if(retries % 10 == 0) try { Thread.sleep(10); } catch (InterruptedException ie) {}
            }
            if(dbg != null) dbg.debug("rBW 6: completed with " + bytes_read + " bytes after " + retries + " retries\n");
 
            return bytes_read;
	}
	
	private int jake_read_info_line(byte[] buf, int maxlen) {
		int bufpos = 0;
		byte[] b = new byte[1];
		
		while(bufpos < maxlen) {
			readBytesWrapper(b, 1, 0);
			
			if(b[0] == 0)
				continue;
			
			buf[bufpos] = b[0];
			
			if(buf[bufpos] == 0xD || buf[bufpos] == 0xA) {
				if(bufpos >= 1) {
					int i;
					
					i = bufpos - 1;
					if((buf[i] == 0xA && buf[i+1] == 0xD) || (buf[i] == 0xD && buf[i+1] == 0xA))
						return i;
				}
			}
			bufpos++;
		}
		
		return -1;
	}
	
	private int read_jake_device_info() {
		byte[][] buf = new byte[7][200];
		int linecount = 0, len;
		
		for(linecount=0;linecount<7;linecount++) {
			len = jake_read_info_line(buf[linecount], 200);
			if(len == -1)
				return JAKEConstants.JAKE_ERROR;
			
			if(linecount == JAKE_FIRMWARE_REV) {
				int plen = 0;
				byte[] ptr = buf[linecount];
				while((ptr[plen] != 0xA && ptr[plen] != 0xD) && (ptr[plen] < '0' || ptr[plen] > '9')) {
					plen++;
				}
				try {
					this.fwrev = Float.parseFloat(new String(ptr).substring(plen, plen+4));
                } catch (NumberFormatException nfe) {}
			} else if(linecount == JAKE_HARDWARE_REV) {
				int plen = 0;
				byte[] ptr = buf[linecount];
				while((ptr[plen] != 0xA && ptr[plen] != 0xD) && (ptr[plen] < '0' || ptr[plen] > '9')) {
					plen++;
				}
				this.hwrev = Float.parseFloat(new String(ptr).substring(plen, plen+4));
			} else if(linecount == JAKE_SERIAL_NUMBER) {
				byte[] ptr = buf[linecount];
				int spacecount = 0, plen = 0;
				while(spacecount < 2) {
					if(ptr[plen] == ' ') 
						spacecount++;
					plen++;
				}
				
				this.serial = new String(ptr).substring(plen);
				this.serial = this.serial.substring(0, this.serial.indexOf('\r'));
			}
		}

		
		return 1;
	}
	
	public void run() {
        try {
			int packet_type = JAKEPackets.JAKE_BAD_PACKET;
			int bytes_read;
			boolean valid_header = false;
			byte[] packetbuf = new byte[128];
			
			while(!closed) {
	            valid_header = false;
	            packet_type = JAKEPackets.JAKE_BAD_PACKET;
	            
	            do {
	                if(dbg != null) dbg.debug("\nNEW LOOP\n");
	                // read 4 byte packet header
	                bytes_read = readBytesWrapper(packetbuf, JAKEPackets.JAKE_HEADER_LEN, 0);
	                if(bytes_read == JAKEPackets.JAKE_HEADER_LEN) {
                        if(packetbuf[0] == '$') {
                        	packet_type = JAKEPackets.classifyHeader(packetbuf, JAKEPackets.JAKE_HEADER_LEN);
                        }
	                }
			
	                // error finding header
	                if(packet_type == JAKEPackets.JAKE_BAD_PACKET) {
	                        byte[] c = new byte[1];
	
	                        int read_count = 0;
	                        while(read_count < 50 && (c[0] != '$')) {
	                                readBytesWrapper(c, 1, 0);
	                                read_count++;
	                        }
	
	                        packetbuf[0] = c[0];
	                        if(c[0] == '$') {
	                        	readBytesWrapper(packetbuf, JAKEPackets.JAKE_HEADER_LEN - 1, 1);
	                            packet_type = JAKEPackets.classifyHeader(packetbuf, JAKEPackets.JAKE_HEADER_LEN);
	                        }
	                }
			
	                if(packet_type != JAKEPackets.JAKE_BAD_PACKET)
	                    valid_header = true;
	                else
	                    if(dbg != null) dbg.debug("Bad header: " + packetbuf[0] + packetbuf[1] + packetbuf[2] + packetbuf[3]);
			
	            } while (!closed && !valid_header);
		
	            if(closed)
                    return;
	
	            if(dbg != null) dbg.debug("\nPacket type: " + packet_type);

	            read_jake_packet(packet_type, packetbuf);
	        }
			
			if(dbg != null)  dbg.debug("Thread exiting");
        } catch (Exception e) {
            
               e.printStackTrace();
        }
	}
	
	private int read_jake_packet(int packet_type, byte[] packetbuf) {
		int packet_size = 0, bytes_left, bytes_read = 0;
		
		bytes_left = JAKEPackets.jake_packet_lengths[packet_type] - JAKEPackets.JAKE_HEADER_LEN;
		bytes_read = readBytesWrapper(packetbuf, bytes_left, JAKEPackets.JAKE_HEADER_LEN);
		if(bytes_left != bytes_read) {
			return JAKEPackets.JAKE_READ_ERROR;
		}
		
		return parse_jake_packet(packet_type, packetbuf, bytes_read + JAKEPackets.JAKE_HEADER_LEN);
	}

	private int parse_jake_packet(int packet_type, byte[] packetbuf, int len) {
		if(packet_type == JAKEPackets.JAKE_DATA) {
			update_data(packet_type, packetbuf);
		} else if(packet_type == JAKEPackets.JAKE_ACK_ACK || packet_type == JAKEPackets.JAKE_ACK_NEG) {
			parse_ack(packet_type, packetbuf);
		}
		
		return JAKEPackets.JAKE_READ_OK;
	}
	
	private void update_data(int packet_type, byte[] packet) {
		// accel data, 3x16 bit signed
		for(int a=0;a<3;a++) {
			int lsb = packet[4+(a*2)];
			int msb = packet[4+(a*2)+1];
			this.acc[a] = JAKEPackets.fixBytes(lsb, msb);
		}
		// mag data, 3x16 bit signed
		for(int m=0;m<3;m++) {
			int lsb = packet[10+(m*2)+1];
			int msb = packet[10+(m*2)];
			this.mag[m] = JAKEPackets.fixBytes(msb, lsb);
		}
		// heading data, 1x16 bit signed
		this.heading = JAKEPackets.fixBytes(packet[16], packet[17]);
		
		// power, 1x8 bit unsigned
		this.power = 0xFF & (int)packet[18];
		
		// rssi, 1x8 bit signed
		this.rssi = packet[19];
		
		// lastval, 1x8 bit unsigned
		//this.lastval = packet[20];
		
		// timestamp, 1x16 bit unsigned
		this.timestamp = ((int)packet[21] & 0xFF) | (((int)packet[22] & 0xFF) << 8); 
			//0xJAKEPackets.fixBytes(packet[21], packet[22]);
		
	}
	
	private void parse_ack(int packet_type, byte[] packetbuf) {
		// lastval, 1x8 bit unsigned
		this.lastval = 0xFF & packetbuf[20];
		
		if(packet_type == JAKEPackets.JAKE_ACK_ACK)
			this.lastack = true;
		else
			this.lastack = false;
	}
	
	protected void getDeviceInfo() {
		int fwminor, fwmajor;
		int hwminor, hwmajor;
		int sminor, smajor;

		fwminor = readMainRegister(JAKEConstants.JAKE_REG_FW_MINOR);
		fwmajor = readMainRegister(JAKEConstants.JAKE_REG_FW_MAJOR);

		hwminor = readMainRegister(JAKEConstants.JAKE_REG_HW_MINOR);
		hwmajor = readMainRegister(JAKEConstants.JAKE_REG_HW_MAJOR);

		sminor = readMainRegister(JAKEConstants.JAKE_REG_SERIAL_LSB);
		smajor = readMainRegister(JAKEConstants.JAKE_REG_SERIAL_MSB);

		info_retrieved = true;
		fwrev = (float)(fwmajor + (0.01 * fwminor));
		hwrev = (float)(hwmajor + (0.01 * hwminor));
		this.serial = JAKEUtils.formatDec(smajor + (sminor << 8), 4);
	}
}
