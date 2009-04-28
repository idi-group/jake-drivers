package JAKE;

/* This class defines the public JAKE API */

public class JAKEDevice {
	
	private internalJAKE priv = null;

	
	public JAKEDevice(JAKESerialInterface _port) {
		priv = new internalJAKE(_port);
	}

	public boolean connect(String btaddress) {
		if(!priv.openConnection(btaddress))
			return false;
		
		return true;
	}

	public boolean connect(int com_port) {
		if(!priv.openConnection(com_port))
			return false;
		
		return true;
	}
	
	public boolean close() {
		return priv.closeConnection();
	}
	
	public void waitForACKs(boolean wait_on) {
		priv.wait_for_acks = wait_on;
	}
	
	public int accx() {
		return priv.acc[0];
	}
	
	public int accy() {
		return priv.acc[1];
	}
	
	public int accz() {
		return priv.acc[2];
	}
	
	public int[] acc() {
		return priv.acc;
	}
	
	public int magx() {
		return priv.mag[0];
	}
	
	public int magy() {
		return priv.mag[1];
	}
	
	public int magz() {
		return priv.mag[2];
	}
	
	public int[] mag() {
		return priv.mag;
	}
	
	public int heading() {
		return priv.heading;
	}
	
	public int data_timestamp() {
		return priv.timestamp;
	}
	
	public void setDebugOut(JAKEDebug dbg) {
		priv.dbg = dbg;
	}

	public float firmwareRevision() {
		if(!priv.info_retrieved) {
			priv.getDeviceInfo();
		}
		return priv.fwrev;
	}
	
	public float hardwareRevision() {
		if(!priv.info_retrieved) {
			priv.getDeviceInfo();
		}
		return priv.hwrev;
	}
	
	public String serialNumber() {
		if(!priv.info_retrieved) {
			priv.getDeviceInfo();
		}
		return priv.serial;
	}
	
	public int readMain(int reg_address) {
		return priv.readMainRegister(reg_address);
	}
	
	public boolean writeMain(int reg, int val) {
		return priv.writeMainRegister(reg, val);
	}

	public int readConfiguration() {
		return priv.readMainRegister(0);
	}

	public boolean writeConfiguration(int value) {
		return priv.writeMainRegister(0, value);
	}

	public int readConfiguration2() {
		return priv.readMainRegister(1);
	}

	public boolean writeConfiguration2(int value) {
		return priv.writeMainRegister(1, value);
	}

	public int readAccOffset(int xyz, int range) {
		int base_reg = -1;
		switch(range) {
			case JAKEConstants.JAKE_ACCEL_RANGE_2G:
				base_reg = JAKEConstants.JAKE_REG_ACCX_2G_OFFSET_LSB;
				break;
			case JAKEConstants.JAKE_ACCEL_RANGE_6G:
				base_reg = JAKEConstants.JAKE_REG_ACCX_6G_OFFSET_LSB;
				break;
			default:
				return JAKEConstants.JAKE_ERROR;
		}
		
		switch(xyz) {
			case JAKEConstants.JAKE_X_AXIS:
				break;
			case JAKEConstants.JAKE_Y_AXIS:
				base_reg += 2;
				break;
			case JAKEConstants.JAKE_Z_AXIS:
				base_reg += 4;
				break;
			default:
				return JAKEConstants.JAKE_ERROR;
		}
		
		int msb = JAKEConstants.JAKE_ERROR;
		int lsb = JAKEConstants.JAKE_ERROR;
		lsb = priv.readMainRegister(base_reg);
		if(lsb == JAKEConstants.JAKE_ERROR)
			return JAKEConstants.JAKE_ERROR;
		msb = priv.readMainRegister(base_reg+1);
		if(msb == JAKEConstants.JAKE_ERROR)
			return JAKEConstants.JAKE_ERROR;
		
		return (msb << 8) + lsb;
	}
	
