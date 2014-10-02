#!/usr/bin/env python2

import os
import sys

try:
    from distutils.core import setup, Extension, find_packages
except ImportError:
    from setuptools import setup, Extension, find_packages

# Publish Helper.
if sys.argv[-1] == 'publish':
    os.system('python setup.py sdist upload')
    sys.exit()

include_dirs = ['iwlib']
libraries = ["iw"]

ext_modules = [
    Extension("iwlib.iwconfig", sources=['iwlib/utils.c', 'iwlib/iwconfig.c'],
              include_dirs=include_dirs, libraries=libraries),

    Extension("iwlib.iwlist", sources=['iwlib/iwlist.c', 'iwlib/utils.c'],
              include_dirs=include_dirs, libraries=libraries),

    Extension("iwlib.utils", sources=['iwlib/utils.c'],
              include_dirs=include_dirs, libraries=libraries),
]

settings = {
    'name': 'iwlib',
    'version': '1.5',
    'description': "Python module to interface with iwlib",
    'long_description': open('README.rst').read(),
    'author': 'Nathan Hoad',
    'author_email': 'nathan@getoffmalawn.com',
    'url': 'https://github.com/nathan-hoad/python-iwlib',
    'license': 'GPLv2',
    'classifiers': (
        # 'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Natural Language :: English',
        'License :: OSI Approved :: GNU General Public License v2 (GPLv2)',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
    ),
    'ext_modules': ext_modules,
    'packages': find_packages(),
}

setup(**settings)
