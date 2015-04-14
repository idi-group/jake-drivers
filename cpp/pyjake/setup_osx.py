#!/usr/bin/env python

from distutils.core import setup, Extension
from distutils.command.install import INSTALL_SCHEMES
import sys, os
import datetime
today = datetime.date.today()

for scheme in INSTALL_SCHEMES.values():
	scheme['data'] = scheme['platlib']

setup(name='jake',
		version='%02d%02d%04d' % (today.day, today.month, today.year),
		description='Python module for the JAKE sensor pack',
		author='University of Glasgow',
		author_email='andrew.ramsay@glasgow.ac.uk',
		url='http://github.com/andrewramsay/jake-drivers',
		py_modules = ['jake'],
		package_dir = {'':'./python_wrapper'},
		data_files = [('/usr/local/lib', ['../jake_driver/libjake_driver.so'])],
		ext_modules = 
		[
			Extension('pyjake', ['src/pyjake.cpp'], include_dirs=['../jake_driver/inc', 'inc'], library_dirs=['../jake_driver'], libraries=[ 'jake_driver'], define_macros = [('PYJAKE_COMPILE', '1')]
			)
		]
)