	public boolean writeAccOffset(int xyz, int range, int value) {
		int base_reg = -1;
		switch(range) {
			case JAKEConstants.JAKE_ACCEL_RANGE_2G:
				base_reg = JAKEConstants.JAKE_REG_ACCX_2G_OFFSET_LSB;
				break;
			case JAKEConstants.JAKE_ACCEL_RANGE_6G:
				base_reg = JAKEConstants.JAKE_REG_ACCX_6G_OFFSET_LSB;
				break;
			default:
				return false;
		}
		
		switch(xyz) {
			case JAKEConstants.JAKE_X_AXIS:
				break;
			case JAKEConstants.JAKE_Y_AXIS:
				base_reg += 2;
				break;
			case JAKEConstants.JAKE_Z_AXIS:
				base_reg += 4;
				break;
			default:
				return false;
		}
		
		int lsb, msb;
		lsb = (value & 0x00ff);
		msb = (value & 0xff00) >> 8;
		if(!priv.writeMainRegister(base_reg, lsb))
			return false;
		if(!priv.writeMainRegister(base_reg+1, msb))
			return false;
		
		return true;
	}
	
	public int readAccScale(int xyz, int range) {
		int base_reg = -1;
		switch(range) {
			case JAKEConstants.JAKE_ACCEL_RANGE_2G:
				base_reg = JAKEConstants.JAKE_REG_ACCX_2G_SCALE_LSB;
				break;
			case JAKEConstants.JAKE_ACCEL_RANGE_6G:
				base_reg = JAKEConstants.JAKE_REG_ACCX_6G_SCALE_LSB;
				break;
			default:
				return JAKEConstants.JAKE_ERROR;
		}
		
		switch(xyz) {
			case JAKEConstants.JAKE_X_AXIS:
				break;
			case JAKEConstants.JAKE_Y_AXIS:
				base_reg += 2;
				break;
			case JAKEConstants.JAKE_Z_AXIS:
				base_reg += 4;
				break;
			default:
				return JAKEConstants.JAKE_ERROR;
		}
		
		int msb = JAKEConstants.JAKE_ERROR;
		int lsb = JAKEConstants.JAKE_ERROR;
		lsb = priv.readMainRegister(base_reg);
		if(lsb == JAKEConstants.JAKE_ERROR)
			return JAKEConstants.JAKE_ERROR;
		msb = priv.readMainRegister(base_reg+1);
		if(msb == JAKEConstants.JAKE_ERROR)
			return JAKEConstants.JAKE_ERROR;
		
		return (msb << 8) + lsb;
	}
	
	public boolean writeAccScale(int xyz, int range, int value) {
		int base_reg = -1;
		switch(range) {
			case JAKEConstants.JAKE_ACCEL_RANGE_2G:
				base_reg = JAKEConstants.JAKE_REG_ACCX_2G_SCALE_LSB;
				break;
			case JAKEConstants.JAKE_ACCEL_RANGE_6G:
				base_reg = JAKEConstants.JAKE_REG_ACCX_6G_SCALE_LSB;
				break;
			default:
				return false;
		}
		
		switch(xyz) {
			case JAKEConstants.JAKE_X_AXIS:
				break;
			case JAKEConstants.JAKE_Y_AXIS:
				base_reg += 2;
				break;
			case JAKEConstants.JAKE_Z_AXIS:
				base_reg += 4;
				break;
			default:
				return false;
		}
		
		int lsb, msb;
		lsb = (value & 0x00ff);
		msb = (value & 0xff00) >> 8;
		if(!priv.writeMainRegister(base_reg, lsb))
			return false;
		if(!priv.writeMainRegister(base_reg+1, msb))
			return false;
		
		return true;
	}

	public int readMagScale(int xyz) {
		int base_reg = JAKEConstants.JAKE_REG_MAGX_SCALE_LSB;
		
		switch(xyz) {
			case JAKEConstants.JAKE_X_AXIS:
				break;
			case JAKEConstants.JAKE_Y_AXIS:
				base_reg += 2;
				break;
			case JAKEConstants.JAKE_Z_AXIS:
				base_reg += 4;
				break;
			default:
				return JAKEConstants.JAKE_ERROR;
		}
		
		int msb = JAKEConstants.JAKE_ERROR;
		int lsb = JAKEConstants.JAKE_ERROR;
		lsb = priv.readMainRegister(base_reg);
		if(lsb == JAKEConstants.JAKE_ERROR)
			return JAKEConstants.JAKE_ERROR;
		msb = priv.readMainRegister(base_reg+1);
		if(msb == JAKEConstants.JAKE_ERROR)
			return JAKEConstants.JAKE_ERROR;
		
		return (msb << 8) + lsb;
	}
	
