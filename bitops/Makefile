
DESTDIR =
PREFIX  =
BINDIR  = $(PREFIX)/bin
SBINDIR = $(PREFIX)/sbin
 
INSTALL = /usr/bin/install

CC     = gcc
CFLAGS = -O2 -Wall -g

OBJECTS = bitmap.o bitops.o

all: bitops

bitmap.o: bitmap.h

bitops: $(OBJECTS)
	$(CC) -o bitops $(OBJECTS)

clean:
	rm -f bitops $(OBJECTS)

install:
	$(INSTALL) -s bitops $(DESTDIR)$(BINDIR)
