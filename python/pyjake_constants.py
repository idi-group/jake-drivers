# Copyright (c) 2006-2015, University of Glasgow
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright notice, this list of 
#       conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright notice, this list
#       of conditions and the following disclaimer in the documentation and/or other
#       materials provided with the distribution.
#    * Neither the name of the University of Glasgow nor the names of its contributors 
#       may be used to endorse or promote products derived from this software without
#       specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
# THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


## Global success code for JAKE functions.
JAKE_SUCCESS = 1
## Global error code for JAKE functions.
JAKE_ERROR = -1

JAKE_CONN_TYPE_SERIAL_PORT = 0
JAKE_CONN_TYPE_DEBUG_FILE = 1

# registers (not complete list yet)
JAKE_REG_CONFIG0 = 0x00
JAKE_REG_CONFIG1 = 0x01

# bits in the first configuration register (power)
JAKE_ACCEL              = 0x01
JAKE_MAG            = 0x02
JAKE_HEADING        = 0x04
JAKE_ACCEL_CALIBRATION      = 0x08
JAKE_MAG_CALIBRATION    = 0x10
JAKE_ACCEL_FILTER_ORDER16       = 0x20
JAKE_MAG_FILTER_ORDER8      = 0x40
JAKE_ACCEL_RANGE_6G     = 0x80

JAKE_SAMPLE_RATE_0 = 0
JAKE_SAMPLE_RATE_7_5 = 1
JAKE_SAMPLE_RATE_15 = 2
JAKE_SAMPLE_RATE_30 = 3
JAKE_SAMPLE_RATE_60 = 4
JAKE_SAMPLE_RATE_120 = 5

JAKE_OUTPUT_RATE_COUNT = 6

JAKE_OUTPUT_RATES = [0, 7.5, 15, 30, 60, 120 ]

#       Length of a JAKE packet header in bytes
JAKE_HEADER_LEN     = 4

#       Packet types enumeration
(
    JAKE_DATA, 
    
    JAKE_CMD_READ,
    JAKE_CMD_BT_READ,
    
    JAKE_CMD_WRITE,
    JAKE_CMD_BT_WRITE,

    JAKE_ACK_ACK,
    JAKE_ACK_NEG,
) = range(7)

JAKE_NUM_PACKET_TYPES = 7

#       Indicates an unknown/badly formatted packet
JAKE_BAD_PKT        = -1

# variables

jake_packet_headers =\
[       
    "$$$D", 
            
    "$$R",              
    "$$r", 
                
    "$$W",      
    "$$w",
            
    "$$$A",         
    "$$$N", 
]

jake_packet_lengths =\
[ 
    24,

    6, 
    6,

    6, 
    6,

    24, 
    24, 
]

