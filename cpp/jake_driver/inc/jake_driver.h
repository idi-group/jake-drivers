/**
*	@file jake_driver.h
*	Main JAKE header
*
*	Copyright (c) 2006-2009, University of Glasgow
*	All rights reserved.
*
*	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*
*		* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*		* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
*			in the documentation and/or other materials provided with the distribution.
*		* Neither the name of the University of Glasgow nor the names of its contributors may be used to endorse or promote products derived 
*			from this software without specific prior written permission.
*
*	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
*	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
*	BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
*	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
*	LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _JAKE_HEADER_
#define _JAKE_HEADER_

#include "jake_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/**	Global success code for JAKE functions. */
#define JAKE_SUCCESS 	 1

/** Global error code for JAKE functions. */
#define JAKE_ERROR 	 -1

/** Instances of this structure represent a connection to a single JAKE device. 
*	A succesful call to jake_init_device() will return a pointer to an instance of this structure. 
*	The application using the driver then has to pass this pointer as the first parameter when calling 
*	other JAKE functions */
typedef struct {
	/** device ID number. */
	int handle; 		
	/** Pointer to internal data. */
	void* priv;			
} jake_device;

#include "jake_registers.h"

/*	=== Startup/shutdown functions === */

// NOTE:
//	valid forms for bluetooth address string in rfcomm funcs:
//		1. "aabbccddeeff"
//		2. "0xaabbccddeeff"
//		3. "aa:bb:cc:dd:ee:ff"
//	ALL LETTERS MUST BE LOWER CASE

#ifdef _WIN32
/**	(Windows only) Creates a connection to a JAKE device.
*	This function attempts to create a Bluetooth connection to a JAKE device. Typically on a 
*	Windows system you cannot specify a Bluetooth name or hardware address, so the only parameter
*	required is the outgoing COM port number to connect to the device through (see JAKE user manual)
*	@param com_port The COM port number (accepted range is -1 to autodetect, else 1-100)
*	
*	@return NULL on failure, otherwise a pointer to a jake_device structure */
JAKE_API jake_device* jake_init_device(int com_port);

/**	(Windows XP SP2 / Windows Mobile 5) Creates a connection to a JAKE device.
*	Unlike the jake_init_device() function, this function uses the Microsoft Bluetooth API to
*	open an RFCOMM connection to a specific JAKE device, currently specified by a numeric 
*	Bluetooth address. 
*	This function will not work if you are using non-Microsoft Bluetooth drivers!
*	@param btaddr Bluetooth MAC address of the target JAKE
*	
*	@return NULL on failure, otherwise a pointer to a jake_device structure */

JAKE_API jake_device* jake_init_device_rfcomm_i64(long long btaddr);
JAKE_API jake_device* jake_init_device_rfcomm_str(char* btaddr);

#else
/*	(Linux only)
*	Main initialisation function for setting up a link to a JAKE device.
*	<btaddr> is the Bluetooth MAC address of the device to connect to, 
*	given in string format (eg "12:34:56:78:90:ab")
*	
*	Returns NULL on failure, otherwise a pointer to a jake_device 
*	structure which can then be passed to the functions below. */
JAKE_API jake_device* jake_init_device_rfcomm_i64(long long btaddr);
JAKE_API jake_device* jake_init_device_rfcomm_str(char* btaddr);
#endif

/**	For debugging use. Sets up the driver to read data from readfile and send any output to writefile. */
JAKE_API jake_device* jake_init_device_DEBUGFILE(char* readfile, char* writefile);


/**
*	Close the link with a JAKE device and free up any resources used in maintaining the connection.
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*
*	@return JAKE_SUCCESS on success, JAKE_ERROR on failure. */
JAKE_API int jake_free_device(jake_device* dev);

/* === JAKE miscellaneous functions === */

/**	Query JAKE firmware revision number.
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@return JAKE firmware revision number (format is x.yz), or 0.0 on error. */
JAKE_API float jake_info_firmware_revision(jake_device* dev);

/**	Query JAKE hardware revision number.
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@return JAKE hardware revision number (format is x.yz), or 0.0 on error. */
JAKE_API float jake_info_hardware_revision(jake_device* dev);

