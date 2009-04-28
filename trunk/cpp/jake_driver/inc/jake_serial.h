#ifndef _JAKE_SERIAL_
#define _JAKE_SERIAL_

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

#include "jake_packets.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
/* serial port parameters, only important on Windows which uses
*	a Bluetooth COM port. */
enum jake_serial {
	JAKE_SERIAL_BAUDRATE = 0,
	JAKE_SERIAL_PARITY,
	JAKE_SERIAL_STOP_BITS,
	JAKE_SERIAL_START_BITS,
	JAKE_SERIAL_HWFLOW,
	JAKE_SERIAL_SWFLOW,
};

/*	JAKE serial port parameters. To get a particular parameter, just do
*		int baudrate = jake_serial_params[JAKE_SERIAL_BAUDRATE]
*	etc.. */
static unsigned jake_serial_params[] = {
	115200,		// baud rate
	NOPARITY,	// no parity
	ONESTOPBIT, // 1 stop bit
	1,			// 1 start bit
	1,			// hardware flow control
	0,			// no software flow control
};

/*	++ Windows only ++
*	Opens the indicated serial port for use with a JAKE device.
*	<port> is a pointer to a jake_serial_port structure that will store all the
*			information about the port.
*	<number> is the number of the COM port to open. -1 to autodetect.
*	Returns the <port> pointer on success, NULL on failure. */
jake_serial_port* jake_open_serial(jake_serial_port* port, int number);

/*	Closes the indicated serial port
*	<port is a pointer to a populated jake_serial_port structure.
*	Always returns 1. */
int jake_close_serial(jake_serial_port* port);
#endif /* _WIN32 */

#ifdef __cplusplus
}
#endif

#endif /* _JAKE_SERIAL_ */
