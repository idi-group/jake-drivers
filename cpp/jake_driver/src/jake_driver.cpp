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

#include <stdlib.h>
#include "jake_platform.h"
#include "jake_driver.h"
#include "jake_serial.h"
#ifdef JAKE_RFCOMM_SUPPORTED
	#include "jake_rfcomm.h"
#endif
#include "jake_packets.h"
#include "jake_thread.h"

int read_rfcomm_bytes(jake_device_private* devpriv, char* buf, int bytes_to_read) {
	#ifdef JAKE_RFCOMM_SUPPORTED
	DWORD bytes_read = 0;
	int sleepcounter = 0;
	int remaining_bytes = bytes_to_read;

	while(1) {
		int len = recv(devpriv->port.rfcomm.sock, buf+bytes_read, remaining_bytes, 0);
		if(len == SOCKET_ERROR) {
			break;
		}
		bytes_read += len;
		remaining_bytes -= len;

		if(remaining_bytes == 0)
			break;
		
		/* check if the thread is exiting, and if so break out of the loop and return */
		if(devpriv->rthread_done)
			break;

		/* otherwise just loop back round and read the port again */
		++sleepcounter;
		if(sleepcounter > 10) {
			jake_sleep(1);
			sleepcounter = 0;
		}
		continue;
	}

	devpriv->data_recv += (bytes_read - remaining_bytes);
	return bytes_to_read - remaining_bytes;
	#else
	return 0;
	#endif
}

int write_rfcomm_bytes(jake_device_private* devpriv, char* buf, int bytes_to_write) {
	#ifdef JAKE_RFCOMM_SUPPORTED
	DWORD bytes_written = 0;
	int remaining_bytes = bytes_to_write;

	while(1) {
		int len = send(devpriv->port.rfcomm.sock, buf+bytes_written, remaining_bytes, 0);

		if(len == SOCKET_ERROR) {

			break;
		}

		/* subtract the bytes we just wrote from the total amount we want */
		remaining_bytes -= len;
		bytes_written += len;

		/* if we didn't get them all.. */
		if(remaining_bytes != 0) {
			/* loop back round and write the port again */
			continue;
		} else
			/* if all bytes have been written, break out of loop and return */
			break;
	}

	return bytes_to_write - remaining_bytes;
	#else
	return 0;
	#endif
}

/*	utility func, reads <bytes_to_read> bytes from the port associated with <devpriv> 
*	into <buf>, handling any timeouts that occur during this operation.
*	Returns number of bytes read. */
int read_serial_bytes(jake_device_private* devpriv, char* buf, int bytes_to_read) {
	DWORD bytes_read;
	int sleepcounter = 0;
	int attempts = 0;
	int remaining_bytes = bytes_to_read;

	/* read the port in a loop to deal with timeouts */
	while(1) {
		#ifdef _WIN32
		if(!ReadFile(devpriv->port.serial.port, buf + (bytes_to_read - remaining_bytes), remaining_bytes, &bytes_read, NULL)) {
			return bytes_to_read - remaining_bytes;
		}
		#endif

		/* subtract the bytes we just read from the total amount we want */
		remaining_bytes -= bytes_read;

		/* if we didn't get them all.. */
		if(remaining_bytes != 0) {
			/* check if the thread is exiting, and if so break out of the loop and return */
			if(devpriv->rthread_done)
				break;

			/* otherwise just loop back round and read the port again */
			++sleepcounter;
			if(sleepcounter > 15) {
				jake_sleep(1);
				attempts++;
				sleepcounter = 0;

				if(attempts > 100)
					break;
			}
			continue;
		} else
			/* if all bytes have been read, break out of loop and return */
			break;
	}
	devpriv->data_recv += (bytes_to_read - remaining_bytes);
	return bytes_to_read - remaining_bytes;
}

/*	utility func, writes <bytes_to_write> bytes to the port associated with <devpriv>
*	from <buf>, handling any timeouts that occur during this operation.
*	Returns number of bytes written. */
int write_serial_bytes(jake_device_private* devpriv, char* buf, int bytes_to_write) {
	DWORD bytes_written;
	int remaining_bytes = bytes_to_write;

	#ifdef _WIN32
	/* write the port in a loop to deal with timeouts */
	while(1) {
		if(!WriteFile(devpriv->port.serial.port, buf + (bytes_to_write - remaining_bytes), remaining_bytes, &bytes_written, NULL))
			return bytes_to_write - remaining_bytes;

		/* subtract the bytes we just wrote from the total amount we want */
		remaining_bytes -= bytes_written;

		/* if we didn't get them all.. */
		if(remaining_bytes != 0) {
			/* loop back round and write the port again */
			continue;
		} else
			/* if all bytes have been written, break out of loop and return */
			break;
	}
	#endif
	return bytes_to_write - remaining_bytes;
}

