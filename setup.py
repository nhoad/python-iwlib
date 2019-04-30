#!/usr/bin/env python2

import os
import sys

from setuptools import setup

# Publish Helper.
if sys.argv[-1] == 'publish':
    os.system('python setup.py sdist upload')
    sys.exit()

include_dirs = ['iwlib']

dependencies = ['cffi>=1.0.0']
cffi_modules = ['iwlib/_iwlib_build.py:ffibuilder']

settings = {
    'name': 'iwlib',
    'version': '1.6.2',
    'description': "Python module to interface with iwlib",
    'long_description': open('README.rst').read(),
    'author': 'Nathan Hoad',
    'author_email': 'nathan@getoffmalawn.com',
    'url': 'https://github.com/nathan-hoad/python-iwlib',
    'license': 'GPLv2',
    'classifiers': [
        # 'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Natural Language :: English',
        'License :: OSI Approved :: GNU General Public License v2 (GPLv2)',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2.6',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: Implementation :: PyPy',
        'Programming Language :: Python :: Implementation :: CPython',
    ],
    'zip_safe': False,
    'packages': ['iwlib'],
    'install_requires': dependencies,
    'setup_requires': dependencies,
    'cffi_modules': cffi_modules,
}

setup(**settings)
