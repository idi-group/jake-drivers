# Copyright (c) 2006-2015, University of Glasgow
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright notice, this list of 
#           conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright notice, this list
#           of conditions and the following disclaimer in the documentation and/or other
#           materials provided with the distribution.
#    * Neither the name of the University of Glasgow nor the names of its contributors 
#           may be used to endorse or promote products derived from this software without
#           specific prior written permission.
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


# JAKE packet handling stuff. This is where all the parsing etc is done.

import thread, struct
from time import sleep

import pyjake_serial_pc as pyjake_serial
from pyjake_constants import *
    
debugfp = None
debugging = False
def debug(str, opennew=False):
    if not debugging:
        return

    f = None
    if opennew:
        f = open("debug.txt", "w")
    else:
        f = open("debug.txt", "a")

    f.write(str + "\n")
    #print str
    f.close()

# class which defines a variable for each item of data the JAKE can return, plus sequence numbers
class jake_sensor_data:
    def __init__(self):
        self.accx, self.accy, self.accz = 0,0,0
        self.magx, self.magy, self.magz = 0,0,0
        self.heading = 0
        self.power = 0
        self.rssi = 0
        self.ack = 0
        self.timestamp = 0
        self.power_level = 0
        self.power_source = 0

class jake_device_private:
    def __init__(self, conn_type, conn_data):
        self.data = jake_sensor_data()

        #self.dbgmode = "starting up"
        self.sendmsg = None
        self.lastack = False
        self.lastaddr, self.lastval = 0,0
        self.thread_done = False
        self.thread_exit = False
        self.waiting_for_ack = False
        self.synced = False

        self.data_packets = 0
        self.ack_packets = 0
        self.nak_packets = 0
        self.error_packets = 0
        self.received_packets = 0

        self.fwrev = None
        self.hwrev = None
        self.serial = None
        self.read_startup = False

        self.data_callback = None

        self.bluetooth_addr = None
        self.file_objects = None

        self.unpack_data = struct.Struct("4shhhhhhhBbxH")
        self.unpack_ack = struct.Struct("4shhhhhhhBbBH")

        self.conn_type = conn_type
        if conn_type == JAKE_CONN_TYPE_SERIAL_PORT:
            self.bluetooth_addr = conn_data[0]
        elif conn_type == JAKE_CONN_TYPE_DEBUG_FILE:
            self.inpfile, self.outfile, self.eof_callback = conn_data
            self.want_eof_callbacks = True

        self.port = None

        debug("starting thread", True)
        thread.start_new_thread(self.run, ())

    #       sends the given byte string through the internal port object
    def write(self, bytes):
        if self.conn_type == JAKE_CONN_TYPE_SERIAL_PORT:
            self.port.write(bytes)
        else:
            self.outfile.write(bytes)

    def read(self, bytes_to_read):
        if self.conn_type == JAKE_CONN_TYPE_SERIAL_PORT:
            allbytes = self.port.read(bytes_to_read)
        else:
            allbytes = self.inpfile.read(bytes_to_read)
            if len(allbytes) == 0:
                result = False
                if self.eof_callback and self.want_eof_callbacks:
                    result, self.want_eof_callbacks = self.eof_callback()
                if not self.eof_callback or result:
                    self.inpfile.seek(0)
                    allbytes = self.inpfile.read(bytes_to_read)

        if len(allbytes) > 0:
            self.synced = True
        return allbytes
            
    #       closes the internal port object and terminates reader thread
    def close(self):
        self.thread_done = True
        
        totaltime = 0.0
        while totaltime < 1.0 and not self.thread_exit:
            sleep(0.1)
            totaltime += 0.1
        debug("thread exited event")
    
    def port_setup(self):
        try:    
            debug("creating port")
            self.port = pyjake_serial.serial_port(self.bluetooth_addr)
            if not self.port.open():
                debug("port creation failed")
                self.thread_done = True
                return JAKE_ERROR

            debug("port opened OK")
        except pyjake_serial.pyjake_serial_error:
            debug(u'error; port creation failed')
            self.thread_done = True
            return JAKE_ERROR

    def send_waiting_packet(self):
        self.write(self.sendmsg)
        self.sendmsg = None

    def parse_jake_packet(self, packet_type, packet):
        if packet_type == JAKE_DATA:
            self.jake_update_data( packet)
        else:
            self.jake_parse_ack_packet( packet)


    def read_jake_packet(self, packet_type, packet):
        bytes_left = jake_packet_lengths[packet_type] - JAKE_HEADER_LEN
        bytes_read = self.read(bytes_left)

        if bytes_left != len(bytes_read):
            return -1

        packet += bytes_read

        return self.parse_jake_packet(packet_type, packet)

    # this function runs in a thread and deals with reading/writing
    # data from/to the serial port/bt socket.
    def run(self):
        packet = ""
        debug("setting up port")
        if self.conn_type == JAKE_CONN_TYPE_SERIAL_PORT:
            if self.port_setup() == JAKE_ERROR:
                debug("port setup failed")
                return 

        self.thread_done = False
        debug("port setup done")
        
        debug("entering main loop")
        while not self.thread_done:
            debug("start of main loop")
            valid_header = False
            packet_type = -1
        
            if self.thread_done:
                break

            debug("starting check for header")
            while not self.thread_done and not valid_header:
                packet = self.read(JAKE_HEADER_LEN)
            
                debug("initial header: " + packet + "(" + str(len(packet)) + "), " + str(len(packet)) + " bytes")

                if len(packet) > 0 and packet[0] == '$':
                    packet_type = self.classify_packet_header(packet)
                    debug("ML) Type = %d" % packet_type)
                else:
                    packet_type = JAKE_BAD_PKT
                    
                if packet_type == JAKE_DATA:
                    self.data_packets += 1
                elif packet_type == JAKE_ACK_ACK:
                    self.lastack = True
                    self.ack_packets += 1
                elif packet_type == JAKE_ACK_NEG:
                    self.lastack = False
                    self.nak_packets += 1
                elif packet_type == JAKE_BAD_PKT:
                    self.error_packets += 1
                
                if packet_type == JAKE_BAD_PKT:
                    debug("in error handler")
                    read_count = 0
                    packet = [' ']
                    while read_count < 50 and (len(packet) > 0 and packet[0] != '$' and ord(packet[0]) != 0x7F):
                        packet = self.read(1)
                        read_count += 1
                    
                    if len(packet) > 0 and packet[0] == '$':
                        debug("error handler: first char is ASCII header")
                        packet += self.read(JAKE_HEADER_LEN - 1)
                        debug("error handler: new header is  "+ packet)
                        packet_type = self.classify_packet_header( packet)
                        
                if packet_type != JAKE_BAD_PKT:
                    valid_header = True
            
            debug("Packet type = " + str(packet_type))
            self.read_jake_packet(packet_type, packet)
    
            self.synced = True

        if self.conn_type == JAKE_CONN_TYPE_SERIAL_PORT:
            self.port.close()
        self.thread_exit = True
        debug("closing port and exiting thread")


    def jake_parse_ack_packet(self, packet):
        # ack packets have an identical format to the data packets, but only
        # the header and the "ack" field are relevant, ignore everything else
        packet_data = self.unpack_ack.unpack(packet)
        self.lastval = packet_data[10]

    def classify_packet_header(self, header):
        debug("classify len = " + str(len(header)) + " first char = " + header[0])
        if (len(header) != JAKE_HEADER_LEN):
            return JAKE_BAD_PKT

        debug("classify: initial tests OK")
        i = 0
        
        for i in range(i, JAKE_NUM_PACKET_TYPES):
            if jake_packet_headers[i] == header[:JAKE_HEADER_LEN]:
                return i
            
        return JAKE_BAD_PKT

    # converts the 16-bit signed raw data values into Python ints
    def convert_raw_data_value(self, bytes):
        lsb = ord(bytes[0])
        msb = ord(bytes[1])

        if msb & 0x80:
            return (lsb + (msb << 8)) - 65536
        else:
            return lsb + (msb << 8)

    def jake_update_data(self, packet):
        (hdr,                                                       # 4 byte header, ignored
            self.data.accx, self.data.accy, self.data.accz,         # 3x 16-bit signed
            self.data.magx, self.data.magy, self.data.magz,         # 3x 16-bit signed
            self.data.heading,                                          # 1x 16-bit signed
            self.data.power,                                        # 1x 8-bit unsigned
            self.data.rssi,                                         # 1x 8-bit signed
            self.data.timestamp                                     # 1x 16-bit unsigned
        ) = self.unpack_data.unpack(packet)

        if self.data_callback:
            self.data_callback((self.data.accx, self.data.accy, self.data.accz), (self.data.magx, self.data.magy, self.data.magz), self.data.heading, self.data.timestamp)
