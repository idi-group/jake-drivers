## 	@package jake Python wrapper for the JAKE C driver.
## 	@file jake.py Python wrapper for the JAKE C driver.

import sys, atexit, os

# set the directory used for loading the pyjake.pyd extension
ext_dir = os.path.join(sys.prefix, 'lib\\site-packages\\')
sys.path.append(ext_dir)

# only supports 2.4 and 2.5
if sys.version[:3] == "2.4":
	import pyjake
elif sys.version[:3] == "2.5":
	import imp
	# check if on windows
	if os.name != "posix":
		import pyjake
	else:
		# TODO distutils for this too!
		pyjake = imp.load_dynamic('pyjake', os.getcwd()+'/pyjake.so')
else:
	print "Unsupported Python version: "  + sys.version[:3]
	sys.exit(-1)

# JAKE driver constants

## Global success code for JAKE functions.
JAKE_SUCCESS = 1
## Global error code for JAKE functions.
JAKE_ERROR = -1

# jake register addresses
[   
	JAKE_REG_CONFIG0,
	JAKE_REG_CONFIG1,

	JAKE_REG_ACCX_2G_OFFSET_LSB,
	JAKE_REG_ACCX_2G_OFFSET_MSB,

	JAKE_REG_ACCY_2G_OFFSET_LSB,
	JAKE_REG_ACCY_2G_OFFSET_MSB,

	JAKE_REG_ACCZ_2G_OFFSET_LSB,
	JAKE_REG_ACCZ_2G_OFFSET_MSB,

	JAKE_REG_ACCX_2G_SCALE_LSB,
	JAKE_REG_ACCX_2G_SCALE_MSB,

	JAKE_REG_ACCY_2G_SCALE_LSB,
	JAKE_REG_ACCY_2G_SCALE_MSB,

	JAKE_REG_ACCZ_2G_SCALE_LSB,
	JAKE_REG_ACCZ_2G_SCALE_MSB,

	JAKE_REG_ACCX_6G_OFFSET_LSB,
	JAKE_REG_ACCX_6G_OFFSET_MSB,

	JAKE_REG_ACCY_6G_OFFSET_LSB,
	JAKE_REG_ACCY_6G_OFFSET_MSB,

	JAKE_REG_ACCZ_6G_OFFSET_LSB,
	JAKE_REG_ACCZ_6G_OFFSET_MSB,

	JAKE_REG_ACCX_6G_SCALE_LSB,
	JAKE_REG_ACCX_6G_SCALE_MSB,

	JAKE_REG_ACCY_6G_SCALE_LSB,
	JAKE_REG_ACCY_6G_SCALE_MSB,

	JAKE_REG_ACCZ_6G_SCALE_LSB,
	JAKE_REG_ACCZ_6G_SCALE_MSB,

	JAKE_REG_MAGX_OFFSET_LSB,
	JAKE_REG_MAGX_OFFSET_MSB,

	JAKE_REG_MAGY_OFFSET_LSB,
	JAKE_REG_MAGY_OFFSET_MSB,

	JAKE_REG_MAGZ_OFFSET_LSB,
	JAKE_REG_MAGZ_OFFSET_MSB,

	JAKE_REG_MAGX_SCALE_LSB,
	JAKE_REG_MAGX_SCALE_MSB,

	JAKE_REG_MAGY_SCALE_LSB,
	JAKE_REG_MAGY_2G_SCALE_MSB,

	JAKE_REG_MAGZ_SCALE_LSB,
	JAKE_REG_MAGZ_SCALE_MSB,

	JAKE_REG_FW_MINOR,
	JAKE_REG_FW_MAJOR,

	JAKE_REG_HW_MINOR,
	JAKE_REG_HW_MAJOR,

	JAKE_REG_SERIAL_LSB,
	JAKE_REG_SERIAL_MSB 
] = range(44)

# bits in main config register
[
	JAKE_ACCEL,
	JAKE_MAG,
	JAKE_HEADING,
	JAKE_ACCEL_CALIBRATION,
	JAKE_MAG_CALIBRATION,
	JAKE_ACCEL_FILTER_ORDER16,
	JAKE_MAG_FILTER_ORDER8,
	JAKE_ACCEL_RANGE_6G
] = range(8)

JAKE_OUTPUT_RATE_COUNT = 6

[
	JAKE_SAMPLE_RATE_0,
	JAKE_SAMPLE_RATE_7_5,
	JAKE_SAMPLE_RATE_15,
	JAKE_SAMPLE_RATE_30,
	JAKE_SAMPLE_RATE_60,
	JAKE_SAMPLE_RATE_120
] = range(JAKE_OUTPUT_RATE_COUNT)

