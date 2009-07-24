#!/usr/bin/python2

from distutils.core import setup, Extension

iwlib = Extension("iwlib",
                  sources = ["python-iwlib/iwlib.c"],
                  libraries=["iw"])

# don't reformat this line, Makefile parses it
setup(name="iwlib",
      version="1.0",
      description="Python module to interface with iwlib",
      author="Harald Hoyer",
      author_email="harald@redhat.com",
      url="http://fedoraproject.org/wiki/python-iwlib",
      long_description = """The iwlib module provides functions to
                            examine the wireless network devices installed on the system.""",
      ext_modules=[iwlib])
