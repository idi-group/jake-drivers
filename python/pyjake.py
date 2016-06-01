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

class jake_device:
    """An instance of this class represents a single JAKE device.
    This naming will be removed, use JakeDevice instead.
    """

    def __init__(self):
        self.priv = pyjake_packets.jake_device_private()
        self.ack_timeout_ms = 500

    @property
    def connected(self):
        """use this to know when the jake is connected
        This is connected to the thread in priv
        """
        return not self.priv.thread_done

    def connect(self, addr):
        """
        Connect to a JAKE over a Bluetooth serial connection. 

        :param addr: any valid pyserial address, eg 'COM9:' on Windows or 
            '/dev/tty.JAKEJK8SN0015-SPP' on OSX. 
        :returns: True if the connection was created, False on error.
        """
        if self.priv.port != None:
            self.priv.close()

        return self.priv.connect(JAKE_CONN_TYPE_SERIAL_PORT, (addr,))

    def connect_debug(self, inputfile, eof_callback=None, outputfile=None):
        """
        Allows you to configure the the driver to read from a local file rather
        than a serial port. Probably only useful for debugging. 

        :param inputfile: filename of the local file containing JAKE data 
            packets (in their original binary format).
        :param eof_callback: optional callable to be called when the end of the
            inputfile is reached (can be used to seek back to the start of the
            file and continue parsing from there).
        :param outputfile: optional filename for a local file where data packets
            normally transmitted to the JAKE will be written.
        :returns: True on success, False otherwise.
        """
        if self.priv != None:
            self.priv.close()

        if not os.path.exists(inputfile):
            return False

        inputfilefp = open(inputfile, "rb")
        if not outputfile:
            outputfilefp = sys.stdout
        else:
            outputfilefp = open(outputfile, "w")

        self.priv.connect(JAKE_CONN_TYPE_DEBUG_FILE, (inputfilefp, outputfilefp, eof_callback))

        return True

    def close(self):
        """
        Close any active connection.
        """
        if self.priv:
            self.priv.close()

    def register_data_callback(self, cb):
        """
        Registers a callable to be called when each new data packet arrives from
        the JAKE. 
        
        :param cb: function to be called for each new data packet. It should 
            expect 4 parameters:
            - accelerometer data [x, y, z] (list)
            - magnetometer data [x, y, z] (list)
            - heading (int)
            - timestamp (int)
        """
        self.priv.data_callback = cb

    def data_timestamp(self):
        """
        Provides the sequence number from the last received data packet.

        :returns: the sequence number (internally a 16-bit int)
        """
        return self.priv.data.timestamp

    def accx(self):
        """
        Latest x-axis accelerometer reading.

        :returns: x-axis accelerometer reading
        """
        return self.priv.data.accx

    def accy(self):
        """
        Latest y-axis accelerometer reading.

        :returns: y-axis accelerometer reading
        """
        return self.priv.data.accy

    def accz(self):
        """
        Latest z-axis accelerometer reading.

        :returns: z-axis accelerometer reading
        """
        return self.priv.data.accz

    def acc(self):
        """
        Latest accelerometer readings.

        :returns: accelerometer readings as a 3-element list ([x, y, z])
        """
        return [self.priv.data.accx, self.priv.data.accy, self.priv.data.accz]

    def magx(self):
        """
        Latest x-axis magnetometer reading.

        :returns: x-axis magnetometer reading
        """
        return self.priv.data.magx
    
    def magy(self):
        """
        Latest y-axis magnetometer reading.

        :returns: y-axis magnetometer reading
        """
        return self.priv.data.magy

    def magz(self):
        """
        Latest z-axis magnetometer reading.

        :returns: z-axis magnetometer reading
        """
        return self.priv.data.magz

    #   Returns mag readings in a list: [x, y, z]
    def mag(self):
        """
        Latest magnetometer readings.

        :returns: magnetometer reading as a 3-element list ([x, y, z])
        """
        return [self.priv.data.magx, self.priv.data.magy, self.priv.data.magz]

    #   Returns heading
    def heading(self):
        """
        Latest compass heading sensor reading.

        :returns: reading in tenths of a degree (0-3599)
        """
        return self.priv.data.heading

    def info_firmware_revision(self):
        """
        TODO
        """
        return self.priv.fwrev

    def info_hardware_revision(self):
        """
        TODO
        """
        return self.priv.hwrev

    def info_serial_number(self):
        """
        TODO
        """
        return self.priv.serial

    def info_rssi(self):
        """
        Each data packet sent by the JAKE contains an RSSI field. This indicates
        the current receive signal strength of the Bluetooth connection at the
        JAKE end. This can be used to monitor the connection quality.

        :returns: the signal strength as a signed value in units of dBm. The 
            user manual states any value below roughly -90dBm would indicate
            a very weak signal. 
        """
        return self.priv.data.rssi

    def info_external_power(self):
        """
        Indicates if the device is currently running off external (USB) power
        or off the internal battery. This value is updated by each data packet
        sent from the JAKE.

        :returns: True if running on USB power, False otherwise
        """
        return self.priv.data.power_source

    def info_power_level(self):
        """
        Gives the current battery level of the connected JAKE as a percentage.

        :returns: the battery charge level from 0-99
        """
        return self.priv.data.power_level

    def wait_for_acks(self, wait):
        self.priv.waiting_for_ack = wait

    def read_configuration(self):
        """
        Shortcut function for reading config register 0 on the JAKE. This 
        register controls various aspects of the 3 main sensors (power state,
        calibration, filter order, acceleration range). See the user manual
        for details.

        :returns: a 2-tuple (result, value). Result will be either JAKE_ERROR
            or JAKE_SUCCESS. If result is JAKE_SUCCESS, value will be the
            current state of the register. 
        """
        return self.read_main(JAKE_REG_CONFIG0)

    def write_configuration(self, value):
        """
        Shortcut function for writing config register 0 on the JAKE. This 
        register controls various aspects of the 3 main sensors (power state,
        calibration, filter order, acceleration range). See the user manual 
        for details.

        :param value: the value to write into the register, see the user manual
            for information on which bits you should set. 
        :returns: JAKE_SUCCESS or JAKE_ERROR
        """
        return self.write_main(JAKE_REG_CONFIG0, value)

    def read_configuration2(self):
        """
        Shortcut function for reading config register 1 on the JAKE. This 
        register is mostly used for controlling the output rate of the JAKE
        data packets. 

        :returns: a 2-tuple (result, value). Result will be either JAKE_ERROR
            or JAKE_SUCCESS. If result is JAKE_SUCCESS, value will be the
            current state of the register. 
        """
        return self.read_main(JAKE_REG_CONFIG1)

    def write_configuration2(self, value):
        """
        Shortcut function for writing config register 1 on the JAKE. This
        register is mostly used for controlling the output rate of the JAKE
        data packets.

        :param value: the value to write into the register, see the user manual
            for information on which bits you should set.
        :returns: JAKE_SUCCESS or JAKE_ERROR
        """
        return self.write_main(JAKE_REG_CONFIG1, value)

    def read_sample_rate(self):
        """
        Reads the register containing the current sample rate.

        :returns: a 2-tuple containing (result, value), where result is either
            JAKE_ERROR or JAKE_SUCCESS. If result == JAKE_SUCCESS, value will
            be set to the current value of the register. You can then check the
            current sample rate by matching the value against the various
            constants defined by the driver, eg JAKE_SAMPLE_RATE_60
        """
        (result, value) = self.read_main(JAKE_REG_CONFIG1)
        if result == JAKE_ERROR:
            return (result, value)

        return (result, value & 0x07)
    
    def write_sample_rate(self, newrate):
        """
        Updates the register controlling the current sensor sample rate. 

        :param newrate: the new sample rate (integer). You can either supply
            one of the predefined constants (eg JAKE_SAMPLE_RATE_60), or any
            value between 0 and 120Hz. The JAKE only supports 6 predefined 
            sample rates, so any other values will be mapped to the nearest
            of the predefined set. 
        :return: JAKE_SUCCESS or JAKE_ERROR
        """
        if newrate < JAKE_OUTPUT_RATES[JAKE_SAMPLE_RATE_0] or newrate > JAKE_OUTPUT_RATES[JAKE_SAMPLE_RATE_120]:
            # map any negative values to 0Hz and any overlarge values to 120Hz
            if newrate < 0:
                newrate = JAKE_SAMPLE_RATE_0
            elif newrate > 120:
                newrate = JAKE_SAMPLE_RATE_120
        elif newrate >= JAKE_SAMPLE_RATE_0 and newrate <= JAKE_SAMPLE_RATE_120:
            pass # assume a predefined constant has been passed in
        else:
            # for other values, work out the best match among the supported 
            # sample rates and use that
            index = 0
            while index < len(JAKE_OUTPUT_RATES) - 1:
                if newrate < JAKE_OUTPUT_RATES[index]:
                    break
                index += 1

            if JAKE_OUTPUT_RATES[index] - newrate > newrate - JAKE_OUTPUT_RATES[index - 1]:
                newrate = index - 1
            else:
                newrate = index

        return self.write_main(JAKE_REG_CONFIG1, newrate)

    def get_ack_timeout_ms(self):
        """
        Get the current time in milliseconds the driver will wait for responses
        to read/write commands.

        :returns: the current timeout in milliseconds
        """
        return self.ack_timeout_ms

    def set_ack_timeout_ms(self, new_timeout_ms):
        """
        Set the time in milliseconds the driver will wait for responses to 
        read/write commands. Try increasing this if read/write commands seem
        to return errors for no reason, especially if you're streaming a lot
        of sensor data from the device.

        :param new_timeout_ms: the new timeout in milliseconds
        """
        self.ack_timeout_ms = new_timeout_ms
    
    def _read(self, address, hdr):
        # packet: $$R or $$r, 1 byte address, 2 bytes unused and zeroed
        packet = struct.pack("3sBBB", hdr, address, 0, 0)

        if self.priv.waiting_for_ack:
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

    def _write(self, address, value, hdr):
        # packet: $$W or $$w, 1 bytes address, 1 byte value, 1 byte unused and zeroed
        packet = struct.pack("3sBBB", hdr, address, value, 0)

        if self.priv.waiting_for_ack:
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
            return JAKE_ERROR
    
        self.lastack = False

        return JAKE_SUCCESS
    
    def read_main(self, address):
        """
        Reads one of the main JAKE configuration registers. 

        :param address: the address of the register to read from (see the user
            manual for details).
        :returns: a 2-tuple (result, value). result will be either JAKE_ERROR 
            or JAKE_SUCCESS. If result is JAKE_SUCCESS then value will give the
            current value of the register.
        """
        return self._read(address, "$$R")

    def write_main(self, address, value):
        """
        Writes to one of the main JAKE configuration registers.

        :param address: the address of the register to write into (see the user
            manual for details).
        :param value: the new value to be written to the register.
        :returns: JAKE_SUCCESS or JAKE_ERROR
        """
        return self._write(address, value, "$$W")

    def read_bluetooth(self, address):
        """
        Reads one of the JAKE Bluetooth microcontroller registers.

        :param address: the address of the register to read from (see the user
            manual for details).
        :returns: a 2-tuple (result, value). result will be either JAKE_ERROR 
            or JAKE_SUCCESS. If result is JAKE_SUCCESS then value will give the
            current value of the register.
        """
        return self._read(address, "$$r")

    def write_bluetooth(self, address, value):
        """
        Writes to one of the JAKE Bluetooth microcontroller registers.

        :param address: the address of the register to write into (see the user
            manual for details).
        :param value: the new value to be written to the register.
        :returns: JAKE_SUCCESS or JAKE_ERROR
        """
        return self._write(address, value, "$$w")

class JakeDevice(jake_device):
    """Wrapper around jake_device class.
    Respect the naming convention.
    """
    pass
