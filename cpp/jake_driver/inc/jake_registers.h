/** @file shake_registers.h
*	Contains JAKE register definitions. 
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

#ifndef _JAKE_REGISTERS_
#define _JAKE_REGISTERS_

/** bits in the main JAKE configuration register */
enum jake_config0 {
	JAKE_ACCEL,						/** accelerometer on/off */
	JAKE_MAG,						/** magnetometer on/off */
	JAKE_HEADING,					/** heading on/off */
	JAKE_ACCEL_CALIBRATION,			/** accelerometer calibration on/off */
	JAKE_MAG_CALIBRATION,			/** magnetometer calibration on/off */
	JAKE_ACCEL_FILTER_ORDER_8_16,	/** accelerometer filter order 8 (unset) or 16 (set) */
	JAKE_MAG_FILTER_ORDER_4_8,		/**	magnetometer filter order 4 (unset) or 8 (set) */
	JAKE_ACCEL_RANGE_2G_6G,			/** accelerometer range 2g (unset) or 6g (set) */
};

enum jake_power_sources {
	JAKE_CHARGING_USB,
	JAKE_BATTERY
};

/**	Number of sample rates supported */
#define JAKE_OUTPUT_RATE_COUNT 6

/** symbolic sample rate values */
enum jake_sample_rates {
	JAKE_SAMPLE_RATE_0,			/** JAKE sample rate is 0Hz */
	JAKE_SAMPLE_RATE_7_5,		/** JAKE sample rate is 7.5Hz */
	JAKE_SAMPLE_RATE_15,		/** JAKE sample rate is 15Hz */
	JAKE_SAMPLE_RATE_30,		/** JAKE sample rate is 30Hz */
	JAKE_SAMPLE_RATE_60,		/** JAKE sample rate is 60Hz */
	JAKE_SAMPLE_RATE_120,		/** JAKE sample rate is 120Hz */
};

/**	actual JAKE sample rates in Hz */
static float jake_output_rates[] = { 0, 7.5, 15, 30, 60, 120 };

/**	JAKE axes, used in various register read/write functions (eg jake_read_acc_offset(), jake_write_acc_offset() ) */
enum jake_axes {
	JAKE_X_AXIS,
	JAKE_Y_AXIS,
	JAKE_Z_AXIS
};

/**	JAKE accelerometer ranges */
enum jake_accel_ranges {
	JAKE_ACCEL_RANGE_2G,
	JAKE_ACCEL_RANGE_6G,
};

/*	the rest of this file lists the actual register addresses on the device. You don't typically
*	need to bother with these unless you're using the basic shake_read/shake_write register
*	access functions */
enum jake_registers {
	JAKE_REG_CONFIG0 = 0x00,
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
	JAKE_REG_SERIAL_MSB,
};


#endif