[
	JAKE_X_AXIS,
	JAKE_Y_AXIS,
	JAKE_Z_AXIS
] = range(3)

[
	JAKE_ACCEL_RANGE_2G,
	JAKE_ACCEL_RANGE_6G
] = range(2)


# work out platform from os.name
PLAT_WIN32, PLAT_LINUX = range(2)

platform = -1

# 	Linux
if os.name == 'posix':
	platform = PLAT_LINUX
# 	Windows 2000/XP or CE
elif os.name == 'nt' or os.name == 'ce':
	platform = PLAT_WIN32
else:
	raise pyjake_serial_error("Unsupported or unknown platform: " + os.name)

## An instance of this class represents a single JAKE device.
class jake_device:

	## Constructor
	def __init__(self):
		self.__jakedev = -1;
		self.__lasterror = ""
		self.__connected = False

	##	Attempts to open a connection to a JAKE device
	#
	# 	@param p should be a COM port number on Windows (integer type) or a Bluetooth address in string format
	# 		("12:34:56:78:90") on Linux
	# 
	# 	@return True on success, False on failure. If False, use last_error() to obtain a string indicating the problem.
	def connect(self, p):
		ti = type(123)
		ts = type("123")

		if self.__connected:
			self.__lasterror = "Already connected to a JAKE!"
			return False

		if p == None:
		 	self.__lasterror = "Must provide either COM port (int) or Bluetooth address (string)"
			return False

		if type(p) != ti and type(p) != ts:
			self.__lasterror = "Unknown parameter type (expected int or string)"
			return False
	
		if type(p) != ti and platform == PLAT_WIN32:
			self.__lasterror = "Error: must give COM port as parameter on Windows platforms!"
			return False
		elif type(p) != ts and (platform == PLAT_LINUX):
			self.__lasterror = "Error: must give Bluetooth address as parameter on Linux platforms!"
			return False

		# pass the parameter onto pyjake.init_device
		self.__jakedev = pyjake.init_device(p)

		if self.__jakedev == -1:
			self.__lasterror = "Failed to connect"
			return False

		self.__connected = True
		return True

	## 	Attempts to open a connection to a JAKE device
	#
	# 	@param btaddr should be a Bluetooth address in integer or string format. Note that on Windows this will
	# 		only work if you're using the Bluetooth stack provided by Microsoft. For other stacks, use the 
	# 		connect() function and a COM port number.
	# 		If the address is in integer format, it should correspond directly to the target device Bluetooth address
	# 			eg 00:11:22:33:44:55 could be given as 0x001122334455
	# 		If the address is in string format, any of the following formats should work (note that a-f characters must be lowercase)
	# 			"00:aa:bb:cc:dd:ee:ff" OR "00aabbccddeeff"
	#
	# 	@return True on success, False on failure. If False, use last_error() to obtain a string indicating the problem.
	def connect_rfcomm(self, btaddr):
		ti = type(123L)
		ts = type("123")

		if type(btaddr) == ti:
			self.__jakedev = pyjake.init_device_rfcomm_i64(btaddr)
		else:
			self.__jakedev = pyjake.init_device_rfcomm_str(btaddr)

		if self.__jakedev == -1:
			self.__lasterror = "Failed to connect"
			return False

		self.__connected = True
		return True

	## 	Attempts to open a "debugging connection" to a JAKE device. This simply means that instead of making a Bluetooth
	# 	connection, the driver will instead read data from the file given as the first parameter, and output any commands
	# 	into the file given as the second parameter. 
	#
	# 	@param inp a file that the driver should read data from. This should contain valid JAKE data in ASCII or binary format. When
	# 		the end of the file is reached the reading position will be reset back to the start. 
	# 	@param outp a file that the driver should output data into (eg commands that would normally be sent to the JAKE). This
	# 		file will be overwritten each time the function is called. 
	#
	# 	@return True on success, False on failure. If False, use last_error() to obtain a string indicating the problem.
	def connect_debug(self, inp, outp):
		self.__jakedev = pyjake.init_device_debug(inp, outp)
		if self.__jakedev == -1:
			self.__lasterror = "Failed to connect"
			return False

		self.__connected = True
		return True
	
	## 	Retrieves the last error message produced by the connect() function.
	#
	# 	@return a string containing the message
	def last_error(self):
		return self.__lasterror

	## 	Closes the connection associated with this instance of the class
	#
	# 	@return True on success, False on failure
	def close(self):
		if not self.__connected:
			self.__lasterror = 'Not connected'
			return False

		self.__connected = False
		pyjake.free_device(self.__jakedev)
		self.__jakedev = -1
		return True

	## 	Indicates if connection is currently active.
	# 	@return True if connection active, False if not.
	def isconnected(self):
		return self.__connected

	##	Gets firmware revision of connected device.
	# 	@return 0.0 on error, else firmware version (x.yz format)
	def info_firmware_revision(self):
		if not self.__connected:
			return 0.0
		return pyjake.info_firmware_revision(self.__jakedev)

	##	Gets hardware revision of connected device.
	# 	@return 0.0 on error, else hardware version (x.yz format)
	def info_hardware_revision(self):
		if not self.__connected:
			return 0.0
		return pyjake.info_hardware_revision(self.__jakedev)

	## 	Get serial number of connected device.
	# 	@return empty string on error, else serial number in format: "SK6-xxxx" (eg "SK6-0003")
	def info_serial_number(self):
		if not self.__connected:
			return ""
		return pyjake.info_serial_number(self.__jakedev)

	## 	Get x-axis accelerometer reading
	# 	@return current x-axis accelerometer reading
	def accx(self):
		if not self.__connected:
			return JAKE_ERROR
		return pyjake.accx(self.__jakedev)

	## 	Get y-axis accelerometer reading
	# 	@return current y-axis accelerometer reading
	def accy(self):
		if not self.__connected:
			return JAKE_ERROR
		return pyjake.accy(self.__jakedev)

	## 	Get z-axis accelerometer reading
	# 	@return current z-axis accelerometer reading
	def accz(self):
		if not self.__connected:
			return JAKE_ERROR
		return pyjake.accz(self.__jakedev)

	## 	Get current readings from all 3 accelerometer axes
	# 	@return empty list on error, else a 3 element list with [x, y, z] axis readings
	def acc(self):
		if not self.__connected:
			return []
		return pyjake.acc(self.__jakedev)
			
	## 	Get x-axis magnetometer reading
	# 	@return current x-axis magnetometer reading
	def magx(self):
		if not self.__connected:
			return JAKE_ERROR
		return pyjake.magx(self.__jakedev)

	## 	Get y-axis gyro reading
	# 	@return current y-axis gyro reading
	def magy(self):
		if not self.__connected:
			return JAKE_ERROR
		return pyjake.magy(self.__jakedev)

	## 	Get z-axis magnetometer reading
	# 	@return current z-axis magnetometer reading
	def magz(self):
		if not self.__connected:
			return JAKE_ERROR
		return pyjake.magz(self.__jakedev)

	## 	Get current readings from all 3 magnetometer axes
	# 	@return empty list on error, else a 3 element list with [x, y, z] axis readings
	def mag(self):
		if not self.__connected:
			return []
		return pyjake.mag(self.__jakedev)

	## 	Get current compass heading
	# 	@return current compass heading (0-3599, tenths of a degree)
	def heading(self):
		if not self.__connected:
			return JAKE_ERROR
		return pyjake.heading(self.__jakedev)

	## 	Used to obtain the sequence number for the last set of data retrieved for a particular sensor
	# 	For example, if you call acc() to retrieve a set of acc data, then call data_timestamp(JAKE_SENSOR_ACC)
	# 	the return value will be the sequence number that was attached to the packet that provided the readings
	# 	returned by the acc() function. 
	#
	# 	@param sensor one of the JAKE_SENSOR_ constants
	#
	# 	@return JAKE_ERROR or the sequence number for the selected sensor
	def data_timestamp(self, sensor):
		if not self.__connected:
			return JAKE_ERROR
		return pyjake.data_timestamp(self.__jakedev, sensor)

	## 	Reads main register <address> and returns a 2-tuple: (success/error, value), where
	# 	the first item in the tuple is JAKE_SUCCESS or JAKE_ERROR depending on the
	# 	result of the operation. value will be 0 on error, otherwise it will be the
	# 	current contents of the specified register.
	#
	# 	@param address the address of the main register to read
	#
	# 	@return a 2-tuple containing a success/error value (JAKE_SUCCESS/JAKE_ERROR) and the contents
	# 		of the selected register. If the first value is JAKE_ERROR, the second value is always 0. 
	def read_main(self, address):
		if not self.__connected:
			return (JAKE_ERROR, 0)
		return pyjake.read_main(self.__jakedev, address)

	## 	Writes <value> into main register <address> and returns either JAKE_ERROR or
	# 	JAKE_SUCCESS to indicate the result.
	#
	# 	@param address the address of the main register to write into
	# 	@param value the value to write into the register
	#
	# 	@return JAKE_SUCCESS or JAKE_ERROR
	def write_main(self, address, value):
		if not self.__connected:
			return JAKE_ERROR
		return pyjake.write_main(self.__jakedev, address, value)

	## 	Reads Bluetooth register <address> and returns a 2-tuple: (success/error, value), where
	# 	the first item in the tuple is JAKE_SUCCESS or JAKE_ERROR depending on the
	# 	result of the operation. value will be 0 on error, otherwise it will be the
	# 	current contents of the specified register.
	#
	# 	@param address the address of the Bluetooth register to read
	#
	# 	@return a 2-tuple containing a success/error value (JAKE_SUCCESS/JAKE_ERROR) and the contents
	# 		of the selected register. If the first value is JAKE_ERROR, the second value is always 0. 
	def read_bluetooth(self, address):
		if not self.__connected:
			return (JAKE_ERROR, 0)
		return pyjake.read_bluetooth(self.__jakedev, address)

	## 	Writes <value> into Bluetooth register <address> and returns either JAKE_ERROR or
	# 	JAKE_SUCCESS to indicate the result.
	#
	# 	@param address the address of the Bluetooth register to write into
	# 	@param value the value to write into the register
	#
	# 	@return JAKE_SUCCESS or JAKE_ERROR
	def write_bluetooth(self, address, value):
		if not self.__connected:
			return JAKE_ERROR
		return pyjake.write_bluetooth(self.__jakedev, address, value)

	def wait_for_acks(self, wait_for_ack):
		if not self.__connected:
			return JAKE_ERROR

		pyjake.wait_for_acks(self.__jakedev, wait_for_ack)
		return JAKE_SUCCESS

	def read_configuration(self):
		if not self.__connected:
			return (JAKE_ERROR, 0)

		return pyjake.read_main(self.__jakedev, JAKE_REG_CONFIG0)

	def write_configuration(self, value):
		if not self.__connected:
			return JAKE_ERROR

		return pyjake.write_main(self.__jakedev, JAKE_REG_CONFIG0, value)

	def read_configuration2(self):
		if not self.__connected:
			return (JAKE_ERROR, 0)

		return pyjake.read_main(self.__jakedev, JAKE_REG_CONFIG1)

	def write_configuration2(self, value):
		if not self.__connected:
			return JAKE_ERROR

		return pyjake.write_main(self.__jakedev, JAKE_REG_CONFIG1, value)

	# acc offset
	def read_acc_offset(self, xyz, range):
		if not self.__connected:
			return (JAKE_ERROR, 0)

		return pyjake.read_acc_offset(self.__jakedev, xyz, range, value)

	def write_acc_offset(self, xyz, range, value):
		if not self.__connected:
			return JAKE_ERROR

		return pyjake.write_acc_offset(self.__jakedev, xyz, range, value)

	# acc scale
	def read_acc_scale(self, xyz, range):
		if not self.__connected:
			return (JAKE_ERROR, 0)

		return pyjake.read_acc_scale(self.__jakedev, xyz, range, value)

	def write_acc_scale(self, xyz, range, value):
		if not self.__connected:
			return JAKE_ERROR

		return pyjake.write_acc_scale(self.__jakedev, xyz, range, value)

	# mag offset
	def read_mag_offset(self, xyz):
		if not self.__connected:
			return (JAKE_ERROR, 0)

		return pyjake.read_mag_offset(self.__jakedev, xyz, value)

	def write_mag_offset(self, xyz, value):
		if not self.__connected:
			return JAKE_ERROR

		return pyjake.write_mag_offset(self.__jakedev, xyz, value)

	# mag scale
	def read_mag_scale(self, xyz, range):
		if not self.__connected:
			return (JAKE_ERROR, 0)

		return pyjake.read_mag_scale(self.__jakedev, xyz, value)

	def write_mag_scale(self, xyz, range, value):
		if not self.__connected:
			return JAKE_ERROR

		return pyjake.write_mag_scale(self.__jakedev, xyz, value)

	# sample rate
	def read_sample_rate(self):
		if not self.__connected:
			return (JAKE_ERROR, 0)

		return pyjake.read_sample_rate(self.__jakedev)

	def write_sample_rate(self, newrate):
		if not self.__connected:
			return JAKE_ERROR

		return pyjake.write_sample_rate(self.__jakedev, newrate)
