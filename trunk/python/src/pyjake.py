import pyjake_packets, atexit, struct
from pyjake_constants import *
import math

# custom exception class for JAKE code
class jake_error(Exception):
	def __init__(self, value):
		self.value = value
	def __str__(self):
		return repr(self.value)

# this function is registered to be called on exit from a Python 
# shell, and just calls the close method of any active jake_device
# instances
def cleanup():
	for i in jake_device.instances:
		if i != None:
			i.close()

atexit.register(cleanup)

# an instance of this class represents a single JAKE device
class jake_device:
	jake_handle_count 	= 0
	instances = []

	def __init__(self):
			
		self.handle = jake_device.jake_handle_count
		jake_device.jake_handle_count += 1

		jake_device.instances.append(self)
		self.priv = None

	def connect(self, addr):
		if self.priv != None:
			self.priv.close()
			self.priv = None

		self.priv = pyjake_packets.jake_device_private(addr)

		elapsed = 0
		while not self.priv.synced and not self.priv.thread_done and elapsed < 10000:
			pyjake_packets.sleep(0.05)
			elapsed += 50

		if not self.priv.synced or elapsed >= 10000:
			raise jake_error("Failed to sync!")

		return True

	def dbgmode(self):
		return self.priv.dbgmode
		
	def close(self):
		if self.priv:
			self.priv.close()
		try:
			jake_device.instances.remove(self)
		except:
			pass

	def data_timestamp(self):
		if sensor < JAKE_SENSOR_ACC or sensor > JAKE_SENSOR_ANA1:
			return -1

		return self.priv.data.timestamp

	# 	Returns x-axis accelerometer reading
	def accx(self):
		return self.priv.data.accx

	# 	Returns y-axis accelerometer reading
	def accy(self):
		return self.priv.data.accy

	# 	Returns z-axis accelerometer reading
	def accz(self):
		return self.priv.data.accz

	# 	Returns accelerometer readings in a list: [x, y, z]
	def acc(self):
		return [self.priv.data.accx, self.priv.data.accy, self.priv.data.accz]

	# 	Returns x-axis mag reading
	def magx(self):
		return self.priv.data.magx
		
	# 	Returns y-axis mag reading
	def magy(self):
		return self.priv.data.magy

	# 	Returns z-axis mag reading
	def magz(self):
		return self.priv.data.magz

	# 	Returns mag readings in a list: [x, y, z]
	def mag(self):
		return [self.priv.data.magx, self.priv.data.magy, self.priv.data.magz]

	# 	Returns heading
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

		return (result, value & 0x7F)
	
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

	def write_mag_scale(self, value):
		pass

	def read(self, address, hdr):
		# packet: $$R or $$r, 1 byte address, 2 bytes unused and zeroed
		packet = struct.pack("3sBBB", hdr, address, 0, 0)

		if self.priv.waiting_for_ack:
			pyjake_packets("read() already waiting for ack")
			return JAKE_ERROR

		self.priv.write_to_port(packet)
		pyjake_packets.debug("SENT: " + str(len(packet)) + " bytes")
		
		self.priv.waiting_for_ack_signal = True
		self.priv.waiting_for_ack = True
		
		timeout = 250
		while self.priv.waiting_for_ack_signal:
			pyjake_packets.ssleep(0.001)
			timeout -= 1
			if timeout == 0:
				break
			
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
		
		self.priv.write_to_port(packet)
		pyjake_packets.debug("SENT: " + str(len(packet)) + " bytes")
		
		self.priv.waiting_for_ack_signal = True
		self.priv.waiting_for_ack = True
		
		timeout = 250
		while timeout != 0 and self.priv.waiting_for_ack_signal:
			pyjake_packets.ssleep(0.001)
			timeout -= 1
			
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


