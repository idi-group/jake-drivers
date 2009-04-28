from pyjake import *
import time

jd = jake_device()
jd.connect(5) # COM 6

jd.write_main(JAKE_REG_CONFIG1, JAKE_SAMPLE_RATE_30)
for i in range(150):
	time.sleep(0.01)
	print jd.acc(), jd.mag(), jd.heading()

jd.close()
