from __future__ import print_function

def test_imports_dont_fail():
    '''Gross test for ensuring packaging hasn't been broken.'''
    imports = [
        "iwlib.iwconfig.get_iwconfig",
        "iwlib.iwlist.scan",
        "iwlib.utils.get_max_quality",
        "iwlib.utils.set_essid",
        "iwlib.utils.supports_scanning",
    ]

    for imp in imports:
        module, _from = imp.rsplit('.', 1)
        print('from %s import %s' % (module, _from))
        __import__(module, fromlist=[_from])


def test_backwards_compatibility():
    '''Ensure backwards compatibility with the module before I came along.'''
    from iwlib import get_iwconfig
