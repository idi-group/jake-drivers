#!/usr/bin/env python

from distutils.core import setup, Extension
from distutils.command.install import INSTALL_SCHEMES
import sys, os
import datetime
today = datetime.date.today()

for scheme in INSTALL_SCHEMES.values():
    scheme['data'] = scheme['platlib']

setup(name='pyjake',
        version='%02d%02d%04d' % (today.day, today.month, today.year),
        description='Python module for JAKE sensor pack',
        author='University of Glasgow',
        author_email='andrew.ramsay@glasgow.ac.uk',
        url='http://github.com/andrewramsay/jake-drivers',
        py_modules = ['pyjake', 'pyjake_constants', 'pyjake_serial_pc', 'pyjake_packets'],
)
