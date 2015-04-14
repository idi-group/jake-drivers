#ifndef _JAKE_THREAD_H_
#define _JAKE_THREAD_H_

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

/*	Windows threading stuff. Handle to the thread itself, and to the object
*	used for signalling ack packets have arrived */
typedef struct {
	HANDLE rthread;		// handle to thread used to read from serial port
	HANDLE cmd_event;	// handle to an event object used to signal arrival of ACK/NAK packets
} jake_thread;

#define JAKE_THREAD_FUNC LPTHREAD_START_ROUTINE

#else

/*	Linux threading stuff. Variables for the thread itself, a condition object for 
*	signalling, and a mutex used to prevent race conditions when locking the condition
*	object */
typedef struct {
	pthread_t rthread;		// handle to thread used to read from serial port
	pthread_cond_t cmd_event;	// object used to to signal arrival of ACK/NAK packets
	pthread_mutex_t cmd_mutex;	// pthread mutex (used with pthread condition object)
} jake_thread;

typedef void*(*JAKE_THREAD_FUNC)(void*);

#endif	/* _WIN32 */

#ifndef CMD_THREAD
	#define CMD_THREAD 1
#endif

// initialisation
jake_thread* jake_thread_init(jake_thread* st, JAKE_THREAD_FUNC cmdfunc, void* cmdparam, TCHAR* cmdeventname);
// waiting for event
BOOL jake_thread_wait(jake_thread* st, int secs, int ms, int thread);
// signalling event
BOOL jake_thread_signal(jake_thread* st, int thread);
// freeing up resources
BOOL jake_thread_free(jake_thread* st);

#endif 
