
DESTDIR =
PREFIX  =
BINDIR  = $(PREFIX)/bin
SBINDIR = $(PREFIX)/sbin
 
INSTALL = /usr/bin/install

CC      = gcc
CFLAGS  = -O2 -Wall -g

SUBDIRS = bitops

all:
	

clean:
	rm -f bitops $(OBJECTS)

install:
	$(INSTALL) -s bitops $(DESTDIR)$(BINDIR)