int read_debug_bytes(jake_device_private* devpriv, char* buf, int bytes_to_read) {
	DWORD bytes_read;
	int sleepcounter = 0;
	int attempts = 0;
	int remaining_bytes = bytes_to_read;

	jake_sleep(10);

	while(1) {
		JAKE_DBG("Reading %d bytes from position %d\n", remaining_bytes, ftell(devpriv->port.dbg_read));
		if(devpriv->rthread_done) {
			return 0;
		}
		bytes_read = fread(buf + (bytes_to_read - remaining_bytes), 1, remaining_bytes, devpriv->port.dbg_read);

		/* subtract the bytes we just read from the total amount we want */
		remaining_bytes -= bytes_read;

		/* if we didn't get them all.. */
		if(remaining_bytes != 0) {
			/* check if the thread is exiting, and if so break out of the loop and return */
			if(devpriv->rthread_done) {
				return 0;
			}

			if(feof(devpriv->port.dbg_read)) {
				// seek back to start
				fseek(devpriv->port.dbg_read, 0, SEEK_SET);
				JAKE_DBG("******************************** \n\n RETURNING TO START OF INPUT FILE \n ********************************* \n\n\n");
			}

			/* otherwise just loop back round and read the port again */
			++sleepcounter;
			if(sleepcounter > 15) {
				jake_sleep(1);
				attempts++;
				sleepcounter = 0;

				if(attempts > 30)
					break;
			}
			continue;
		} else
			/* if all bytes have been read, break out of loop and return */
			break;
	}
	devpriv->data_recv += (bytes_to_read - remaining_bytes);
	return bytes_to_read - remaining_bytes;
}

/*	The driver supports several possible methods of creating a connection. 
*	It can be given a virtual serial port number (currently Windows only), a Bluetooth address
*	in string or 64-bit integer formats (opens an RFCOMM socket), or a filename. In the latter case data read
*	from the file is treated exactly the same way as data coming from a Bluetooth connection
*	making it very useful for debugging.
*
*	This requires different methods of opening the connection, so this function
*	acts as a standard "read" interface for the rest of the code */
int read_bytes(jake_device_private* dev, char* buf, int bytes_to_read) {
	if(dev == NULL)
		return 0;

	if(dev->rthread_done)
		return 0;

	int returned_bytes = 0;

	switch(dev->port.comms_type) {
		/* virtual serial port */
		case JAKE_CONN_VIRTUAL_SERIAL: {
			returned_bytes += read_serial_bytes(dev, buf, bytes_to_read);
			return returned_bytes;
		}
		/* RFCOMM socket */
		case JAKE_CONN_RFCOMM_I64:
		case JAKE_CONN_RFCOMM_STR: {
			returned_bytes += read_rfcomm_bytes(dev, buf, bytes_to_read);
			return returned_bytes;
		}
		/* File */
		case JAKE_CONN_DEBUGFILE: {
			returned_bytes += read_debug_bytes(dev, buf, bytes_to_read);
			return returned_bytes;
		}
		default:
			break;
	}
	return 0;
}

int write_bytes(jake_device_private* dev, char* buf, int bytes_to_write) {
	switch(dev->port.comms_type) {
		case JAKE_CONN_VIRTUAL_SERIAL:
			return write_serial_bytes(dev, buf, bytes_to_write);
		case JAKE_CONN_RFCOMM_I64:
		case JAKE_CONN_RFCOMM_STR:
			return write_rfcomm_bytes(dev, buf, bytes_to_write);
		case JAKE_CONN_DEBUGFILE:
			return fwrite(buf, 1, bytes_to_write, dev->port.dbg_write);
		default:
			break;
	}
	return 0;
}

/* generic function to read a register on the JAKE */
int jake_read_main(jake_device* dev, int addr, unsigned char* value) {
	jake_device_private* devpriv;
	char cpbuf[6];
	int timeout = 250;

	if(!dev) return JAKE_ERROR;

	devpriv = (jake_device_private*)dev->priv;
	
	/* construct the packet */
	sprintf(cpbuf, "$$R"); // header
	cpbuf[3] = addr; // address
	cpbuf[4] = cpbuf[5] = 0; // unused

	/*  send a command packet requesting the contents of the appropriate
	*	register, then wait for an ack to appear with the value */
	write_bytes(devpriv, cpbuf, jake_packet_lengths[JAKE_CMD_READ]);

	devpriv->waiting_for_ack_signal = TRUE;	
	devpriv->waiting_for_ack = TRUE;

	while(devpriv->waiting_for_ack_signal == TRUE) {
		jake_sleep(1);
		--timeout;
		if(timeout == 0)
			break;
	}

	devpriv->waiting_for_ack = FALSE;
	
	/* read the dev->lastack, dev->lastaddr and dev->lastval entries to get the response from the ack packet */
	if(!devpriv->lastack) {
		JAKE_DBG("FAILED TO READ %04X\n", addr);
		return JAKE_ERROR;
	}

	*value = devpriv->lastval;

	return JAKE_SUCCESS;
}

/* generic function to write a register on the JAKE */
int jake_write_main(jake_device* dev, int addr, unsigned char value) {
	jake_device_private* devpriv;
	char cpbuf[6];
	int timeout = 250;

	devpriv = (jake_device_private*)dev->priv;

	if(!devpriv || devpriv->waiting_for_ack) return JAKE_ERROR;

	/* construct the packet */
	sprintf(cpbuf, "$$W"); // header
	cpbuf[3] = addr; // address
	cpbuf[4] = value; // data
	cpbuf[5] = 0; // unused

	/* send a command packet containing the new value for the register, then
	*	wait for an ack packet to come back with a success/failure code */
	
	write_bytes(devpriv, cpbuf, jake_packet_lengths[JAKE_CMD_WRITE]);

	if(devpriv->wait_for_acks == 0)
		return JAKE_SUCCESS;

	devpriv->waiting_for_ack_signal = TRUE;	
	devpriv->waiting_for_ack = TRUE;

	while(devpriv->waiting_for_ack_signal == TRUE) {
		jake_sleep(1);
		--timeout;
		if(timeout == 0)
			break;
	}

	devpriv->waiting_for_ack = FALSE;
	/* read the dev->lastack, dev->lastaddr and dev->lastval entries to get the response from the ack packet */
	if(!devpriv->lastack)
		return JAKE_ERROR;

	devpriv->lastack = FALSE;
                            
	return JAKE_SUCCESS;
}

