#ifndef _JAKE_PACKETS_
#define _JAKE_PACKETS_

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

#include "jake_platform.h"
#include "jake_thread.h"
#include "jake_registers.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#pragma pack(1)
#define PACKED
#else
#define PACKED __attribute__ ((__packed__))
#endif

struct jake_data_packet {
	char header[4]; // $DAT or $ACK or $NAK
	short acc[3];
	short mag[3];
	short heading;
	char power;
	char rssi;
	char ack;
	char RESERVED;
	unsigned short counter;
} PACKED;
// both the ack and data packets share the same structure
typedef struct jake_data_packet jake_data_packet;
typedef struct jake_data_packet jake_ack_packet;

struct jake_cmd_packet {
	char header[3];
	char address;
	char data;
	char unused;
} PACKED;
typedef struct jake_cmd_packet jake_cmd_packet;

#ifdef _WIN32
#pragma pack()
#else

#endif

/* holds pointer to and data about BT serial port being used */
#ifdef _WIN32
typedef struct {
	HANDLE port;
	DCB dcb;
	COMMTIMEOUTS cto;
} jake_serial_port;
#endif

#include "jake_btdefs.h"

#ifdef JAKE_RFCOMM_SUPPORTED
	#ifdef _WIN32
		typedef struct {
			SOCKET sock;
			SOCKADDR_BTH addr;
		} jake_rfcomm_socket;
	#else
	typedef struct {
		int sock;
	} jake_rfcomm_socket;
	#endif
#endif

typedef struct {
	int comms_type;
	#ifdef _WIN32
	jake_serial_port serial;
	#endif
	#ifdef JAKE_RFCOMM_SUPPORTED
	jake_rfcomm_socket rfcomm;
	#endif
	FILE* dbg_read;
	FILE* dbg_write;
} jake_port;

enum jake_connection_types {
	JAKE_CONN_VIRTUAL_SERIAL = 0,
	JAKE_CONN_RFCOMM_I64,
	JAKE_CONN_RFCOMM_STR,
	JAKE_CONN_DEBUGFILE,
};

typedef struct {
	int type;
	int com_port;
	JAKE_INT64 btaddr;
	char btaddr_str[20];
	char readfile[256];
	char writefile[256];
} jake_conn_data;

/* internal data about a particular JAKE */
typedef struct {
	jake_data_packet data;		// structure containing sensor readings and seq numbers
	jake_port port;				// structure containing port details
	jake_thread thread;			// structure containing threading objects
	BOOL rthread_done;			// indicates when the reader thread should exit
	BOOL rthread_exit;			
	BOOL lastack;				// TRUE if last ack was positive, FALSE if negative
	int lastval;				// Value from last ack received
	BOOL waiting_for_ack;		// TRUE if currently expecting an ACK/NAK, FALSE otherwise
	BOOL waiting_for_ack_signal; 	// TRUE if app is waiting for an ack to arrive, FALSE otherwise
	char serial[20];			// Serial number
	float fwrev;				// Firmware revision
	float hwrev;				// Hardware revision
	BOOL got_fwhwserial;		
	long long data_recv;		// total data received
	int wait_for_acks;
	JAKE_INT64 packets_received;

	unsigned long data_packets;
	unsigned long ack_packets;
	unsigned long nak_packets;
	unsigned long error_packets;
} jake_device_private;

// used to indicate a malformed packet
#define JAKE_BAD_PKT 	 -1

#define JAKE_MAX_PACKET_SIZE 	32
#define JAKE_NUM_PACKET_TYPES	7

#define JAKE_HEADER_LEN		4	

/* used to assign a different numeric ID to each JAKE device on the local machine */
static int jake_handle_count = 0;

/* packet types */
enum jake_packet_types {
	/* data packet first */
	JAKE_DATA,
	
	/* command packets */
	JAKE_CMD_READ, 						// read command to main microcontroller
	JAKE_CMD_BT_READ,					// read command to BT microcontroller

	JAKE_CMD_WRITE, 					// write command to main microcontroller
	JAKE_CMD_BT_WRITE,					// write command to BT microcontroller
	
	/* acknowledgement packets */
	JAKE_ACK_ACK,						// successful ACK
	JAKE_ACK_NEG, 						// negative ACK (ie an error occurred)

};

static char* jake_packet_type_names[] = {
	"JAKE/Data",
	"JAKE/Main read",
	"JAKE/BT read",
	"JAKE/Main write",
	"JAKE/BT write",
	"JAKE/ACK",
	"JAKE/NAK",
};

/* packet headers */
static char* jake_packet_headers[] = {
	"$$$D",		// data ($$$D)
	
	"$$R",		// command; read main microcontroller
	"$$r",		// command; read BT microcontroller

	"$$W",		// command; write main microcontroller
	"$$w",		// command; write BT microcontroller

	"$$$A",		// ack; positive
	"$$$N",		// ack; negative
};


/* packet lengths (bytes) */
static unsigned jake_packet_lengths[] = {
	24,  		// data

	6, 			// command; read main microcontroller
	6,			// command; read BT microcontroller

	6, 			// command; write main microcontroller
	6,			// command; write BT microcontroller

	24,			// ack; positive
	24,  		// ack; negative
};


// extracts the address and value from an ack packet
int jake_parse_ack_packet(jake_ack_packet* scp, jake_device_private* devpriv);

// Attempts to classify the packet in <packet_data> by examining the header.
// <packet_length> should be the length of <packet_data> in bytes. Returns
// JAKE_BAD_PKT if unable to determine the type of the packet or if <packet_length>
// is too short. Otherwise, returns a member of the "packet_types" enumeration
// defined above.
int jake_classify_packet_header(char* header, unsigned header_length);
int jake_update_data(void* shakedev, int type, char* rawpacket);

int dec_ascii_to_int(char* ascii_buf, int buflen, int digits);
int hex_ascii_to_int(char* ascii_buf, int buflen, int digits);

#ifdef __cplusplus
}
#endif

#endif /* _JAKE_PACKETS_ */
