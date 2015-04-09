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

#undef _DEBUG /* Make sure to link with pythonxx.lib and not pythonxx_d.lib */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#else
#include <stdlib.h>
#endif

#include "jake_driver.h"
#include "jake_btdefs.h"

#ifdef _WIN32
#include "Python.h"
#else
#include <Python.h>
#endif
#include "pyjake.h"

static PyObject* pyjake_ex;

#define MAX_JAKES		8

static jake_device* devicelist[MAX_JAKES];
static int devicelist_count = 0;

static PyObject* pyjake_cleanup(PyObject* self, PyObject* args) {
	int i;

	for(i=0;i<MAX_JAKES;i++) {
		if(devicelist[i] != NULL)
			jake_free_device(devicelist[i]);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

PYJAKE_EXPORT void initpyjake(void) {
	PyObject* mod = Py_InitModule("pyjake", pyjake_methods);
	PyEval_InitThreads();
    char newExceptionName[] = "pyshake.error";
	pyjake_ex = PyErr_NewException(&newExceptionName[0], NULL, NULL);
	Py_INCREF(pyjake_ex);
	PyModule_AddObject(mod, "PyjakeError", pyjake_ex);
}

// arguments: a COM port number or a BT addr 
static PyObject* pyjake_init_device(PyObject* self, PyObject* args) {
	jake_device* dev = NULL;

	if(devicelist_count >= MAX_JAKES) {
		PyRun_SimpleString("print 'Maximum number of JAKES reached!'");
		return Py_BuildValue("i", JAKE_ERROR);
	}

	#ifdef _WIN32
    int com_port, parsedok;
	parsedok = PyArg_ParseTuple(args, "i", &com_port);
	dev = jake_init_device(com_port);
	#endif
	
	if(dev == NULL)
		return Py_BuildValue("i", JAKE_ERROR);

	devicelist[devicelist_count] = dev;
	devicelist_count++;

	return Py_BuildValue("i", devicelist_count - 1);
}

static PyObject* pyjake_init_device_rfcomm_i64(PyObject* self, PyObject* args) {
	#ifdef JAKE_RFCOMM_SUPPORTED
	jake_device* dev;
	PY_LONG_LONG btaddr;

	if(devicelist_count >= MAX_JAKES) {
		PyRun_SimpleString("print 'Maximum number of JAKES reached!'");
		return Py_BuildValue("i", JAKE_ERROR);
	}

	// XXX has to be "L" not "l" despite what API docs say
	PyArg_ParseTuple(args, "L", &btaddr);
	dev = jake_init_device_rfcomm_i64(btaddr);

	if(dev == NULL)
		return Py_BuildValue("i", JAKE_ERROR);

	devicelist[devicelist_count] = dev;
	devicelist_count++;

	return Py_BuildValue("i", devicelist_count - 1);

	#else
	return Py_BuildValue("i", JAKE_ERROR);
	#endif
}

static PyObject* pyjake_init_device_rfcomm_str(PyObject* self, PyObject* args) {
	#ifdef JAKE_RFCOMM_SUPPORTED
	jake_device* dev;
	char* btaddr;

	if(devicelist_count >= MAX_JAKES) {
		PyRun_SimpleString("print 'Maximum number of JAKES reached!'");
		return Py_BuildValue("i", JAKE_ERROR);
	}

	PyArg_ParseTuple(args, "s", &btaddr);
	dev = jake_init_device_rfcomm_str(btaddr);

	if(dev == NULL)
		return Py_BuildValue("i", JAKE_ERROR);

	devicelist[devicelist_count] = dev;
	devicelist_count++;

	return Py_BuildValue("i", devicelist_count - 1);

	#else
	return Py_BuildValue("i", JAKE_ERROR);
	#endif
}

static PyObject* pyjake_init_device_debug(PyObject* self, PyObject* args) {
	jake_device* dev;
	char* inp, *outp;

	if(devicelist_count >= MAX_JAKES) {
		PyRun_SimpleString("print 'Maximum number of JAKES reached!'");
		return Py_BuildValue("i", JAKE_ERROR);
	}

	PyArg_ParseTuple(args, "ss", &inp, &outp);

	printf("Connecting to: %s | %s\n", inp, outp);
	dev = jake_init_device_DEBUGFILE(inp, outp);

	if(dev == NULL)
		return Py_BuildValue("i", JAKE_ERROR);

	devicelist[devicelist_count] = dev;
	devicelist_count++;

	return Py_BuildValue("i", devicelist_count - 1);
}

static PyObject* pyjake_free_device(PyObject* self, PyObject* args) {
	int id, i;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_JAKES)
		return Py_BuildValue("i", JAKE_ERROR);

	jake_free_device(devicelist[id]);

	devicelist[id] = NULL;
	for(i=id;i<MAX_JAKES-1;i++) {
		devicelist[id] = devicelist[id+1];
	}
	devicelist_count--;

	return Py_BuildValue("i", 1);
}

static PyObject* pyjake_info_firmware_revision(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_JAKES)
		return Py_BuildValue("d", 0.0);

	return Py_BuildValue("f", jake_info_firmware_revision(devicelist[id]));
}