/* generic function to read a register on the JAKE */
int jake_read_bluetooth(jake_device* dev, int addr, unsigned char* value) {
	jake_device_private* devpriv;
	char cpbuf[6];
	int timeout = 250;

	if(!dev) return JAKE_ERROR;

	devpriv = (jake_device_private*)dev->priv;
	
	/* construct the packet */
	sprintf(cpbuf, "$$r"); // header
	cpbuf[3] = addr; // address
	cpbuf[4] = cpbuf[5] = 0; // unused

	/* send a command packet requesting the contents of the appropriate
	*	register, then wait for an ack to appear with the value */

	write_bytes(devpriv, cpbuf, jake_packet_lengths[JAKE_CMD_BT_READ]);

	devpriv->waiting_for_ack_signal = TRUE;	
	devpriv->waiting_for_ack = TRUE;

	while(devpriv->waiting_for_ack_signal == TRUE) {
		jake_sleep(1);
		--timeout;
		if(timeout == 0)
			break;
	}

	devpriv->waiting_for_ack = FALSE;
	
	/* read the dev->lastack, dev->lastaddr and dev->lastval entries to get the response from the ack packet */
	if(!devpriv->lastack) {
		JAKE_DBG("FAILED TO READ %04X\n", addr);
		return JAKE_ERROR;
	}

	*value = devpriv->lastval;

	return JAKE_SUCCESS;
}

/* generic function to write a register on the JAKE */
int jake_write_bluetooth(jake_device* dev, int addr, unsigned char value) {
	jake_device_private* devpriv;
	char cpbuf[6];
	int timeout = 250;

	devpriv = (jake_device_private*)dev->priv;

	if(!devpriv || devpriv->waiting_for_ack) return JAKE_ERROR;

	/* construct the packet */
	sprintf(cpbuf, "$$w"); // header
	cpbuf[3] = addr; // address
	cpbuf[4] = value; // data
	cpbuf[5] = 0; // unused

	/* send a command packet containing the new value for the register, then
	*	wait for an ack packet to come back with a success/failure code */
	
	write_bytes(devpriv, cpbuf, jake_packet_lengths[JAKE_CMD_BT_WRITE]);

	if(devpriv->wait_for_acks == 0)
		return JAKE_SUCCESS;

	devpriv->waiting_for_ack_signal = TRUE;	
	devpriv->waiting_for_ack = TRUE;

	while(devpriv->waiting_for_ack_signal == TRUE) {
		jake_sleep(1);
		--timeout;
		if(timeout == 0)
			break;
	}

	devpriv->waiting_for_ack = FALSE;
	/* read the dev->lastack, dev->lastaddr and dev->lastval entries to get the response from the ack packet */
	if(!devpriv->lastack)
		return JAKE_ERROR;

	devpriv->lastack = FALSE;
                            
	return JAKE_SUCCESS;
}

#define JAKE_READ_OK			1
#define JAKE_READ_ERROR		-1
#define JAKE_READ_CONTINUE		2

/* deals with parsing a complete raw packet */
int parse_jake_packet(jake_device* dev, jake_device_private* devpriv, int packet_type, char* packetbuf, int packetlen) {
	
	if(packet_type == JAKE_DATA) {
		JAKE_DBG("*** Parsing raw\n");
		jake_update_data(dev, packet_type, packetbuf);
		devpriv->packets_received++;
		devpriv->data_packets++;
	} else if (packet_type == JAKE_ACK_ACK || packet_type == JAKE_ACK_NEG) {
		JAKE_DBG("*** Parsing ack\n");
		jake_parse_ack_packet((jake_ack_packet*)packetbuf, devpriv);
		devpriv->packets_received++;
		if(packet_type == JAKE_ACK_ACK)
			devpriv->ack_packets++;
		else
			devpriv->nak_packets++;
	}
	return JAKE_READ_OK;
}

/* deals with reading a complete raw packet from the JAKE, not including the header which is provided already */
int read_jake_packet(jake_device* dev, jake_device_private* devpriv, int packet_type, char* packetbuf) {
	int packet_size = 0, bytes_left, bytes_read = 0;

	/* calculate bytes remaining and read them */
	bytes_left = jake_packet_lengths[packet_type] - JAKE_HEADER_LEN;
	bytes_read = read_bytes(devpriv, packetbuf + JAKE_HEADER_LEN, bytes_left);
	JAKE_DBG("bytes_left = %d, bytes_read = %d\n", bytes_left, bytes_read);

	// if we got a full packet
	if(bytes_left != bytes_read) {
		JAKE_DBG("ERROR READING PACKET\n");
		JAKE_DBG("%02X %02X %02X\n", packetbuf[0], packetbuf[1], packetbuf[2]);
		return JAKE_READ_ERROR;
	}

	return parse_jake_packet(dev, devpriv, packet_type, packetbuf, bytes_read + JAKE_HEADER_LEN);
}

