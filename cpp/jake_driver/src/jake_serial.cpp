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

#include <stdio.h>

#include "jake_serial.h"
#include "jake_platform.h"

#ifdef WIN32

jake_serial_port* jake_open_serial(jake_serial_port* port, int number) {
	TCHAR portname[24];

	// NOTE: the "COMx" style of device name only seems to work when x < 10
	// but the "\\\\.\\COMx" style works for higher numbers too
	#ifndef _WIN32_WCE
	_stprintf(portname, _T("\\\\.\\COM%d"), number);
	#else
	// on PocketPC/Windows Mobile, port numbers should be under 10 and require the
	// trailing colon. Not sure if the above works on these OSes
	_stprintf(portname, _T("COM%d:"), number);
	#endif

	port->port = CreateFile(portname, GENERIC_READ | GENERIC_WRITE, 0,
							NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(port->port == INVALID_HANDLE_VALUE) {
		JAKE_DBG("CreateFile failed (%d)\n", GetLastError());
		return NULL;
	}

	memset(&(port->dcb), 0, sizeof(DCB));
	memset(&(port->cto), 0, sizeof(COMMTIMEOUTS));

	// MSDN: ReadIntervalTimeout to MAXWORD and set both ReadTotalTimeoutMultiplier 
	// and ReadTotalTimeoutConstant to zero.
	port->cto.ReadIntervalTimeout = MAXDWORD;
	port->cto.ReadTotalTimeoutMultiplier = 0;
	port->cto.ReadTotalTimeoutConstant = 0;
	port->cto.WriteTotalTimeoutConstant = 0;
	port->cto.WriteTotalTimeoutMultiplier = 0;
	/*port->cto.ReadIntervalTimeout = 10000; // time in ms allowed between successive bytes
	port->cto.ReadTotalTimeoutMultiplier = 1; // timeout multiplier
	port->cto.ReadTotalTimeoutConstant = 0; // additional timeout constant
	port->cto.WriteTotalTimeoutConstant = 10000;
	port->cto.WriteTotalTimeoutMultiplier = 1;
	*/

	port->dcb.DCBlength = sizeof(DCB);
	GetCommState(port->port, &(port->dcb));

	port->dcb.BaudRate = jake_serial_params[JAKE_SERIAL_BAUDRATE];
	port->dcb.fBinary = TRUE;
	port->dcb.fParity = FALSE;
	port->dcb.ByteSize = 8;
	port->dcb.Parity = jake_serial_params[JAKE_SERIAL_PARITY];
	port->dcb.StopBits = jake_serial_params[JAKE_SERIAL_STOP_BITS];
	
	SetCommTimeouts(port->port, &(port->cto));
	SetCommState(port->port, &(port->dcb));
	SetupComm(port->port, 10000, 10000);

	return port;
}

int jake_close_serial(jake_serial_port* port) {
	PurgeComm(port->port, PURGE_RXABORT | PURGE_TXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR);

	if(CloseHandle(port->port) == 0) 
		return 0;
	return 1;
}
#endif