static PyObject* pyjake_info_hardware_revision(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_JAKES)
		return Py_BuildValue("d", 0.0);

	return Py_BuildValue("f", jake_info_hardware_revision(devicelist[id]));
}

static PyObject* pyjake_info_serial_number(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_JAKES)
		return Py_BuildValue("s", "");

	return Py_BuildValue("s", jake_info_serial_number(devicelist[id]));
}

static PyObject* pyjake_wait_for_acks(PyObject* self, PyObject* args) {
	int id, wait_for_ack;

	PyArg_ParseTuple(args, "ii", &id, &wait_for_ack);

	if(id < 0 || id >= MAX_JAKES)
		return Py_BuildValue("i", 0);

	jake_wait_for_acks(devicelist[id], wait_for_ack);
	return Py_BuildValue("i", 1);
}

// arguments: 1 int, ID number
static PyObject* pyjake_accx(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_JAKES)
		return Py_BuildValue("i", JAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", jake_accx(devicelist[id]));

	return Py_BuildValue("i", JAKE_ERROR);
}

// arguments: 1 int, ID number
static PyObject* pyjake_accy(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_JAKES)
		return Py_BuildValue("i", JAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", jake_accy(devicelist[id]));

	return Py_BuildValue("i", JAKE_ERROR);
}

// arguments: 1 int, ID number
static PyObject* pyjake_accz(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_JAKES)
		return Py_BuildValue("i", JAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", jake_accz(devicelist[id]));

	return Py_BuildValue("i", JAKE_ERROR);
}