/*	this function is where all the parsing of incoming data is done, running in a dedicated thread */
#ifdef _WIN32
unsigned long __stdcall jake_read_thread(void* shakedev) {
#else
void* jake_read_thread(void *shakedev) {
#endif
	jake_device* dev;
	jake_device_private* devpriv;
	char packetbuf[256];	// buffer for incoming packet
	int packet_type;		// type of packet
	int bytes_read;

	dev = (jake_device*)shakedev;
	devpriv = (jake_device_private*)dev->priv;

	char* info_header = "JAKE";
	BOOL valid_header = FALSE;

	/* loop while thread hasn't been told to exit */
	while(!devpriv->rthread_done) {
		memset(packetbuf, 0, 256); // clear buffer
		valid_header = FALSE;
		packet_type = JAKE_BAD_PKT;

		JAKE_DBG("--------------------- NEW LOOP\n");

		/* the job of this loop is:
		*	a) make sure a valid header has been detected before continuing
		*	b) classify the header 
		*/
		do {
			/*	start by reading 4 bytes, since packet headers are 4 bytes long */
			bytes_read = read_bytes(devpriv, packetbuf, JAKE_HEADER_LEN);
			JAKE_DBG("ML) Read initial header: %d bytes\n", bytes_read);

			/* only attempt to classify the header if all 4 bytes were read successfully */
			if(bytes_read == JAKE_HEADER_LEN) {
				if(packetbuf[0] == '$') {
					packet_type = jake_classify_packet_header(packetbuf, JAKE_HEADER_LEN);
					if(packet_type != JAKE_BAD_PKT)
						JAKE_DBG("ML) Type = %d (%s)\n", packet_type, jake_packet_type_names[packet_type]);
					else
						JAKE_DBG("ML) Type = -1\n");
				}
			}

			if(devpriv->rthread_done) {
				devpriv->rthread_exit = TRUE;
				#ifdef _WIN32
				return 1;
				#else
				pthread_exit((void*)1);
				#endif
			}

			/* if packet remains unclassified, try to find the next header in the data stream */
			if(packet_type == JAKE_BAD_PKT) {
				char c = ' ';
				JAKE_DBG("JAKE_BAD_PKT\n");
				JAKE_DBG("packetbuf: \"%s\"\n", packetbuf);
				JAKE_DBG("ML) packetbuf bin:");
				for(int i=0;i<4;i++) {
					JAKE_DBG("%d/%02X ", packetbuf[i], packetbuf[i]);
				}
				JAKE_DBG("\n");
				memset(packetbuf, 0, 256);

				int read_count = 0;
				/* read up to 50 bytes until next header is found. maximum JAKE packet size is less than 
				*	30 bytes so normally this should guarantee that a new header is found.
				*	the function just checks for $ characters, indicating the start of a
				*	packet header */
				while(read_count++ < 50 && (c != '$')) {
					read_bytes(devpriv, &c, 1);
				}
				packetbuf[0] = c;
				
				/* check for a possible header and attempt to classify */
				if(c == '$') {
					JAKE_DBG("ML-EH) Found header\n");
					read_bytes(devpriv, packetbuf+1, JAKE_HEADER_LEN-1);
					packet_type = jake_classify_packet_header(packetbuf, JAKE_HEADER_LEN);
				}
			}
			/* if packet was successfully classified, end the loop */
			if(packet_type != JAKE_BAD_PKT)
				valid_header = TRUE;
			JAKE_DBG("ML) valid header = %d\n", valid_header);
		} while(!devpriv->rthread_done && !valid_header);

		if(devpriv->rthread_done) {
			devpriv->rthread_exit = TRUE;
			#ifdef _WIN32
			return 1;
			#else
			pthread_exit((void*)1);
			#endif
		}
		read_jake_packet(dev, devpriv, packet_type, packetbuf);

	}

	devpriv->rthread_exit = TRUE;

	#ifdef _WIN32
	return 1;
	#else
	pthread_exit((void*)1);
	#endif
}

jake_device* jake_init_internal(jake_conn_data* scd) {
	jake_device* dev;
	jake_device_private* devpriv;
	TCHAR eventname[20];

	#ifdef _WIN32
	if((scd->type == JAKE_CONN_VIRTUAL_SERIAL) && (scd->com_port != -1 && (scd->com_port < 1 || scd->com_port > 100)))
		return NULL;
	#endif
	if((scd->type == JAKE_CONN_RFCOMM_I64) && (scd->btaddr == 0))
		return NULL;
	if((scd->type == JAKE_CONN_RFCOMM_STR) && scd->btaddr_str == NULL)
		return NULL;

	// setup serial port etc
	dev = (jake_device*)malloc(sizeof(jake_device));
	dev->priv = devpriv = (jake_device_private*)(void*)malloc(sizeof(jake_device_private));
	dev->handle = ++jake_handle_count;
	memset(devpriv, 0, sizeof(jake_device_private));

	devpriv->port.comms_type = -1;
	#ifdef _WIN32
		if(scd->type == JAKE_CONN_VIRTUAL_SERIAL) {
			devpriv->port.comms_type = JAKE_CONN_VIRTUAL_SERIAL;
			
			if(jake_open_serial(&(devpriv->port.serial), scd->com_port) == NULL) {
				free(devpriv);
				free(dev);
				return NULL;
			}
		} 
		#ifdef JAKE_RFCOMM_SUPPORTED
		if(scd->type == JAKE_CONN_RFCOMM_I64 || scd->type == JAKE_CONN_RFCOMM_STR) {
			devpriv->port.comms_type = scd->type;
			if(scd->type == JAKE_CONN_RFCOMM_I64) {
				if(jake_open_rfcomm_i64(&(devpriv->port.rfcomm), scd->btaddr) == NULL) {
					free(devpriv);
					free(dev);
					return NULL;
				}
			} else {
				if(jake_open_rfcomm_str(&(devpriv->port.rfcomm), scd->btaddr_str) == NULL) {
					free(devpriv);
					free(dev);
					return NULL;
				}
			}
		}
		#endif
		// open the input/output files 
		// BINARY mode ("b") important on Windows to avoid fread returning on newlines and
		// other silly stuff
		if(scd->type == JAKE_CONN_DEBUGFILE) {
			devpriv->port.comms_type = scd->type;
			devpriv->port.dbg_read = fopen(scd->readfile, "rb");
			if(devpriv->port.dbg_read == NULL) 
				return NULL;
			devpriv->port.dbg_write = fopen(scd->writefile, "wb");
			if(devpriv->port.dbg_write == NULL) {
				fclose(devpriv->port.dbg_read);
				return NULL;
			}
		}
	#endif
    #ifdef JAKE_RFCOMM_SUPPORTED
		if(scd->type == JAKE_CONN_RFCOMM_I64 || scd->type == JAKE_CONN_RFCOMM_STR) {
			devpriv->port.comms_type = scd->type;
			if(scd->type == JAKE_CONN_RFCOMM_I64) {
				if(jake_open_rfcomm_i64(&(devpriv->port.rfcomm), scd->btaddr) == NULL) {
					free(devpriv);
					free(dev);
					return NULL;
				}
			} else {
				if(jake_open_rfcomm_str(&(devpriv->port.rfcomm), scd->btaddr_str) == NULL) {
					free(devpriv);
					free(dev);
					return NULL;
				}
			}
		}
	#endif

	if(devpriv->port.comms_type == -1) {
		free(devpriv);
		free(dev);
		return NULL;
	}

	// name of event to be used for signalling the arrival of ACK/NAK packets
	// (this is only used on Windows)
	#ifdef _WIN32
	_stprintf(eventname, _T("jakecmd%d"), dev->handle);
	#endif

	// start thread to read from serial port
	devpriv->rthread_done = FALSE;
	devpriv->rthread_exit = FALSE;
	devpriv->got_fwhwserial = FALSE;

	devpriv->data_recv = 0;
	devpriv->wait_for_acks = 1; // NOTE
	devpriv->hwrev = devpriv->fwrev = 0.0;

	// launch the thread used internally to read the data
	jake_thread_init(&(devpriv->thread), jake_read_thread, (void*)dev, eventname);

	return dev;
}

/*	Main initialisation function for setting up a link to a JAKE device.
*	On Windows:
*		<com_port> is the number of the COM port to use for Bluetooth comms. 
*		If you use -1, the correct port number will be searched for in the
*		Windows registry, but this isn't guaranteed to work.
*	On Linux:
*		<btaddr> is the Bluetooth MAC address to connect to.
*		TODO: support device names eg "JAKE_ROO"
*
*	Returns NULL on failure, otherwise a handle to the JAKE device. */
#ifdef _WIN32
JAKE_API jake_device* jake_init_device(int com_port) {
	jake_conn_data scd = { 0 };
	scd.type = JAKE_CONN_VIRTUAL_SERIAL;
	scd.com_port = com_port;
	return jake_init_internal(&scd);
}
#endif

#ifdef JAKE_RFCOMM_SUPPORTED
JAKE_API jake_device* jake_init_device_rfcomm_i64(long long btaddr) {
	jake_conn_data scd = { 0 };
	scd.type = JAKE_CONN_RFCOMM_I64;
	scd.btaddr = btaddr;
	return jake_init_internal(&scd);
}

JAKE_API jake_device* jake_init_device_rfcomm_str(char* btaddr) {
	jake_conn_data scd = { 0 };
	scd.type = JAKE_CONN_RFCOMM_STR;
	if(btaddr == NULL || strlen(btaddr) > 20)
		return NULL;
	strcpy(scd.btaddr_str, btaddr);
	return jake_init_internal(&scd);
}
#endif

JAKE_API jake_device* jake_init_device_DEBUGFILE(char* readfile, char* writefile) {
	jake_conn_data scd = { 0 };
	scd.type = JAKE_CONN_DEBUGFILE;
	if(readfile == NULL || writefile == NULL || strlen(readfile) > 255 || strlen(writefile) > 255) 
		return NULL;
	strcpy(scd.readfile, readfile);
	strcpy(scd.writefile, writefile);
	return jake_init_internal(&scd);
}

int jake_close(jake_port* port) {
	#ifdef _WIN32
	if(port->comms_type == JAKE_CONN_VIRTUAL_SERIAL) {
		return jake_close_serial(&(port->serial));
	} 
	#endif
	#ifdef JAKE_RFCOMM_SUPPORTED
	if(port->comms_type == JAKE_CONN_RFCOMM_I64) {
		return jake_close_rfcomm(&(port->rfcomm));
	}
	else if(port->comms_type == JAKE_CONN_RFCOMM_STR) {
		return jake_close_rfcomm(&(port->rfcomm));
	}
	#endif
	if(port->comms_type == JAKE_CONN_DEBUGFILE) {
		fclose(port->dbg_read);
		fclose(port->dbg_write);
		port->dbg_read = NULL;
		port->dbg_write = NULL;
		return JAKE_SUCCESS;
	}
	return JAKE_ERROR;
}

/*	Call this function to close the link with a JAKE device and free up any resources
*	used in maintaining the connection.
*	<sh> is a handle to a JAKE device,
*
*	Returns JAKE_SUCCESS on success, JAKE_ERROR on failure. */
JAKE_API int jake_free_device(jake_device* dev) {
	jake_device_private* devpriv;
	int c = 0;

	if(!dev) return JAKE_ERROR;

	devpriv = (jake_device_private*)dev->priv;

	DWORD status = 0;
	devpriv->rthread_done = TRUE;
	#ifdef XXX
	while(GetExitCodeThread(&(devpriv->thread.rthread), &status)) {
		if(status != STILL_ACTIVE) {
			break;
		}
	}
	#endif

	jake_close(&(devpriv->port));
	while(!devpriv->rthread_exit) {
		jake_sleep(1);
	}

	free(devpriv);
	free(dev);

	devpriv = NULL;
	dev = NULL;
	return JAKE_SUCCESS;
}

int get_info(jake_device* dev) {
	if(!dev) return JAKE_ERROR;

	unsigned char fwminor, fwmajor;
	unsigned char hwminor, hwmajor;
	unsigned char sminor, smajor;

	int ret = 0;
	ret += jake_read_main(dev, JAKE_REG_FW_MINOR, &fwminor);
	ret += jake_read_main(dev, JAKE_REG_FW_MAJOR, &fwmajor);

	ret += jake_read_main(dev, JAKE_REG_HW_MINOR, &hwminor);
	ret += jake_read_main(dev, JAKE_REG_HW_MAJOR, &hwmajor);

	ret += jake_read_main(dev, JAKE_REG_SERIAL_LSB, &sminor);
	ret += jake_read_main(dev, JAKE_REG_SERIAL_MSB, &smajor);

	if(ret != (6*JAKE_SUCCESS))
		return JAKE_ERROR;

	jake_device_private* devpriv = (jake_device_private*)dev->priv;
	
	devpriv->got_fwhwserial = TRUE;
	// XXX fix
	devpriv->fwrev = fwmajor + (0.01 * fwminor);
	devpriv->hwrev = hwmajor + (0.01 * hwminor);
	sprintf(devpriv->serial, "%04d", smajor + (sminor << 8));
	

	return JAKE_SUCCESS;
}

JAKE_API float jake_info_firmware_revision(jake_device* dev) {
	if(!dev) return 0.0;

	jake_device_private* devpriv = (jake_device_private*)dev->priv;

	if(!devpriv->got_fwhwserial) {
		return get_info(dev);
	}

	return devpriv->fwrev;
}

JAKE_API float jake_info_hardware_revision(jake_device* dev) {
	if(!dev) return 0.0;

	jake_device_private* devpriv = (jake_device_private*)dev->priv;

	return ((jake_device_private*)dev->priv)->hwrev;
}

JAKE_API char* jake_info_serial_number(jake_device* dev) {
	if(!dev) return NULL;

	jake_device_private* devpriv = (jake_device_private*)dev->priv;

	return ((jake_device_private*)dev->priv)->serial;
}

JAKE_API char jake_info_rssi(jake_device* dev) {
	if(!dev) return NULL;

	jake_device_private* devpriv = (jake_device_private*)dev->priv;

	return devpriv->data.rssi;
}

JAKE_API int jake_info_power_source(jake_device* dev) {
	if(!dev) return NULL;

	jake_device_private* devpriv = (jake_device_private*)dev->priv;

	return (devpriv->data.power & 0x7F) >> 7; // only want top bit
}

JAKE_API int jake_info_power_level(jake_device* dev) {
	if(!dev) return NULL;

	jake_device_private* devpriv = (jake_device_private*)dev->priv;

	return devpriv->data.power & 0x7E; // mask off top bit
}

JAKE_API JAKE_INT64 jake_info_received_packets(jake_device* dev) {
	if(!dev) return -1;

	jake_device_private* devpriv = (jake_device_private*)dev->priv;

	return devpriv->packets_received;
}

int jake_accx(jake_device* dev) {
	if(!dev) return JAKE_ERROR;

	jake_device_private* devpriv = (jake_device_private*)dev->priv;

	return devpriv->data.acc[0];
}

int jake_accy(jake_device* dev){
	if(!dev) return JAKE_ERROR;

	jake_device_private* devpriv = (jake_device_private*)dev->priv;

	return devpriv->data.acc[1];
}

int jake_accz(jake_device* dev){
	if(!dev) return JAKE_ERROR;

	jake_device_private* devpriv = (jake_device_private*)dev->priv;

	return devpriv->data.acc[2];
}

int jake_acc(jake_device* dev, int* xyz) {
	jake_device_private* devpriv;

	if(!dev || !xyz) return JAKE_ERROR;

	devpriv = (jake_device_private*)dev->priv;

	for(int i=0;i<3;i++)
		xyz[i] = devpriv->data.acc[i];
	return JAKE_SUCCESS;
}

int jake_magx(jake_device* dev) {
	if(!dev) return JAKE_ERROR;

	jake_device_private* devpriv = (jake_device_private*)dev->priv;

	return devpriv->data.mag[0];
}

int jake_magy(jake_device* dev) {
	if(!dev) return JAKE_ERROR;

	jake_device_private* devpriv = (jake_device_private*)dev->priv;

	return devpriv->data.mag[1];
}

int jake_magz(jake_device* dev) {
	if(!dev) return JAKE_ERROR;

	jake_device_private* devpriv = (jake_device_private*)dev->priv;

	return devpriv->data.mag[2];
}

int jake_mag(jake_device* dev, int* xyz) {
	jake_device_private* devpriv;

	if(!dev || !xyz) return JAKE_ERROR;

	devpriv = (jake_device_private*)dev->priv;

	for(int i=0;i<3;i++)
		xyz[i] = devpriv->data.mag[i];
	return JAKE_SUCCESS;
}

int jake_heading(jake_device* dev) {
	if(!dev) return JAKE_ERROR;
	
	jake_device_private* devpriv = (jake_device_private*)dev->priv;
	return devpriv->data.heading;
}

void jake_wait_for_acks(jake_device* dev, int wait_for_ack) {
	jake_device_private* devpriv;

	if(!dev) return;

	devpriv = (jake_device_private*)dev->priv;

	devpriv->wait_for_acks = wait_for_ack;
}

unsigned short jake_data_timestamp(jake_device* dev) {
	if(!dev) return JAKE_ERROR;
	
	jake_device_private* devpriv = (jake_device_private*)dev->priv;

	return devpriv->data.counter;
}

JAKE_API int jake_read_configuration(jake_device* dev, unsigned char* value) {
	if(!dev) return JAKE_ERROR;

	return jake_read_main(dev, JAKE_REG_CONFIG0, value);
}

JAKE_API int jake_write_configuration(jake_device* dev, unsigned char value) {
	if(!dev) return JAKE_ERROR;

	return jake_write_main(dev, JAKE_REG_CONFIG0, value);
}

JAKE_API int jake_read_configuration2(jake_device* dev, unsigned char* value) {
	if(!dev) return JAKE_ERROR;

	return jake_read_main(dev, JAKE_REG_CONFIG1, value);
}

JAKE_API int jake_write_configuration2(jake_device* dev, unsigned char value) {
	if(!dev) return JAKE_ERROR;

	return jake_write_main(dev, JAKE_REG_CONFIG1, value);
}

JAKE_API int jake_read_acc_offset(jake_device* dev, int xyz, int range, short* value) {
	if(!dev) return JAKE_ERROR;

	int base_reg = -1;
	switch(range) {
		case JAKE_ACCEL_RANGE_2G:
			base_reg = JAKE_REG_ACCX_2G_OFFSET_LSB;
			break;
		case JAKE_ACCEL_RANGE_6G:
			base_reg = JAKE_REG_ACCX_6G_OFFSET_LSB;
			break;
		default:
			return JAKE_ERROR;
	}

	switch(xyz) {
		case JAKE_X_AXIS:
			break;
		case JAKE_Y_AXIS:
			base_reg += 2;
			break;
		case JAKE_Z_AXIS:
			base_reg += 4;
			break;
		default:
			return JAKE_ERROR;
	}

	unsigned char lsb, msb;
	if(jake_read_main(dev, base_reg, &lsb) != JAKE_SUCCESS)
		return JAKE_ERROR;
	if(jake_read_main(dev, base_reg+1, &msb) != JAKE_SUCCESS)
		return JAKE_ERROR;

	*value = (msb << 8) + lsb;

	return JAKE_SUCCESS;
}

JAKE_API int jake_write_acc_offset(jake_device* dev, int xyz, int range, short value) {
	if(!dev) return JAKE_ERROR;

	int base_reg = -1;
	switch(range) {
		case JAKE_ACCEL_RANGE_2G:
			base_reg = JAKE_REG_ACCX_2G_OFFSET_LSB;
			break;
		case JAKE_ACCEL_RANGE_6G:
			base_reg = JAKE_REG_ACCX_6G_OFFSET_LSB;
			break;
		default:
			return JAKE_ERROR;
	}

	switch(xyz) {
		case JAKE_X_AXIS:
			break;
		case JAKE_Y_AXIS:
			base_reg += 2;
			break;
		case JAKE_Z_AXIS:
			base_reg += 4;
			break;
		default:
			return JAKE_ERROR;
	}

	unsigned char lsb, msb;
	lsb = (value & 0x00ff);
	msb = (value & 0xff00) >> 8;
	if(jake_write_main(dev, base_reg, lsb) != JAKE_SUCCESS)
		return JAKE_ERROR;
	if(jake_write_main(dev, base_reg+1, msb) != JAKE_SUCCESS)
		return JAKE_ERROR;

	return JAKE_SUCCESS;
}

JAKE_API int jake_read_acc_scale(jake_device* dev, int xyz, int range, short* value) {
	if(!dev) return JAKE_ERROR;

	int base_reg = -1;
	switch(range) {
		case JAKE_ACCEL_RANGE_2G:
			base_reg = JAKE_REG_ACCX_2G_SCALE_LSB;
			break;
		case JAKE_ACCEL_RANGE_6G:
			base_reg = JAKE_REG_ACCX_6G_SCALE_LSB;
			break;
		default:
			return JAKE_ERROR;
	}

	switch(xyz) {
		case JAKE_X_AXIS:
			break;
		case JAKE_Y_AXIS:
			base_reg += 2;
			break;
		case JAKE_Z_AXIS:
			base_reg += 4;
			break;
		default:
			return JAKE_ERROR;
	}

	unsigned char lsb, msb;
	if(jake_read_main(dev, base_reg, &lsb) != JAKE_SUCCESS)
		return JAKE_ERROR;
	if(jake_read_main(dev, base_reg+1, &msb) != JAKE_SUCCESS)
		return JAKE_ERROR;

	*value = (msb << 8) + lsb;

	return JAKE_SUCCESS;
}

JAKE_API int jake_write_acc_scale(jake_device* dev, int xyz, int range, short value) {
	if(!dev) return JAKE_ERROR;

	int base_reg = -1;
	switch(range) {
		case JAKE_ACCEL_RANGE_2G:
			base_reg = JAKE_REG_ACCX_2G_SCALE_LSB;
			break;
		case JAKE_ACCEL_RANGE_6G:
			base_reg = JAKE_REG_ACCX_6G_SCALE_LSB;
			break;
		default:
			return JAKE_ERROR;
	}

	switch(xyz) {
		case JAKE_X_AXIS:
			break;
		case JAKE_Y_AXIS:
			base_reg += 2;
			break;
		case JAKE_Z_AXIS:
			base_reg += 4;
			break;
		default:
			return JAKE_ERROR;
	}

	unsigned char lsb, msb;
	lsb = (value & 0x00ff);
	msb = (value & 0xff00) >> 8;
	if(jake_write_main(dev, base_reg, lsb) != JAKE_SUCCESS)
		return JAKE_ERROR;
	if(jake_write_main(dev, base_reg+1, msb) != JAKE_SUCCESS)
		return JAKE_ERROR;

	return JAKE_SUCCESS;
}

JAKE_API int jake_read_mag_scale(jake_device* dev, int xyz, short* value) {
	if(!dev) return JAKE_ERROR;

	int base_reg = JAKE_REG_MAGX_SCALE_LSB;

	switch(xyz) {
		case JAKE_X_AXIS:
			break;
		case JAKE_Y_AXIS:
			base_reg += 2;
			break;
		case JAKE_Z_AXIS:
			base_reg += 4;
			break;
		default:
			return JAKE_ERROR;
	}

	unsigned char lsb, msb;
	if(jake_read_main(dev, base_reg, &lsb) != JAKE_SUCCESS)
		return JAKE_ERROR;
	if(jake_read_main(dev, base_reg+1, &msb) != JAKE_SUCCESS)
		return JAKE_ERROR;

	*value = (msb << 8) + lsb;

	return JAKE_SUCCESS;
}

JAKE_API int jake_write_mag_scale(jake_device* dev, int xyz, short value) {
	if(!dev) return JAKE_ERROR;

	int base_reg = JAKE_REG_MAGX_SCALE_LSB;

	switch(xyz) {
		case JAKE_X_AXIS:
			break;
		case JAKE_Y_AXIS:
			base_reg += 2;
			break;
		case JAKE_Z_AXIS:
			base_reg += 4;
			break;
		default:
			return JAKE_ERROR;
	}

	unsigned char lsb, msb;
	lsb = (value & 0x00ff);
	msb = (value & 0xff00) >> 8;
	if(jake_write_main(dev, base_reg, lsb) != JAKE_SUCCESS)
		return JAKE_ERROR;
	if(jake_write_main(dev, base_reg+1, msb) != JAKE_SUCCESS)
		return JAKE_ERROR;

	return JAKE_SUCCESS;
}

JAKE_API int jake_read_mag_offset(jake_device* dev, int xyz, short* value) {
	if(!dev) return JAKE_ERROR;

	int base_reg = JAKE_REG_MAGX_OFFSET_LSB;

	switch(xyz) {
		case JAKE_X_AXIS:
			break;
		case JAKE_Y_AXIS:
			base_reg += 2;
			break;
		case JAKE_Z_AXIS:
			base_reg += 4;
			break;
		default:
			return JAKE_ERROR;
	}

	unsigned char lsb, msb;
	if(jake_read_main(dev, base_reg, &lsb) != JAKE_SUCCESS)
		return JAKE_ERROR;
	if(jake_read_main(dev, base_reg+1, &msb) != JAKE_SUCCESS)
		return JAKE_ERROR;

	*value = (msb << 8) + lsb;

	return JAKE_SUCCESS;
}

JAKE_API int jake_write_mag_offset(jake_device* dev, int xyz, short value) {
	if(!dev) return JAKE_ERROR;

	int base_reg = JAKE_REG_MAGX_OFFSET_LSB;

	switch(xyz) {
		case JAKE_X_AXIS:
			break;
		case JAKE_Y_AXIS:
			base_reg += 2;
			break;
		case JAKE_Z_AXIS:
			base_reg += 4;
			break;
		default:
			return JAKE_ERROR;
	}

	unsigned char lsb, msb;
	lsb = (value & 0x00ff);
	msb = (value & 0xff00) >> 8;
	if(jake_write_main(dev, base_reg, lsb) != JAKE_SUCCESS)
		return JAKE_ERROR;
	if(jake_write_main(dev, base_reg+1, msb) != JAKE_SUCCESS)
		return JAKE_ERROR;

	return JAKE_SUCCESS;
}

JAKE_API int jake_read_sample_rate(jake_device* dev, unsigned char* current_rate) {
	if(dev == NULL)
		return JAKE_ERROR;


	unsigned char tmp;
	if(jake_read_main(dev, JAKE_REG_CONFIG1, &tmp) != JAKE_SUCCESS)
		return JAKE_ERROR;

	*current_rate = tmp & 0x07; // only lower 3 bits
	return JAKE_SUCCESS;
}

JAKE_API int jake_write_sample_rate(jake_device* dev, int new_rate) {
	if(dev == NULL)
		return JAKE_ERROR;


	if(new_rate < JAKE_SAMPLE_RATE_0 || new_rate > JAKE_SAMPLE_RATE_120) {
		// work out best matching sample rate
		if(new_rate < 0) {
			new_rate = JAKE_SAMPLE_RATE_0;
		} else if(new_rate > 120) {
			new_rate = JAKE_SAMPLE_RATE_120;
		} else {
			unsigned min = 0xFFFFFFFF;
			int minpos = 0;
			for(int i=0;i<JAKE_OUTPUT_RATE_COUNT;i++) {
				if(abs(new_rate - jake_output_rates[i]) < min) {
					min = abs(new_rate - jake_output_rates[i]);
					minpos = i;
				}
			}
			new_rate = minpos;
		}
	}
	return jake_write_main(dev, JAKE_REG_CONFIG1, new_rate);
}
