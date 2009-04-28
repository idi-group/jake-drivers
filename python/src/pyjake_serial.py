# JAKE serial port wrapper class 
import os

class pyjake_serial_error(Exception):
	def __init__(self, value):
		self.value = value
	def __str__(self):
		return repr(self.value)

import sys
if sys.version[:3] == "2.2":
	import socket as btsocket
else:
	import btsocket
import e32
import appuifw
from e32 import ao_yield
from e32 import ao_sleep
# this should allow the phone to redraw the screen after importing the modules, which
# can take some time depending on the phone
ao_yield()

# basic serial port interface class. 
class base_serial_port:
	def __init__(self):
		pass

	def open(self):
		pass

	def close(self):
		pass

	def read(self, bytes_to_read):
		pass

	def write(self, bytes):
		pass



# 	Class for the Nokia S60 platform. Give it a BT address and it tries to open
# 	a serial connection to that device.
class serial_port(base_serial_port):

	def __init__(self, bt_addr):
		self.target = None
		self.connected = False
		#print "port __init__"
		if bt_addr.find(":") == -1:
			#print "Using AF_INET socket"
			self.sock = btsocket.socket(btsocket.AF_INET, btsocket.SOCK_STREAM)
			t = bt_addr.split("|")
			self.target = (t[0], int(t[1]))
		else:
			#print "Using AF_BT socket"
			self.sock = btsocket.socket(btsocket.AF_BT, btsocket.SOCK_STREAM)
			self.target = (bt_addr, 1)

	def open(self):
		if self.connected:
			appuifw.note(u'Already connected!')
			return False

		#print "Calling socket.connect(" + str(self.target) + ")"
		try:
			self.sock.connect(self.target)
		except:
			#print "Connect failed"
			import sys, traceback
			f = open("c:/socket.txt", "w")
			f.write("\n".join(traceback.format_exception(*sys.exc_info())))
			f.close()
		#self.sock.connect(self.target)
		#appuifw.note(u'Connected')
		#print "Connected!"
		self.connected = True
		return True

	def close(self):
		if not self.connected:
			appuifw.note(u'Already disconnected!')
			return False

		
		self.sock.shutdown(2)
		self.sock = None
		#appuifw.note(u'Disconnected')
		self.target = None
		return True

	def read(self, bytes_to_read):
		if not self.sock:
			return ""
		bytes_read = 0
		data = ""
		while bytes_read < bytes_to_read:
			newdata = self.sock.recv(bytes_to_read - bytes_read)
			bytes_read += len(newdata)
			data += newdata
		return data

	def write(self, bytes):
		if not self.sock:
			return 0
		sentbytes = 0
		bytes_to_send = len(bytes)
		#while sentbytes < bytes_to_send:
		#	sentbytes += self.sock.send(bytes[sentbytes:])
		sentbytes = self.sock.send(bytes)
		return sentbytes