static PyObject* pyjake_acc(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_JAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		int xyz[3];
		PyObject* obj;

		jake_acc(devicelist[id], xyz);
		obj = Py_BuildValue("[i, i, i]", xyz[0], xyz[1], xyz[2]);
		return obj;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyjake_magx(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_JAKES)
		return Py_BuildValue("i", JAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", jake_magx(devicelist[id]));

	return Py_BuildValue("i", JAKE_ERROR);
}

static PyObject* pyjake_magy(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_JAKES)
		return Py_BuildValue("i", JAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", jake_magy(devicelist[id]));

	return Py_BuildValue("i", JAKE_ERROR);
}

static PyObject* pyjake_magz(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_JAKES)
		return Py_BuildValue("i", JAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", jake_magz(devicelist[id]));

	return Py_BuildValue("i", JAKE_ERROR);
}

static PyObject* pyjake_mag(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_JAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		int xyz[3];
		PyObject* obj;

		jake_mag(devicelist[id], xyz);
		obj = Py_BuildValue("[i, i, i]", xyz[0], xyz[1], xyz[2]);
		return obj;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyjake_heading(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_JAKES)
		return Py_BuildValue("i", JAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", jake_heading(devicelist[id]));

	return Py_BuildValue("i", JAKE_ERROR);
}

static PyObject* pyjake_read_main(PyObject* self, PyObject* args) {
	int id, reg;

	PyArg_ParseTuple(args, "ii", &id, &reg);

	if(id < 0 || id >= MAX_JAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		unsigned char value, ret;
		PyObject* obj;

		value = 0;
		ret = jake_read_main(devicelist[id], reg, &value);
		obj = Py_BuildValue("[i, i]", ret, value);
		return obj;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyjake_write_main(PyObject* self, PyObject* args) {
	int id, reg, val;

	PyArg_ParseTuple(args, "iii", &id, &reg, &val);

	if(id < 0 || id >= MAX_JAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		short ret;

		ret = jake_write_main(devicelist[id], reg, val);
		return Py_BuildValue("i", ret);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyjake_read_bluetooth(PyObject* self, PyObject* args) {
	int id, reg;

	PyArg_ParseTuple(args, "ii", &id, &reg);

	if(id < 0 || id >= MAX_JAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		unsigned char value, ret;
		PyObject* obj;

		value = 0;
		ret = jake_read_bluetooth(devicelist[id], reg, &value);
		obj = Py_BuildValue("[i, i]", ret, value);
		return obj;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyjake_write_bluetooth(PyObject* self, PyObject* args) {
	int id, reg, val;

	PyArg_ParseTuple(args, "iii", &id, &reg, &val);

	if(id < 0 || id >= MAX_JAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		short ret;

		ret = jake_write_bluetooth(devicelist[id], reg, val);
		return Py_BuildValue("i", ret);
	}

	Py_INCREF(Py_None);
	return Py_None;
}


static PyObject* pyjake_data_timestamp(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_JAKES)
		return Py_BuildValue("i", JAKE_ERROR);

	if(devicelist[id] != NULL)
		return Py_BuildValue("i", jake_data_timestamp(devicelist[id]));

	return Py_BuildValue("i", JAKE_ERROR);
}

static PyObject* pyjake_read_acc_offset(PyObject* self, PyObject* args) {
	int id, xyz, range;

	PyArg_ParseTuple(args, "iii", &id, &xyz, &range);

	if(id < 0 || id >= MAX_JAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		short value, ret;
		PyObject* obj;

		value = 0;
		ret = jake_read_acc_offset(devicelist[id], xyz, range, &value);
		obj = Py_BuildValue("[i, i]", ret, value);
		return obj;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyjake_write_acc_offset(PyObject* self, PyObject* args) {
	int id, xyz, range, val;

	PyArg_ParseTuple(args, "iiii", &id, &xyz, &range, &val);

	if(id < 0 || id >= MAX_JAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		short ret;

		ret = jake_write_acc_offset(devicelist[id], xyz, range, val);
		return Py_BuildValue("i", ret);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyjake_read_acc_scale(PyObject* self, PyObject* args) {
	int id, xyz, range;

	PyArg_ParseTuple(args, "iii", &id, &xyz, &range);

	if(id < 0 || id >= MAX_JAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		short value, ret;
		PyObject* obj;

		value = 0;
		ret = jake_read_acc_scale(devicelist[id], xyz, range, &value);
		obj = Py_BuildValue("[i, i]", ret, value);
		return obj;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyjake_write_acc_scale(PyObject* self, PyObject* args) {
	int id, xyz, range, val;

	PyArg_ParseTuple(args, "iiii", &id, &xyz, &range, &val);

	if(id < 0 || id >= MAX_JAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		short ret;

		ret = jake_write_acc_scale(devicelist[id], xyz, range, val);
		return Py_BuildValue("i", ret);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyjake_read_mag_scale(PyObject* self, PyObject* args) {
	int id, xyz;

	PyArg_ParseTuple(args, "ii", &id, &xyz);

	if(id < 0 || id >= MAX_JAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		short value, ret;
		PyObject* obj;

		value = 0;
		ret = jake_read_mag_scale(devicelist[id], xyz, &value);
		obj = Py_BuildValue("[i, i]", ret, value);
		return obj;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyjake_write_mag_scale(PyObject* self, PyObject* args) {
	int id, xyz, val;

	PyArg_ParseTuple(args, "iiii", &id, &xyz, &val);

	if(id < 0 || id >= MAX_JAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		short ret;

		ret = jake_write_mag_scale(devicelist[id], xyz, val);
		return Py_BuildValue("i", ret);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyjake_read_mag_offset(PyObject* self, PyObject* args) {
	int id, xyz;

	PyArg_ParseTuple(args, "ii", &id, &xyz);

	if(id < 0 || id >= MAX_JAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		short value, ret;
		PyObject* obj;

		value = 0;
		ret = jake_read_mag_scale(devicelist[id], xyz, &value);
		obj = Py_BuildValue("[i, i]", ret, value);
		return obj;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyjake_write_mag_offset(PyObject* self, PyObject* args) {
	int id, xyz, val;

	PyArg_ParseTuple(args, "iii", &id, &xyz, &val);

	if(id < 0 || id >= MAX_JAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		short ret;

		ret = jake_write_mag_scale(devicelist[id], xyz, val);
		return Py_BuildValue("i", ret);
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyjake_read_sample_rate(PyObject* self, PyObject* args) {
	int id;

	PyArg_ParseTuple(args, "i", &id);

	if(id < 0 || id >= MAX_JAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		unsigned char value, ret;
		PyObject* obj;

		value = 0;
		ret = jake_read_sample_rate(devicelist[id], &value);
		obj = Py_BuildValue("[i, i]", ret, value);
		return obj;
	}

	Py_INCREF(Py_None);
	return Py_None;
}

static PyObject* pyjake_write_sample_rate(PyObject* self, PyObject* args) {
	int id, rate;

	PyArg_ParseTuple(args, "ii", &id, &rate);

	if(id < 0 || id >= MAX_JAKES) {
		Py_INCREF(Py_None);
		return Py_None;
	}

	if(devicelist[id] != NULL) {
		int ret;

		ret = jake_write_sample_rate(devicelist[id], rate);
		return Py_BuildValue("i", ret);
	}

	Py_INCREF(Py_None);
	return Py_None;
}
