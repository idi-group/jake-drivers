# Copyright (c) 2006-2015, University of Glasgow
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#       * Redistributions of source code must retain the above copyright notice, this list of 
#           conditions and the following disclaimer.
#       * Redistributions in binary form must reproduce the above copyright notice, this list
#           of conditions and the following disclaimer in the documentation and/or other
#           materials provided with the distribution.
#       * Neither the name of the University of Glasgow nor the names of its contributors 
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

import time, sys

from pyjake import *

def jake_data_callback(acc, mag, heading, timestamp):
    print "Callback data:", acc, mag, heading, timestamp

if __name__ == "__main__":

    if len(sys.argv) != 2:
        print "pyjake_test.py <device address>"
        print ""
        print "Examples:"
        print "pyjake_test.py 4 (connect to COM port 5 on Windows (pyserial port numbers are 0-based))"
        sys.exit(-1)

    jd = jake_device()

    try:
        param = int(sys.argv[1])
    except ValueError:
        param = sys.argv[1]

    if not jd.connect(param):
        print "Failed to connect!"
        sys.exit(-1)

    time.sleep(1)

    # set sample rate (applies to all sensors). The rate can only be set to
    # certain fixed values: 0, 7.5, 15, 30, 60, 120Hz. You can either pass this
    # function a constant enum value (eg JAKE_SAMPLE_RATE_60) or an arbitrary 
    # number. in the latter case it will attempt to work out the closest 
    # supported sample rate and use that. 
    jd.write_sample_rate(60)

    # display some data, method 1
    for i in range(200):
        print jd.acc(), jd.mag(), jd.heading(), jd.data_timestamp()
        time.sleep(0.01)

    # now register a callback for new data packets arriving and display
    # data arriving there
    jd.register_data_callback(jake_data_callback)
    time.sleep(5)

    jd.close()