	public boolean writeMagScale(int xyz, int value) {
		int base_reg = JAKEConstants.JAKE_REG_MAGX_SCALE_LSB;

		switch(xyz) {
			case JAKEConstants.JAKE_X_AXIS:
				break;
			case JAKEConstants.JAKE_Y_AXIS:
				base_reg += 2;
				break;
			case JAKEConstants.JAKE_Z_AXIS:
				base_reg += 4;
				break;
			default:
				return false;
		}
		
		int lsb, msb;
		lsb = (value & 0x00ff);
		msb = (value & 0xff00) >> 8;
		if(!priv.writeMainRegister(base_reg, lsb))
			return false;
		if(!priv.writeMainRegister(base_reg+1, msb))
			return false;
		
		return true;
	}
	
	public int readMagOffset(int xyz) {
		int base_reg = JAKEConstants.JAKE_REG_MAGX_OFFSET_LSB;
		
		switch(xyz) {
			case JAKEConstants.JAKE_X_AXIS:
				break;
			case JAKEConstants.JAKE_Y_AXIS:
				base_reg += 2;
				break;
			case JAKEConstants.JAKE_Z_AXIS:
				base_reg += 4;
				break;
			default:
				return JAKEConstants.JAKE_ERROR;
		}
		
		int msb = JAKEConstants.JAKE_ERROR;
		int lsb = JAKEConstants.JAKE_ERROR;
		lsb = priv.readMainRegister(base_reg);
		if(lsb == JAKEConstants.JAKE_ERROR)
			return JAKEConstants.JAKE_ERROR;
		msb = priv.readMainRegister(base_reg+1);
		if(msb == JAKEConstants.JAKE_ERROR)
			return JAKEConstants.JAKE_ERROR;
		
		return (msb << 8) + lsb;
	}
	
	public boolean writeMagOffset(int xyz, int value) {
		int base_reg = JAKEConstants.JAKE_REG_MAGX_OFFSET_LSB;

		switch(xyz) {
			case JAKEConstants.JAKE_X_AXIS:
				break;
			case JAKEConstants.JAKE_Y_AXIS:
				base_reg += 2;
				break;
			case JAKEConstants.JAKE_Z_AXIS:
				base_reg += 4;
				break;
			default:
				return false;
		}
		
		int lsb, msb;
		lsb = (value & 0x00ff);
		msb = (value & 0xff00) >> 8;
		if(!priv.writeMainRegister(base_reg, lsb))
			return false;
		if(!priv.writeMainRegister(base_reg+1, msb))
			return false;
		
		return true;
	}
	
	public int readSampleRate() {
		int rate = priv.readMainRegister(JAKEConstants.JAKE_REG_CONFIG1);
		if(rate == JAKEConstants.JAKE_ERROR)
			return JAKEConstants.JAKE_ERROR;
		
		return rate & 0x06; // only lower 3 bits
	}
	
	public boolean writeSampleRate(int new_rate) {
		if(new_rate < JAKEConstants.JAKE_SAMPLE_RATE_0 || new_rate > JAKEConstants.JAKE_SAMPLE_RATE_120) {
			// work out best matching rate
			if(new_rate < 0) {
				new_rate = JAKEConstants.JAKE_SAMPLE_RATE_0;
			} else if(new_rate > 120) {
				new_rate = JAKEConstants.JAKE_SAMPLE_RATE_120;
			} else {
				int min = 0xFFFF;
				int minpos = 0;
				for(int i=0;i<JAKEConstants.jake_output_rates.length;i++) {
					if((int)Math.abs(new_rate - JAKEConstants.jake_output_rates[i]) < min) {
						minpos = i;
						min = (int)Math.abs(new_rate - JAKEConstants.jake_output_rates[i]);
					}
				}
				new_rate = minpos;
			}
		}

		System.out.println("writesamplerate = " + new_rate);
		if(priv.writeMainRegister(JAKEConstants.JAKE_REG_CONFIG1, new_rate))
			return true;
		
		return false;
	}
}
