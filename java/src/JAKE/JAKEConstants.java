package JAKE;

public class JAKEConstants {
	
	public static int JAKE_ERROR = -1;
	public static int JAKE_SUCCESS = 1;
	
	// bits in main configuration register
	public static final int JAKE_ACCEL = 0x01;
	public static final int JAKE_MAG = 0x02;
	public static final int JAKE_HEADING = 0x04;
	public static final int JAKE_ACCEL_CALIBRATION = 0x08;
	public static final int JAKE_MAG_CALIBRATION = 0x10;
	public static final int JAKE_ACCEL_FILTER_ORDER_8_16 = 0x20;
	public static final int JAKE_MAG_FILTER_ORDER_4_8 = 0x40;
	public static final int JAKE_ACCEL_RANGE_2G_6G = 0x80;

	public static final int JAKE_CHARGING_USB = 0;
	public static final int JAKE_BATTERY = 1;

	public static final int JAKE_SAMPLE_RATE_0 = 0;
	public static final int JAKE_SAMPLE_RATE_7_5 = 1;
	public static final int JAKE_SAMPLE_RATE_15 = 2;
	public static final int JAKE_SAMPLE_RATE_30 = 3;
	public static final int JAKE_SAMPLE_RATE_60 = 4;
	public static final int JAKE_SAMPLE_RATE_120 = 5;
	
	public static float[] jake_output_rates = { 0, 7.5f, 15, 30, 60, 120 };
	
	public static final int JAKE_X_AXIS = 0;
	public static final int JAKE_Y_AXIS = 1;
	public static final int JAKE_Z_AXIS = 2;
	
	public static final int JAKE_ACCEL_RANGE_2G = 0;
	public static final int JAKE_ACCEL_RANGE_6G = 1;

	public static final int SHAKE_TEMP_MIN = 0x00;
	public static final int SHAKE_TEMP_MAX = 0x40;

	public static final int JAKE_REG_CONFIG0 = 0;
	public static final int JAKE_REG_CONFIG1 = 1;

	public static final int JAKE_REG_ACCX_2G_OFFSET_LSB = 2;
	public static final int JAKE_REG_ACCX_2G_OFFSET_MSB = 3;

	public static final int JAKE_REG_ACCY_2G_OFFSET_LSB = 4;
	public static final int JAKE_REG_ACCY_2G_OFFSET_MSB = 5;

	public static final int JAKE_REG_ACCZ_2G_OFFSET_LSB = 6;
	public static final int JAKE_REG_ACCZ_2G_OFFSET_MSB = 7;

	public static final int JAKE_REG_ACCX_2G_SCALE_LSB = 8;
	public static final int JAKE_REG_ACCX_2G_SCALE_MSB = 9;

	public static final int JAKE_REG_ACCY_2G_SCALE_LSB = 10;
	public static final int JAKE_REG_ACCY_2G_SCALE_MSB = 11;

	public static final int JAKE_REG_ACCZ_2G_SCALE_LSB = 12;
	public static final int JAKE_REG_ACCZ_2G_SCALE_MSB = 13;
	
	public static final int JAKE_REG_ACCX_6G_OFFSET_LSB = 14;
	public static final int JAKE_REG_ACCX_6G_OFFSET_MSB = 15;
	
	public static final int JAKE_REG_ACCY_6G_OFFSET_LSB = 16;
	public static final int JAKE_REG_ACCY_6G_OFFSET_MSB = 17;

	public static final int JAKE_REG_ACCZ_6G_OFFSET_LSB = 18;
	public static final int JAKE_REG_ACCZ_6G_OFFSET_MSB = 19;

	public static final int JAKE_REG_ACCX_6G_SCALE_LSB = 20;
	public static final int JAKE_REG_ACCX_6G_SCALE_MSB = 21;

	public static final int JAKE_REG_ACCY_6G_SCALE_LSB = 22;
	public static final int JAKE_REG_ACCY_6G_SCALE_MSB = 23;

	public static final int JAKE_REG_ACCZ_6G_SCALE_LSB = 24;
	public static final int JAKE_REG_ACCZ_6G_SCALE_MSB = 25;

	public static final int JAKE_REG_MAGX_OFFSET_LSB = 26;
	public static final int JAKE_REG_MAGX_OFFSET_MSB = 27;

	public static final int JAKE_REG_MAGY_OFFSET_LSB = 28;
	public static final int JAKE_REG_MAGY_OFFSET_MSB = 29;

	public static final int JAKE_REG_MAGZ_OFFSET_LSB = 30;
	public static final int JAKE_REG_MAGZ_OFFSET_MSB = 31;

	public static final int JAKE_REG_MAGX_SCALE_LSB = 32;
	public static final int JAKE_REG_MAGX_SCALE_MSB = 33;

	public static final int JAKE_REG_MAGY_SCALE_LSB = 34;
	public static final int JAKE_REG_MAGY_2G_SCALE_MSB = 35;

	public static final int JAKE_REG_MAGZ_SCALE_LSB = 36;
	public static final int JAKE_REG_MAGZ_SCALE_MSB = 37;

	public static final int JAKE_REG_FW_MINOR = 38;
	public static final int JAKE_REG_FW_MAJOR = 39;

	public static final int JAKE_REG_HW_MINOR = 40;
	public static final int JAKE_REG_HW_MAJOR = 41;

	public static final int JAKE_REG_SERIAL_LSB = 42;
	public static final int JAKE_REG_SERIAL_MSB = 43;
}
