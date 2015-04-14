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


import pyjake_packets, struct, sys, os
from time import sleep

from pyjake_constants import *

# an instance of this class represents a single JAKE device
class jake_device:

    def __init__(self):
        self.priv = None
        self.ack_timeout_ms = 500
    
    def connect(self, addr):
        if self.priv != None:
            self.priv.close()
            self.priv = None

        self.priv = pyjake_packets.jake_device_private(JAKE_CONN_TYPE_SERIAL_PORT, (addr,))

        elapsed = 0
        while not self.priv.synced and not self.priv.thread_done and elapsed < 5000:
            sleep(0.05)
            elapsed += 50

        if not self.priv.synced or elapsed >= 5000:
            return False

        return True

    def connect_debug(self, inputfile, eof_callback=None, outputfile=None):
        if self.priv != None:
            self.priv.close()
            self.priv = None

        if not os.path.exists(inputfile):
            raise Exception("Specified file not found ('%s')"%inputfile)

        inputfilefp = open(inputfile, "rb")
        if not outputfile:
            outputfilefp = sys.stdout
        else:
            outputfilefp = open(outputfile, "w")

        self.priv = pyjake_packets.jake_device_private(JAKE_CONN_TYPE_DEBUG_FILE, (inputfilefp, outputfilefp, eof_callback))

        return True

    def close(self):
        if self.priv:
            self.priv.close()

    def register_data_callback(self, cb):
        self.priv.data_callback = cb

    def data_timestamp(self):
        return self.priv.data.timestamp

    #   Returns x-axis accelerometer reading
    def accx(self):
        return self.priv.data.accx

    #   Returns y-axis accelerometer reading
    def accy(self):
        return self.priv.data.accy

    #   Returns z-axis accelerometer reading
    def accz(self):
        return self.priv.data.accz

    #   Returns accelerometer readings in a list: [x, y, z]
    def acc(self):
        return [self.priv.data.accx, self.priv.data.accy, self.priv.data.accz]

    #   Returns x-axis mag reading
    def magx(self):
        return self.priv.data.magx
    
    #   Returns y-axis mag reading
    def magy(self):
        return self.priv.data.magy

    #   Returns z-axis mag reading
    def magz(self):
        return self.priv.data.magz

    #   Returns mag readings in a list: [x, y, z]
    def mag(self):
        return [self.priv.data.magx, self.priv.data.magy, self.priv.data.magz]

    #   Returns heading
    def heading(self):
        return self.priv.data.heading

    def info_firmware_revision(self):
        return self.priv.fwrev

    def info_hardware_revision(self):
        return self.priv.hwrev

    def info_serial_number(self):
        return self.priv.serial

    def info_rssi(self):
        return self.priv.data.rssi

    def info_power_source(self):
        return self.priv.data.power_source

    def info_power_level(self):
        return self.priv.data.power_level

    def info_received_packets(self):
        return self.priv.received_packets

    def wait_for_acks(self, wait):
        self.priv.waiting_for_ack = wait

    def read_configuration(self):
        return self.read_main(JAKE_REG_CONFIG0)

    def write_configuration(self, value):
        return self.write_main(JAKE_REG_CONFIG0, value)

    def read_configuration2(self):
        return self.read_main(JAKE_REG_CONFIG1)

    def write_configuration2(self, value):
        return self.write_main(JAKE_REG_CONFIG1, value)

    def read_sample_rate(self):
        (result, value) = self.read_main(JAKE_REG_CONFIG1)
        if result == JAKE_ERROR:
            return (result, value)

        return (result, value & 0x07)
    
    def write_sample_rate(self, newrate):
        if newrate < JAKE_SAMPLE_RATE_0 or newrate > JAKE_SAMPLE_RATE_120:
            # work out best matching rate
            if newrate < 0:
                newrate = JAKE_SAMPLE_RATE_0
            elif newrate > 120:
                newrate = JAKE_SAMPLE_RATE_120
        else:
            min = 1e06
            minpos = 0
            for i in range(JAKE_OUTPUT_RATE_COUNT):
                if abs(newrate - JAKE_OUTPUT_RATES[i]) < min:
                    min = abs(newrate - JAKE_OUTPUT_RATES[i])
                    minpos = i
            newrate = minpos

        return self.write_main(JAKE_REG_CONFIG1, newrate)

    def read_acc_offset(self):
        pass

    def write_acc_offset(self, value):
        pass

    def read_acc_scale(self):
        pass

    def write_acc_scale(self, value):
        pass

    def read_mag_scale(self):
        pass

    def write_mag_scale(self, value):
        pass

    def read_mag_offset(self):
        pass

    def write_mag_offset(self, value):
        pass

    def get_ack_timeout_ms(self):
        return self.ack_timeout_ms

    def set_ack_timeout_ms(self, new_timeout_ms):
        self.ack_timeout_ms = new_timeout_ms
    
    def read(self, address, hdr):
        # packet: $$R or $$r, 1 byte address, 2 bytes unused and zeroed
        packet = struct.pack("3sBBB", hdr, address, 0, 0)

        if self.priv.waiting_for_ack:
            pyjake_packets("read() already waiting for ack")
            return JAKE_ERROR

        self.priv.write(packet)
        pyjake_packets.debug("SENT: " + str(len(packet)) + " bytes")
    
        self.priv.waiting_for_ack_signal = True
        self.priv.waiting_for_ack = True
    
        timeout = self.ack_timeout_ms
        while timeout > 0 and self.priv.waiting_for_ack_signal:
            sleep(0.01)
            timeout -= 10
        
        self.priv.waiting_for_ack = False
    
        if not self.priv.lastack:
            return (JAKE_ERROR, 0)
    
        self.lastack = False

        return (JAKE_SUCCESS, self.priv.lastval)

    def write(self, address, value, hdr):
        # packet: $$W or $$w, 1 bytes address, 1 byte value, 1 byte unused and zeroed
        packet = struct.pack("3sBBB", hdr, address, value, 0)

        if self.priv.waiting_for_ack:
            pyjake_packets("write() already waiting for ack")
            return JAKE_ERROR
    
        self.priv.write(packet)
        pyjake_packets.debug("SENT: " + str(len(packet)) + " bytes")
        
        self.priv.waiting_for_ack_signal = True
        self.priv.waiting_for_ack = True
    
        timeout = self.ack_timeout_ms
        while timeout > 0 and self.priv.waiting_for_ack_signal:
            sleep(0.01)
            timeout -= 10
        
        pyjake_packets.debug("+++ ACK WAIT OVER timeout = " + str(timeout))
        
        self.priv.waiting_for_ack = False
        if not self.priv.lastack:
            pyjake_packets.debug("write() failed to get ACK")
            return JAKE_ERROR
    
        self.lastack = False

        return JAKE_SUCCESS
    
    def read_main(self, address):
        return self.read(address, "$$R")

    def write_main(self, address, value):
        return self.write(address, value, "$$W")

    def read_bluetooth(self, address):
        return self.read(address, "$$r")

    def write_bluetooth(self, address, value):
        return self.write(address, value, "$$w")


