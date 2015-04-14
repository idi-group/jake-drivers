using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace JAKE.CS
{
    public enum jake_SENSORS : short
    {
        ACC,
        MAG,
        HEADING,
    }

    public enum jake_AXES : short
    {
        X_AXIS,
        Y_AXIS,
        Z_AXIS,
    }

    public enum jake_ACCEL_RANGES : short
    {
        ACCEL_RANGE_2G,
        ACCEL_RANGE_6G,
    }

    public enum jake_REGISTERS : short
    {
        jake_REG_CONFIG0 = 0x00,
        jake_REG_CONFIG1,

        jake_REG_ACCX_2G_OFFSET_LSB,
        jake_REG_ACCX_2G_OFFSET_MSB,

        jake_REG_ACCY_2G_OFFSET_LSB,
        jake_REG_ACCY_2G_OFFSET_MSB,

        jake_REG_ACCZ_2G_OFFSET_LSB,
        jake_REG_ACCZ_2G_OFFSET_MSB,

        jake_REG_ACCX_2G_SCALE_LSB,
        jake_REG_ACCX_2G_SCALE_MSB,

        jake_REG_ACCY_2G_SCALE_LSB,
        jake_REG_ACCY_2G_SCALE_MSB,

        jake_REG_ACCZ_2G_SCALE_LSB,
        jake_REG_ACCZ_2G_SCALE_MSB,

        jake_REG_ACCX_6G_OFFSET_LSB,
        jake_REG_ACCX_6G_OFFSET_MSB,

        jake_REG_ACCY_6G_OFFSET_LSB,
        jake_REG_ACCY_6G_OFFSET_MSB,

        jake_REG_ACCZ_6G_OFFSET_LSB,
        jake_REG_ACCZ_6G_OFFSET_MSB,

        jake_REG_ACCX_6G_SCALE_LSB,
        jake_REG_ACCX_6G_SCALE_MSB,

        jake_REG_ACCY_6G_SCALE_LSB,
        jake_REG_ACCY_6G_SCALE_MSB,

        jake_REG_ACCZ_6G_SCALE_LSB,
        jake_REG_ACCZ_6G_SCALE_MSB,

        jake_REG_MAGX_OFFSET_LSB,
        jake_REG_MAGX_OFFSET_MSB,

        jake_REG_MAGY_OFFSET_LSB,
        jake_REG_MAGY_OFFSET_MSB,

        jake_REG_MAGZ_OFFSET_LSB,
        jake_REG_MAGZ_OFFSET_MSB,

        jake_REG_MAGX_SCALE_LSB,
        jake_REG_MAGX_SCALE_MSB,

        jake_REG_MAGY_SCALE_LSB,
        jake_REG_MAGY_2G_SCALE_MSB,

        jake_REG_MAGZ_SCALE_LSB,
        jake_REG_MAGZ_SCALE_MSB,

        jake_REG_FW_MINOR,
        jake_REG_FW_MAJOR,

        jake_REG_HW_MINOR,
        jake_REG_HW_MAJOR,

        jake_REG_SERIAL_LSB,
        jake_REG_SERIAL_MSB,
    };

    public class jake_device
    {
        // stores C pointer to the device instance
        private IntPtr dev;

        // indicates if connection currently active
        private bool connected = false;

        private int[] iacc = new int[3] { 0, 0, 0 };
        private int[] imag = new int[3] { 0, 0, 0 };

        public const int jake_SUCCESS = 1;
        public const int jake_ERROR = -1;

        public const int jake_ACCEL = 0x01;
        public const int jake_MAG = 0x02;
        public const int jake_HEADING = 0x04;
        public const int jake_ACCEL_CALIBRATION = 0x08;
        public const int jake_MAG_CALIBRATION = 0x10;
        public const int jake_ACCEL_FILTER_ORDER_8_16 = 0x20;
        public const int jake_MAG_FILTER_ORDER_4_8 = 0x40;
        public const int jake_ACCEL_RANGE_2G_6G = 0x80;

        public const int jake_CHARGING_USB = 0x0;
        public const int jake_BATTERY = 0x01;

        public const int jake_OUTPUT_RATE_COUNT = 6;
        
        public float[] jake_OUTPUT_RATES = { 0, 7.5f, 15, 30, 60, 120 };

        public jake_device()
        {
            dev = IntPtr.Zero;
        }

        public bool connect(int com_port)
        {
            if (connected)
                close();

            dev = jake_init_device(com_port);
            if (dev.ToInt32() == 0)
                return false;

            connected = true;
            return true;
        }

        public bool connect_rfcomm_i64(Int64 btaddr)
        {
            if (connected)
                close();

            dev = jake_init_device_rfcomm_i64(btaddr);
            if (dev.ToInt32() == 0)
                return false;

            connected = true;
            return true;
        }

        public bool connect_rfcomm_str(string btaddr)
        {
            if (connected)
                close();

            dev = jake_init_device_rfcomm_str(btaddr);
            if (dev.ToInt32() == 0)
                return false;

            connected = true;
            return true;
        }

        public bool close()
        {
            if (!connected)
                return false;

            connected = false;

            jake_free_device(dev);
            return true;
        }

        public bool IsConnected
        {
            get
            {
                return connected;
            }
        }

        public int accx
        {
            get
            {
                return jake_accx(dev);
            }
        }

        public int accy
        {
            get
            {
                return jake_accy(dev);
            }
        }

        public int accz
        {
            get
            {
                return jake_accz(dev);
            }
        }

        public int[] acc
        {
            get
            {
                jake_acc(dev, iacc);
                return iacc;
            }
        }

        public int magx
        {
            get
            {
                return jake_magx(dev);
            }
        }

        public int magy
        {
            get
            {
                return jake_magy(dev);
            }
        }

        public int magz
        {
            get
            {
                return jake_magz(dev);
            }
        }

        public int[] mag
        {
            get
            {
                jake_mag(dev, imag);
                return imag;
            }
        }

        public int heading
        {
            get
            {
                return jake_heading(dev);
            }
        }

        public int data_timestamp(jake_SENSORS sensor)
        {
            return jake_data_timestamp(dev, (int)sensor);
        }

        public bool read_configuration(ref byte value)
        {
            if (jake_read_main(dev, (int)jake_REGISTERS.jake_REG_CONFIG0, out value) == jake_ERROR)
                return false;

            return true;
        }

        public bool write_configuration(byte value)
        {
            if (jake_write_main(dev, (int)jake_REGISTERS.jake_REG_CONFIG0, value) == jake_ERROR)
                return false;

            return true;
        }

        public bool read_configuration2(ref byte value)
        {
            if (jake_read_main(dev, (int)jake_REGISTERS.jake_REG_CONFIG1, out value) == jake_ERROR)
                return false;

            return true;
        }

        public bool write_configuration2(byte value)
        {
            if (jake_write_main(dev, (int)jake_REGISTERS.jake_REG_CONFIG1, value) == jake_ERROR)
                return false;

            return true;
        }

        public bool read_sample_rate(ref byte value)
        {
            if (jake_read_sample_rate(dev, out value) == jake_ERROR)
                return false;

            return true;
        }

        public bool write_sample_rate(byte value)
        {
            if (jake_write_sample_rate(dev, value) == jake_ERROR)
                return false;

            return true;
        }

        public bool read_acc_offset(jake_AXES xyz, jake_ACCEL_RANGES range, ref short value)
        {
            if (jake_read_acc_offset(dev, (int)xyz, (int)range, out value) == jake_ERROR)
                return false;

            return true;
        }

        public bool write_acc_offset(jake_AXES xyz, jake_ACCEL_RANGES range, short value)
        {
            if (jake_write_acc_offset(dev, (int)xyz, (int)range, value) == jake_ERROR)
                return false;

            return true;
        }

        public bool read_acc_scale(jake_AXES xyz, jake_ACCEL_RANGES range, ref short value)
        {
            if (jake_read_acc_scale(dev, (int)xyz, (int)range, out value) == jake_ERROR)
                return false;

            return true;
        }

        public bool write_acc_scale(jake_AXES xyz, jake_ACCEL_RANGES range, short value)
        {
            if (jake_write_acc_scale(dev, (int)xyz, (int)range, value) == jake_ERROR)
                return false;

            return true;
        }

        public bool read_mag_scale(jake_AXES xyz, ref short value)
        {
            if (jake_read_mag_scale(dev, (int)xyz, out value) == jake_ERROR)
                return false;

            return true;
        }

        public bool write_acc_scale(jake_AXES xyz, short value)
        {
            if (jake_write_mag_scale(dev, (int)xyz, value) == jake_ERROR)
                return false;

            return true;
        }

        public bool read_mag_offset(jake_AXES xyz, ref short value)
        {
            if (jake_read_mag_offset(dev, (int)xyz, out value) == jake_ERROR)
                return false;

            return true;
        }

        public bool write_mag_offset(jake_AXES xyz, short value)
        {
            if (jake_write_mag_offset(dev, (int)xyz, value) == jake_ERROR)
                return false;

            return true;
        }

        public bool read_main(int addr, ref byte value)
        {
            if (jake_read_main(dev, addr, out value) == jake_ERROR)
                return false;

            return true;
        }

        public bool write_main(int addr, byte value)
        {
            if (jake_write_main(dev, addr, value) == jake_ERROR)
                return false;

            return true;
        }

        public bool read_bluetooth(int addr, ref byte value)
        {
            if (jake_read_bluetooth(dev, addr, out value) == jake_ERROR)
                return false;

            return true;
        }

        public bool write_bluetooth(int addr, byte value)
        {
            if (jake_write_bluetooth(dev, addr, value) == jake_ERROR)
                return false;

            return true;
        }

        public double info_firmware_revision
        {
            get
            {
                return jake_info_firmware_revision(dev);
            }
        }

        public double info_hardware_revision
        {
            get
            {
                return jake_info_hardware_revision(dev);
            }
        }

        public string info_serial_number
        {
            get
            {
                return jake_info_serial_number(dev);
            }
        }

        public int rssi
        {

            get
            {
                return jake_info_rssi(dev);
            }
        }

        public int power_source
        {
            get
            {
                return jake_info_power_source(dev);
            }
        }

        public int power_level
        {
            get
            {
                return jake_info_power_level(dev);
            }
        }

        // Initialisation functions
        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_init_device")]
        private static extern IntPtr jake_init_device(int com_port);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_init_device_rfcomm_i64")]
        private static extern IntPtr jake_init_device_rfcomm_i64(Int64 btaddr);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_init_device_rfcomm_str")]
        private static extern IntPtr jake_init_device_rfcomm_str(string btaddr);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_free_device")]
        private static extern int jake_free_device(IntPtr dev);

        // Information functions
        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_info_firmware_revision")]
        private static extern double jake_info_firmware_revision(IntPtr dev);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_info_hardware_revision")]
        private static extern double jake_info_hardware_revision(IntPtr dev);

        // TODO sort this on CE
        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_info_serial_number")]
        private static extern string jake_info_serial_number(IntPtr dev);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_info_rssi")]
        private static extern int jake_info_rssi(IntPtr dev);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_info_power_source")]
        private static extern int jake_info_power_source(IntPtr dev);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_info_power_level")]
        private static extern int jake_info_power_level(IntPtr dev);

        // Data access functions
        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_accx")]
        private static extern int jake_accx(IntPtr dev);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_accy")]
        private static extern int jake_accy(IntPtr dev);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_accz")]
        private static extern int jake_accz(IntPtr dev);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_acc")]
        private static extern int jake_acc(IntPtr dev, int[] xyz);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_magx")]
        private static extern int jake_magx(IntPtr dev);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_magy")]
        private static extern int jake_magy(IntPtr dev);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_magz")]
        private static extern int jake_magz(IntPtr dev);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_mag")]
        private static extern int jake_mag(IntPtr dev, int[] xyz);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_heading")]
        private static extern int jake_heading(IntPtr dev);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_data_timestamp")]
        private static extern int jake_data_timestamp(IntPtr dev, int sensor_id);

        // Register functions
        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_read_configuration")]
        private static extern int jake_read_configuration(IntPtr dev, out byte value);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_write_configuration")]
        private static extern int jake_write_configuration(IntPtr dev, byte value);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_read_configuration2")]
        private static extern int jake_read_configuration2(IntPtr dev, out byte value);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_write_configuration2")]
        private static extern int jake_write_configuration2(IntPtr dev, byte value);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_read_acc_offset")]
        private static extern int jake_read_acc_offset(IntPtr dev, int xyz, int range, out short value);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_write_acc_offset")]
        private static extern int jake_write_acc_offset(IntPtr dev, int xyz, int range, short value);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_read_acc_scale")]
        private static extern int jake_read_acc_scale(IntPtr dev, int xyz, int range, out short value);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_write_acc_scale")]
        private static extern int jake_write_acc_scale(IntPtr dev, int xyz, int range, short value);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_read_mag_offset")]
        private static extern int jake_read_mag_offset(IntPtr dev, int range, out short value);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_write_mag_offset")]
        private static extern int jake_write_mag_offset(IntPtr dev, int range, short value);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_read_mag_scale")]
        private static extern int jake_read_mag_scale(IntPtr dev, int range, out short value);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_write_mag_scale")]
        private static extern int jake_write_mag_scale(IntPtr dev, int range, short value);


        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_read_sample_rate")]
        private static extern int jake_read_sample_rate(IntPtr dev, out byte value);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_write_sample_rate")]
        private static extern int jake_write_sample_rate(IntPtr dev, int value);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_read_main")]
        private static extern int jake_read_main(IntPtr dev, int addr, out byte value);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_write_main")]
        private static extern int jake_write_main(IntPtr dev, int addr, byte value);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_read_bluetooth")]
        private static extern int jake_read_bluetooth(IntPtr dev, int addr, out byte value);

        [DllImport("jake_driver.dll", SetLastError = true, EntryPoint = "jake_write_bluetooth")]
        private static extern int jake_write_bluetooth(IntPtr dev, int addr, byte value);

    }
}
