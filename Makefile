PACKAGE := python-iwlib
VERSION := $(shell python setup.py --version)

bz2:
	git archive --format=tar --prefix=$(PACKAGE)-$(VERSION)/ HEAD | \
	bzip2 -9 > $(PACKAGE)-$(VERSION).tar.bz2

install:
	python setup.py install --root=$(DESTDIR)
