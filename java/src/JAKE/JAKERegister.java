package JAKE;

public class JAKERegister {
	
	public static byte[] getMainReadString(int address) {
		byte[] b = new byte[6];
		b[0] = b[1] = '$';
		b[2] = 'R';
		b[3] = (byte)address;
		b[4] = b[5] = 0;
		return b;
	}
	
	public static byte[] getMainWriteString(int address, int value) {
		byte[] b = new byte[6];
		b[0] = b[1] = '$';
		b[2] = 'W';
		b[3] = (byte)address;
		b[4] = (byte)value;
		b[5] = 0;
		return b;
	}
	
	public static byte[] getBluetoothReadString(int address) {
		byte[] b = new byte[6];
		b[0] = b[1] = '$';
		b[2] = 'r';
		b[3] = (byte)address;
		b[4] = b[5] = 0;
		return b;
	}
	
	public static byte[] getBluetoothWriteString(int address, int value) {
		byte[] b = new byte[6];
		b[0] = b[1] = '$';
		b[2] = 'w';
		b[3] = (byte)address;
		b[4] = (byte)value;
		b[5] = 0;
		return b;
	}
}
