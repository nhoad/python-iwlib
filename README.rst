iwlib - interface with wireless tools in Linux
==============================================

iwlib is a package for interfacing with iwlib, providing an implementation to
the wireless tools in Linux.

While very incomplete at the moment, it aims to eventually become a complete
implementation, as features become necessary. If you find some functionality
missing, feel free to contribute to the project, or create an issue on the bug
tracker.

Currently it provides what I consider the bare minimum to become useful -
scanning, setting the ESSID of a device, and getting the current configuration
back from a device.

http://www.hpl.hp.com/personal/Jean_Tourrilhes/Linux/Tools.html

Installation
------------

Installation requires a compiler recognised by Python::

    $ pip install iwlib

Change History
--------------

1.3 (24th March 2013)
    - Added ``iwlib.utils module``. See documentation for details.
    - Added ``iwlib.iwlist module``. See documentation for details.
    - Added function ``set_essid``, to ``iwlib.iwconfig``.
1.2.1 (21st March 2013)
    - Initial release of code, as it was when I inherited it.