/**	Query JAKE serial number.
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@return pointer to a null terminated string containing the JAKE serial number. Format is
*	JAKE-xxxx, eg JAKE-0003. Do not modify or attempt to deallocate the returned string. 
*	On error, NULL is returned. */
JAKE_API char* jake_info_serial_number(jake_device* dev);

/**	Returns the RSSI value from the last received data packet.
*	RSSI = Receive Signal Strength Indicator, units of dBm.
*	This can be used to monitor the strength of the Bluetooth connection. 
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@return most recent RSSI value in units of dBm (note that the value is usually negative) */
JAKE_API char jake_info_rssi(jake_device* dev);

/**	Indicates the current power source of the JAKE
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@return JAKE_CHARGING_USB if currently powered from USB connection, JAKE_BATTERY otherwise. */
JAKE_API int jake_info_power_source(jake_device* dev);

/**	Indicates the current battery level of the JAKE
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@return percentage of charge remaining in the battery */
JAKE_API int jake_info_power_level(jake_device* dev);

/**	Returns the total number of valid packets (of all types) received so far.
*	@param dev pointer to a jake_device structure as returned by jake_init_device()
*	@return number of packets received */
JAKE_API JAKE_INT64 jake_info_received_packets(jake_device* dev);

/**	Allows you to control whether the driver will wait for acknowledgement packets when sending
*	commands to the JAKE. The default is to enable this functionality as it can be used to confirm
*	whether a command succeeded or not. Register read commands will always wait for acknowledgements
*	since the acknowledgement contains the contents of the requested register.
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@param wait_for_ack zero to disable waiting for ACKs, nonzero to enable 
*
*	@return */
JAKE_API void jake_wait_for_acks(jake_device* dev, int wait_for_ack);

/* === JAKE data access functions === 
*	Each of these returns one or more sensor
*	readings from the given JAKE device. Every function takes a pointer to
*	a jake_device structure as returned by jake_init_device. */

/**	Read current x-axis accelerometer value.
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@return current x-axis accelerometer value */
JAKE_API int jake_accx(jake_device* dev);

/**	Read current y-axis accelerometer value.
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@return current y-axis accelerometer value */
JAKE_API int jake_accy(jake_device* dev);

/**	Read current z-axis accelerometer value.
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@return current z-axis accelerometer value */
JAKE_API int jake_accz(jake_device* dev);

/**	Read current accelerometer values.
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@param xyz pointer to a 3 element array of integers, into which the 3 accelerometer readings will
*	be placed (in x,y,z order).
*	@return JAKE_SUCCESS, or JAKE_ERROR if either parameter is NULL */
JAKE_API int jake_acc(jake_device* dev, int* xyz);

/**	Read current x-axis magnetometer value.
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@return current x-axis magnetometer value */
JAKE_API int jake_magx(jake_device* dev);

/**	Read current y-axis magnetometer value.
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@return current y-axis magnetometer value */
JAKE_API int jake_magy(jake_device* dev);

/**	Read current z-axis magnetometer value.
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@return current z-axis magnetometer value */
JAKE_API int jake_magz(jake_device* dev);

/**	Read current magnetometer values.
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@param xyz pointer to a 3 element array of integers, into which the 3 magnetometer readings will
*	be placed (in x,y,z order).
*	@return JAKE_SUCCESS, or JAKE_ERROR if either parameter is NULL */
JAKE_API int jake_mag(jake_device* dev, int* xyz);

/** Read current compass heading value.
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@return A value are in the range 0-3599 (tenths of a degree), JAKE_ERROR otherwise */
JAKE_API int jake_heading(jake_device* dev);

/**	Each JAKE data packet contains a 16-bit sequence number. This function can be used to retrieve
*	the sequence number from the last received packet. 
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@return the last sequence number for the selected sensor*/
JAKE_API unsigned short jake_data_timestamp(jake_device* dev);

/*	=== Register access functions === 
*	These functions allow you to easily get/set the values of the various configuration registers
*	on a JAKE device */

