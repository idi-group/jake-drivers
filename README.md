# JAKE Drivers

This project contains drivers for the JAKE sensor packs. For more information on the devices and their capabilities, [see the wiki](https://github.com/andrewramsay/jake-drivers/wiki/Overview).

This repo contains multiple drivers. The most feature complete implementation is a C++ driver that can be compiled for Windows, OSX and Linux (32/64-bit). This driver also has bindings for Python, C# and Java, although in some cases functionality supported by the C++ code has not yet been exposed through these bindings.

In addition, there are two separate implementations, one written in Python (with a dependency on [pyserial](http://pyserial.sourceforge.net/)) and one written in Java (probably most useful in that it supports Android). 

**NOTE**: If you install both the Python/C++ bindings and the pure Python implementation in the same Python environment you may encounter naming conflicts, as both include a module called "pyjake".

## Simple installation

If you only need the Python driver (only depends on pyserial and should work on all platforms that it supports), you can now install the package via pip: `pip install jake-drivers`

## Installation on Windows

The easiest way to get up and running quickly on Windows is to use the Python implementation. Install pyserial, then run `python setup.py install` from the `jake-drivers/python` directory. 

If you want to use the C++ driver, you'll need to compile it first. Inside the `jake-drivers/cpp` directory, you'll find:

 * jake\_driver (the C++ code)
 * pyjake (Python bindings for jake\_driver)
 * jake\_cs (C# bindings for jake\_driver)
 * jake\_java (Java bindings for jake\_driver)
 * jake\_mex (Basic Matlab bindings for jake\_driver)

Each of these should have a Visual Studio project file that can be used to compile them individually depending on which parts you need. They will probably also compile with gcc/g++ from MingW, but there are no build scripts for this at the moment. 

## Installation on OSX

Again the Python implementation is the easiest way to get started quickly:

 * Install pyserial
 * Run `python setup.py install` from `jake-drivers/python`

If you have the XCode/developer packages installed and want to compile the C++/Python bindings:

 * Run `sh build_osx_64.sh` from `jake-drivers/cpp/jake_driver`
 * Run `python setup_osx.py install` from `jake-drivers/cpp/pyjake`

## Installation on Linux

Similar to OSX. For the Python implementation:

 * Install pyserial
 * Run `python setup.py install` from `jake-drivers/python`

For the C++/Python bindings:

 * Run `sh build.sh` from `jake-drivers/cpp/jake_driver`
 * Run `python setup.py install` from `jake-drivers/cpp/pyjake`

## A basic example

(for more examples see [the wiki page](https://github.com/andrewramsay/jake-drivers/wiki/Examples)

**Python / pyserial**

```python
from pyjake import *
jd = jake_device()

# connection details depend on platform
# Windows (virtual COM port number)
device = 10
# OSX (Serial Port Profile dev node)
# device = '/dev/tty.JAKESN0077-SPP'

jd.connect(device) # returns False if connection failed
print jd.acc() # current accelerometer output
print jd.mag() # current magnetometer output
print jd.heading() # current heading output
jd.close()
```

**Python/C++ bindings**
```python
from jake import *
jd = jake_device()

# Windows: for non-Microsoft Bluetooth stacks
jd.connect(10) 

# Windows: if you're using the Microsoft Bluetooth stack, 
# you can instead connect by specifying the device address directly
jd.connect_rfcomm('aa:bb:cc:dd:ee:ff')

# OSX: always use this function
# TODO

# Linux: Bluetooth device address
jd.connect_rfcomm('aa:bb:cc:dd:ee:ff')

# access data...
print jd.acc(), jd.mag(), jd.heading()

jd.close()
```


