CFLAGS="-fPIC -fno-stack-protector -Wno-write-strings -arch x86_64 -mmacosx-version-min=10.5 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk -Iinc"
LDFLAGS="-lm -lpthread"
CPP="/usr/bin/g++"
LIBJAKE="libjake_driver.so"

rm -f $LIBJAKE

$CPP -o $LIBJAKE -shared $CFLAGS src/jake_driver.cpp src/jake_packets.cpp src/jake_serial.cpp src/jake_thread.cpp

