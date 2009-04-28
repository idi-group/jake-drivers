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
#include "jake_driver.h"
#include "jake_packets.h"

#include <stdio.h>
#include <stdlib.h>

int jake_parse_ack_packet(jake_ack_packet* ack, jake_device_private* devpriv) {
	if(ack == NULL) return JAKE_ERROR;


	if(ack->header[3] == 'A') { 
		devpriv->lastack = TRUE;
		devpriv->lastval = ack->ack;
		JAKE_DBG("POSITIVE ACK: %d\n", ack->ack);
	} else {
		devpriv->lastack = FALSE;
		devpriv->lastval = ack->ack;
		JAKE_DBG("NEGATIVE ACK!\n");
	}

	return JAKE_SUCCESS;
}

int jake_classify_packet_header(char* header, unsigned header_length) {
	int type = JAKE_BAD_PKT, i;

	JAKE_DBG("classifying(): %c/%02X %c/%02X %c/%02X \n", header[0], header[0], header[1], header[1], header[2], header[2]);

	if(header_length != JAKE_HEADER_LEN || header == NULL || header[0] != '$')
		return JAKE_BAD_PKT;
	
	for(i=0;i<JAKE_NUM_PACKET_TYPES;i++) {
		if(strncmp(jake_packet_headers[i], header, header_length) == 0) {
			type = i;
			break;
		}
	}

	if(type != JAKE_BAD_PKT) {
		JAKE_DBG("Packet classified as %d (%s)\n", type, jake_packet_type_names[type]);
	} else {
		JAKE_DBG("Packet classified as BAD PACKET\n");
	}
	return type;
}

int jake_update_data(void* jakedev, int type, char* rawpacket) {
	jake_device_private* dev = (jake_device_private*)(((jake_device*)jakedev))->priv;

	switch(type) {
		case JAKE_DATA:
			memcpy(&(dev->data), rawpacket, sizeof(jake_data_packet));
			break;
	}

	return JAKE_SUCCESS;
}

// Given a number represented in ASCII of the form sdddd, where s is an optional
// sign character (+/-) and each d is a decimal digit 0-9, returns the numeric equivalent.
// Can also handle dddd, ddd, dd, sdd, etc
int dec_ascii_to_int(char* ascii_buf, int buflen, int digits) {
	int decval = 0, i, mult = 1;
	for(i=buflen-1;i>=buflen-digits;i--) {
		decval += (ascii_buf[i] - '0') * mult;
		mult *= 10;
	}
	if(digits == buflen - 1 && ascii_buf[0] == '-')
		decval *= -1;
	return decval;
}

// as above but for hex. Doesn't handle 0x prefixes or h suffix, and relies on
// any letters being upper case.
int hex_ascii_to_int(char* ascii_buf, int buflen, int digits) {
	int hexval = 0, i, mult = 1;
	for(i=buflen-1;i>=buflen-digits;i--) {
		if(isdigit(ascii_buf[i]))
			hexval += (ascii_buf[i] - '0') * mult;
		else
			hexval += ((ascii_buf[i] - 'A') + 10) * mult;
		mult *= 16;
	}
	if(digits == buflen - 1 && ascii_buf[0] == '-')
		hexval *= -1;
	return hexval;
}