/*	Reads the main JAKE configuration register.
*
*	The register stores a single byte. The purpose of each bit is as follows (taken from Table 4 in the JAKE manual):
*		Bit 0: accelerometer on/off
*		Bit 1: magnetometer on/off
*		Bit 2: heading on/off
*		Bit 3: accelerometer calibration on/off
*		Bit 4: magnetometer calibration on/off
*		Bit 5: Accelerometer filter order (0 = 8, 1 = 16)
*		Bit 6: Magnetometer filter order (0 = 4, 1 = 8)
*		Bit 7: Acceleration range (0 = 2g, 1 = 6g)
*
*	Default is bits 0-6 set, bit 7 off.
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()	
*	@param value the contents of the register
*
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_read_configuration(jake_device* dev, unsigned char* value);

/*	Write a value into the main JAKE configuration register.
*
*	The register stores a single byte. The purpose of each bit is as follows (taken from Table 4 in the JAKE manual):
*		Bit 0: accelerometer on/off
*		Bit 1: magnetometer on/off
*		Bit 2: heading on/off
*		Bit 3: accelerometer calibration on/off
*		Bit 4: magnetometer calibration on/off
*		Bit 5: Accelerometer filter order (0 = 8, 1 = 16)
*		Bit 6: Magnetometer filter order (0 = 4, 1 = 8)
*		Bit 7: Acceleration range (0 = 2g, 1 = 6g)
*
*	Default is bits 0-6 set, bit 7 off.
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()	
*	@param value the new content of the register
*
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_write_configuration(jake_device* dev, unsigned char value);

/*	Reads the second JAKE configuration register.
*
*	The register stores a single byte. The purpose of each bit is as follows (taken from Table 5 in the JAKE manual):
*		Bit 0-2: output data rate (0 = 0Hz, 1 = 7.5Hz, 2 = 15Hz, 3 = 30Hz, 4 = 60Hz, 5 = 120Hz)
*		Bit 3-6: Not used
*		Bit 7: Set this bit to store current configuration register values in FLASH memory
*
*	Note that the jake_read_sample_rate() function can be used to easily obtain the current sample rate.
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()	
*	@param value the contents of the register
*
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_read_configuration2(jake_device* dev, unsigned char* value);

/*	Write a value into the second JAKE configuration register.
*
*	The register stores a single byte. The purpose of each bit is as follows (taken from Table 5 in the JAKE manual):
*		Bit 0-2: output data rate (0 = 0Hz, 1 = 7.5Hz, 2 = 15Hz, 3 = 30Hz, 4 = 60Hz, 5 = 120Hz)
*		Bit 3-6: Not used
*		Bit 7: Set this bit to store current configuration register values in FLASH memory
*
*	Note that the jake_write_sample_rate() function can be used to easily set the current sample rate.
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()	
*	@param value the new content of the register
*
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_write_configuration2(jake_device* dev, unsigned char value);

/*	Reads the accelerometer offset for a selected axis and range.
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()	
*	@param xyz one of the values from the jake_axes enumeration, eg JAKE_X_AXIS
*	@param range one of the values from the jake_accel_ranges enumeration, eg JAKE_ACCEL_RANGE_2G
*	@param value the accelerometer offset for the selected axis and range of the register
*
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_read_acc_offset(jake_device* dev, int xyz, int range, short* value);

/*	Write the accelerometer offset for a selected axis and range.
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()	
*	@param xyz one of the values from the jake_axes enumeration, eg JAKE_X_AXIS
*	@param range one of the values from the jake_accel_ranges enumeration, eg JAKE_ACCEL_RANGE_2G
*	@param value the new accelerometer offset for the selected axis and range of the register
*
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_write_acc_offset(jake_device* dev, int xyz, int range, short value);

/*	Reads the accelerometer scale for a selected axis and range.
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()	
*	@param xyz one of the values from the jake_axes enumeration, eg JAKE_X_AXIS
*	@param range one of the values from the jake_accel_ranges enumeration, eg JAKE_ACCEL_RANGE_2G
*	@param value the accelerometer scale for the selected axis and range of the register
*
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_read_acc_scale(jake_device* dev, int xyz, int range, short* value);

/*	Write the accelerometer scale for a selected axis and range.
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()	
*	@param xyz one of the values from the jake_axes enumeration, eg JAKE_X_AXIS
*	@param range one of the values from the jake_accel_ranges enumeration, eg JAKE_ACCEL_RANGE_2G
*	@param value the new accelerometer scale for the selected axis and range of the register
*
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_write_acc_scale(jake_device* dev, int xyz, int range, short value);

/*	Reads the magnetometer scale for a selected axis.
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()	
*	@param xyz one of the values from the jake_axes enumeration, eg JAKE_X_AXIS
*	@param value the magnetometer scale for the selected axis and range of the register
*
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_read_mag_scale(jake_device* dev, int xyz, short* value);

/*	Write the magnetometer scale for a selected axis
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()	
*	@param xyz one of the values from the jake_axes enumeration, eg JAKE_X_AXIS
*	@param value the new magnetometer scale for the selected axis and range of the register
*
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_write_mag_scale(jake_device* dev, int xyz, short value);

/*	Reads the magnetometer offset for a selected axis.
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()	
*	@param xyz one of the values from the jake_axes enumeration, eg JAKE_X_AXIS
*	@param value the magnetometer offset for the selected axis and range of the register
*
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_read_mag_offset(jake_device* dev, int xyz, short* value);

/*	Write the accelerometer offset for a selected axis.
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()	
*	@param xyz one of the values from the jake_axes enumeration, eg JAKE_X_AXIS
*	@param value the new magnetometer offset for the selected axis and range of the register
*
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_write_mag_offset(jake_device* dev, int xyz, short value);

/*	Reads the current sample rate
*
*	The value returned in <current_rate> will be an index into the jake_output_rates array,
*	which is declared in jake_registers.h. 
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()	
*	@param current_rate on success will contain the index of the current sample rate in the jake_output_rates array
*
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_read_sample_rate(jake_device* dev, unsigned char* current_rate);

/*	Sets a new sample rate
*
*	The <new_rate> parameter can have any value - it will be adjusted internally to the closest sample
*	rate supported by the JAKE. The possible sample rates are 0, 7.5, 15, 30, 60 and 120Hz.
*
*	For example, values < 0 will result in sample rate of 0, values above 120 will be set to 120.
*	A value of 7 or 8 will be set to 7.5.
*	A value of 40 will be set to 30.
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@param new_rate a new sample rate in Hz 
*
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_write_sample_rate(jake_device* dev, int new_rate);


/* 	=== Basic register access functions === 
*	These functions allow you direct access to the various registers on the device.
*	Usually it's easier to use one of the functions above, which don't involve
*	searching for the correct register address or checking if a register is
*	read/write etc. */

