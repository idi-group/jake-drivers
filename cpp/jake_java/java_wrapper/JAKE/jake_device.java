package JAKE;

public class jake_device {

	static {
		try {
			System.loadLibrary("jake_java");	
		} catch (UnsatisfiedLinkError ule) {
			System.out.println("Failed to load jake_java.dll!");
			System.exit(-1);
		}
	}

	private long dev;
	
	private static int JAKE_ERROR = -1;
	private static int JAKE_SUCCESS = 1;

	public static int JAKE_SAMPLE_RATE_0 = 0;
	public static int JAKE_SAMPLE_RATE_7_5 = 1;
	public static int JAKE_SAMPLE_RATE_15 = 2;
	public static int JAKE_SAMPLE_RATE_30 = 3;
	public static int JAKE_SAMPLE_RATE_60 = 4;
	public static int JAKE_SAMPLE_RATE_120 = 5;
	
	public static int JAKE_X_AXIS = 0;
	public static int JAKE_Y_AXIS = 1;
	public static int JAKE_Z_AXIS = 2;

	public static int JAKE_ACCEL_RANGE_2G = 0;
	public static int JAKE_ACCEL_RANGE_6G = 1;

	public static int JAKE_CHARGING_USB = 0;
	public static int JAKE_BATTERY = 1;

	public static int JAKE_ACCEL = 0;
	public static int JAKE_MAG = 1;
	public static int JAKE_HEADING = 2;
	public static int JAKE_ACCEL_CALIBRATION = 3;
	public static int JAKE_MAG_CALIBRATION = 4;
	public static int JAKE_ACCEL_FILTER_ORDER_8_16 = 5;
	public static int JAKE_MAG_FILTER_ORDER_4_8 = 6;
	public static int JAKE_ACCEL_RANGE_2G_6G = 7;

	public jake_device() {
		dev = 0;
	}

	public boolean connect(int com_port) {
		dev = jake_init_device(com_port);
		if(dev == 0)
			return false;
		return true;
	}

	public boolean connect(String bt_addr) {
		// TODO
		return false;
	}

	public boolean connect_rfcomm(long btaddr) {
		dev = jake_init_device_rfcomm(btaddr);
		if(dev == 0)
			return false;
		return true;
	}

	public boolean close() {
		if(jake_free_device(dev) == JAKE_ERROR)
			return false;
		dev = 0;
		return true;
	}

	public float info_firmware_revision() {
		return jake_info_firmware_revision(dev);
	}

	public float info_hardware_revision() {
		return jake_info_hardware_revision(dev);
	}

	public String info_serial_number() {
		return jake_info_serial_number(dev);
	}

	public int info_rssi() {
		return jake_info_rssi(dev);
	}

	public int info_power_source() {
		return jake_info_power_source(dev);
	}

	public int info_power_level() {
		return jake_info_power_level(dev);
	}

	public long info_received_packets() {
		return jake_info_received_packets(dev);
	}

	public int accx() {
		return jake_accx(dev);
	}

	public int accy() {
		return jake_accy(dev);
	}

	public int accz() {
		return jake_accz(dev);
	}

	public int acc(int[] xyz) {
		return jake_acc(dev, xyz);
	}

	public int magx() {
		return jake_magx(dev);
	}

	public int magy() {
		return jake_magy(dev);
	}

	public int magz() {
		return jake_magz(dev);
	}

	public int mag(int[] xyz) {
		return jake_mag(dev, xyz);
	}

	public int heading() {
		return jake_heading(dev);
	}

	public int data_timestamp() {
		return jake_data_timestamp(dev);
	}

	public int read_main(int addr) {
		return jake_read_main(dev, addr);
	}

	public int write_main(int addr, int value) {
		return jake_write_main(dev, addr, value);
	}

	public int read_bluetooth(int addr) {
		return jake_read_bluetooth(dev, addr);
	}

	public int write_bluetooth(int addr, int value) {
		return jake_write_bluetooth(dev, addr, value);
	}

	public int read_sample_rate() {
		return jake_read_sample_rate(dev);
	}

	public int write_sample_rate(int newrate) {
		return jake_write_sample_rate(dev, newrate);
	}

	public int read_acc_offset(int xyz, int range) {
		return jake_read_acc_offset(dev, xyz, range);
	}

	public int write_acc_offset(int xyz, int range, short value) {
		return jake_write_acc_offset(dev, xyz, range, value);
	}

	public int read_acc_scale(int xyz, int range) {
		return jake_read_acc_scale(dev, xyz, range);
	}

	public int write_acc_scale(int xyz, int range, short value) {
		return jake_write_acc_scale(dev, xyz, range, value);
	}

	public int read_mag_offset(int xyz) {
		return jake_read_mag_offset(dev, xyz);
	}

	public int write_mag_offset(int xyz, short value) {
		return jake_write_mag_offset(dev, xyz, value);
	}

	public int read_mag_scale(int xyz) {
		return jake_read_mag_scale(dev, xyz);
	}

	public int write_mag_scale(int xyz, short value) {
		return jake_write_mag_scale(dev, xyz, value);
	}

	// Startup/shutdown functions
	private static native long jake_init_device(int com_port);
	private static native long jake_init_device_rfcomm(long btaddr);
	private static native int jake_free_device(long dev);

	// Information functions
	private static native float jake_info_firmware_revision(long dev);
	private static native float jake_info_hardware_revision(long dev);
	private static native String jake_info_serial_number(long dev);
	private static native int jake_info_rssi(long dev);
	private static native int jake_info_power_source(long dev);
	private static native int jake_info_power_level(long dev);
	private static native long jake_info_received_packets(long dev);

	// Data access functions
	private static native int jake_accx(long dev);
	private static native int jake_accy(long dev);
	private static native int jake_accz(long dev);
	private static native int jake_acc(long dev, int[] xyz);

	private static native int jake_magx(long dev);
	private static native int jake_magy(long dev);
	private static native int jake_magz(long dev);
	private static native int jake_mag(long dev, int[] xyz);

	private static native int jake_heading(long dev);

	private static native int jake_data_timestamp(long dev);

	// Register access functions
	private static native int jake_read_sample_rate(long dev);
	private static native int jake_write_sample_rate(long dev, int newrate);

	private static native int jake_read_acc_offset(long dev, int xyz, int range);
	private static native int jake_write_acc_offset(long dev, int xyz, int range, short value);

	private static native int jake_read_acc_scale(long dev, int xyz, int range);
	private static native int jake_write_acc_scale(long dev, int xyz, int range, short value);

	private static native int jake_read_mag_offset(long dev, int xyz);
	private static native int jake_write_mag_offset(long dev, int xyz, short value);

	private static native int jake_read_mag_scale(long dev, int xyz);
	private static native int jake_write_mag_scale(long dev, int xyz, short value);

	private static native int jake_read_main(long dev, int addr);
	private static native int jake_write_main(long dev, int addr, int value);

	private static native int jake_read_bluetooth(long dev, int addr);
	private static native int jake_write_bluetooth(long dev, int addr, int value);
}

