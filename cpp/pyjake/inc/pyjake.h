#ifndef _PYJAKE_H_
#define _PYJAKE_H_

/*
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

#ifdef _WIN32
#ifdef PYJAKE_COMPILE
#define PYJAKE_EXPORT __declspec(dllexport)
#else
#define PYJAKE_EXPORT __declspec(dllimport)
#endif
#else
#define PYJAKE_EXPORT
#endif

/*	Python wrapper for the JAKE driver 
*	Requires Python 2.4+.
*/
#ifdef __cplusplus
extern "C" {
	PYJAKE_EXPORT void initpyjake(void);
}
#endif

static PyObject* pyjake_init_device(PyObject* self, PyObject* args);
static PyObject* pyjake_init_device_rfcomm_i64(PyObject* self, PyObject* args);
static PyObject* pyjake_init_device_rfcomm_str(PyObject* self, PyObject* args);
static PyObject* pyjake_init_device_debug(PyObject* self, PyObject* args);
static PyObject* pyjake_free_device(PyObject* self, PyObject* args);

static PyObject* pyjake_info_firmware_revision(PyObject* self, PyObject* args);
static PyObject* pyjake_info_hardware_revision(PyObject* self, PyObject* args);
static PyObject* pyjake_info_serial_number(PyObject* self, PyObject* args);

static PyObject* pyjake_accx(PyObject* self, PyObject* args);
static PyObject* pyjake_accy(PyObject* self, PyObject* args);
static PyObject* pyjake_accz(PyObject* self, PyObject* args);
static PyObject* pyjake_acc(PyObject* self, PyObject* args);

static PyObject* pyjake_magx(PyObject* self, PyObject* args);
static PyObject* pyjake_magy(PyObject* self, PyObject* args);
static PyObject* pyjake_magz(PyObject* self, PyObject* args);
static PyObject* pyjake_mag(PyObject* self, PyObject* args);

static PyObject* pyjake_heading(PyObject* self, PyObject* args);

static PyObject* pyjake_read_main(PyObject* self, PyObject* args);
static PyObject* pyjake_write_main(PyObject* self, PyObject* args);
static PyObject* pyjake_read_bluetooth(PyObject* self, PyObject* args);
static PyObject* pyjake_write_bluetooth(PyObject* self, PyObject* args);

static PyObject* pyjake_read_acc_offset(PyObject* self, PyObject* args);
static PyObject* pyjake_write_acc_offset(PyObject* self, PyObject* args);

static PyObject* pyjake_read_acc_scale(PyObject* self, PyObject* args);
static PyObject* pyjake_write_acc_scale(PyObject* self, PyObject* args);

static PyObject* pyjake_read_mag_scale(PyObject* self, PyObject* args);
static PyObject* pyjake_write_mag_scale(PyObject* self, PyObject* args);

static PyObject* pyjake_read_mag_offset(PyObject* self, PyObject* args);
static PyObject* pyjake_write_mag_offset(PyObject* self, PyObject* args);

static PyObject* pyjake_read_sample_rate(PyObject* self, PyObject* args);
static PyObject* pyjake_write_sample_rate(PyObject* self, PyObject* args);

static PyObject* pyjake_data_timestamp(PyObject* self, PyObject* args);

static PyObject* pyjake_wait_for_acks(PyObject* self, PyObject* args);

static PyObject* pyjake_cleanup(PyObject* self, PyObject* args);

static PyMethodDef pyjake_methods[] = {
	{ "init_device",	pyjake_init_device,		1,	"Initialise a JAKE connection" },
	{ "init_device_rfcomm_i64",	pyjake_init_device_rfcomm_i64,		1,	"Initialise a JAKE connection over RFCOMM" },
	{ "init_device_rfcomm_str",	pyjake_init_device_rfcomm_str,		1,	"Initialise a JAKE connection over RFCOMM" },
	{ "init_device_debug", pyjake_init_device_debug, 1, "initialise a JAKE connection to a file (debugging)" },
	{ "free_device",	pyjake_free_device,		1,	"Close a JAKE connection" },

	{ "info_firmware_revision", pyjake_info_firmware_revision, 1, "Firmware revision" },
	{ "info_hardware_revision", pyjake_info_hardware_revision, 1, "Hardware revision" },
	{ "info_serial_number", pyjake_info_serial_number, 1, "Serial number" },

	{ "accx",			pyjake_accx,				1,	"x-axis acceleration" },
	{ "accy",			pyjake_accy,				1,	"y-axis acceleration" },
	{ "accz",			pyjake_accz,				1,	"z-axis acceleration" },
	{ "acc",			pyjake_acc,				1,	"x,y,z-axis acceleration" },

	{ "magx",			pyjake_magx,				1,	"x-axis magnetometer" },
	{ "magy",			pyjake_magy,				1,	"y-axis magnetometer" },
	{ "magz",			pyjake_magz,				1,	"z-axis magnetometer" },
	{ "mag",			pyjake_mag,				1,	"x,y,z-axis magnetometer" },

	{ "heading",		pyjake_heading,			1,	"heading" },

	{ "read_main",			pyjake_read_main,				1,	"read a register" },
	{ "write_main",			pyjake_write_main,				1,	"write a register" },

	{ "read_bluetooth",		pyjake_read_bluetooth,				1,	"read a register" },
	{ "write_bluetooth",	pyjake_write_bluetooth,				1,	"write a register" },

	{ "read_sample_rate",	pyjake_read_sample_rate,		1, "read sample rate" },
	{ "write_sample_rate",	pyjake_write_sample_rate,		1,	"write sample rate" },

	{ "read_acc_offset",	pyjake_read_acc_offset,		1, "read acc offset" },
	{ "write_acc_offset",	pyjake_write_acc_offset,		1,	"write acc offset" },

	{ "read_acc_scale",		pyjake_read_acc_scale,		1, "read acc scale" },
	{ "write_acc_scale",	pyjake_write_acc_scale,		1,	"write acc scale" },

	{ "read_mag_offset",	pyjake_read_mag_offset,		1, "read mag offset" },
	{ "write_mag_offset",	pyjake_write_mag_offset,		1,	"write mag offset" },

	{ "read_mag_scale",		pyjake_read_mag_scale,		1, "read mag scale" },
	{ "write_mag_scale",	pyjake_write_mag_scale,		1,	"write mag scale" },


	{ "data_timestamp", pyjake_data_timestamp,				1, "data timestamp" },

	{ "wait_for_acks", pyjake_wait_for_acks, 1, "enable/disable waiting for acks"},

	{ "cleanup", 		pyjake_cleanup, 			1, "clean up on exit" },

	{ NULL, NULL, 0, NULL }
};

#endif