/** Reads one of the JAKE device configuration registers on the main microcontroller and returns its contents.
*
*	This function can be used to read the contents of any valid register (as an alternative to using
*	the other register specific functions).
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@param addr the register address (use one of the NV_REG/VO_REG constants from jake_registers.h)
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_read_main(jake_device* dev, int addr, unsigned char* value);

/** Writes a value into one of the JAKE device configuration registers.
*
*	This function can be used to write a value into any valid register on the main microcontroller
*	(as an alternative to using the other register specific functions).
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@param addr the register address (use one of the NV_REG/VO_REG constants from jake_registers.h)
*	@param value the value that will be placed into the register (note that the range
*		of values is dependent on the selected register, see user manual for details)
* 
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_write_main(jake_device* dev, int addr, unsigned char value);

/** Reads one of the JAKE device configuration registers on the Bluetooth microcontroller and returns its contents.
*
*	This function can be used to read the contents of any valid register (as an alternative to using
*	the other register specific functions).
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@param addr the register address (use one of the NV_REG/VO_REG constants from jake_registers.h)
*	@param value pointer to an unsigned char variable which will receive the contents of the register 
*	
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_read_bluetooth(jake_device* dev, int addr, unsigned char* value);

/** Writes a value into one of the JAKE device configuration registers.
*
*	This function can be used to write a value into any valid register on the Bluetooth microcontroller
*	(as an alternative to using the other register specific functions).
*
*	@param sh pointer to a jake_device structure as returned by jake_init_device()
*	@param addr the register address (use one of the NV_REG/VO_REG constants from jake_registers.h)
*	@param value the value that will be placed into the register (note that the range
*		of values is dependent on the selected register, see user manual for details)
* 
*	@return JAKE_SUCCESS or JAKE_ERROR */
JAKE_API int jake_write_bluetooth(jake_device* dev, int addr, unsigned char value);

#ifdef __cplusplus
}
#endif

#endif /* _JAKE_HEADER_ */
