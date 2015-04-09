CFLAGS="-fPIC -fno-stack-protector -Wno-write-strings"
LDFLAGS="-lm -lbluetooth -lpthread"
LIBJAKE="libjake_driver.so"

rm -f $LIBJAKE

/usr/bin/g++ $CFLAGS -Iinc -shared -o $LIBJAKE $LDFLAGS src/jake_driver.cpp src/jake_packets.cpp src/jake_rfcomm.cpp src/jake_serial.cpp src/jake_thread.cpp
