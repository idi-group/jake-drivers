#ifndef _JAKE_BTDEFS_H_
#define _JAKE_BTDEFS_H_

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

#undef JAKE_RFCOMM_SUPPORTED

#ifdef _WIN32
#include <winsock2.h>
#endif

// if on recent WinCE version, include proper headers and define macro to enable RFCOMM support
#ifdef _WIN32_WCE
	#if _WIN32_WCE >= 0x420
		#define SOCKADDR_BTH SOCKADDR_BT
		#define JAKE_RFCOMM_SUPPORTED 1
		#include <winsock2.h>
		#include <ws2bth.h>
		#include <bthapi.h>
		#include <bthutil.h>
	#endif
#endif

// if on Win32 and not CE, can include headers and define macro
#ifdef _WIN32
	#ifndef _WIN32_WCE
		#define JAKE_RFCOMM_SUPPORTED 1
		#include "initguid.h"
		#include "ws2bth.h"
	#endif
#endif

// if not on Windows, assume Linux and just define macro
#ifndef _WIN32
	#define JAKE_RFCOMM_SUPPORTED 1
#endif

#endif
