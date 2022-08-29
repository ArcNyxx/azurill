# azurill - init system
# Copyright (C) 2022 ArcNyxx
# see LICENCE file for licensing information

.POSIX:

include config.mk

SRC = azuctl.c azuinit.c azurun.c azustat.c azuwatch.c die.c
HEAD = azustat.h die.h
MAN = azuctl.1 azuinit.1 azurun.1 azustat.1 azuwatch.1
OBJ = $(SRC:.c=.o)

all: azuctl azuinit azurun azustat azuwatch

.c.o:
	$(CC) $(CFLAGS) -c $<

azuctl: azuctl.o die.o
	$(CC) azuctl.o die.o -o $@ $(LDFLAGS)

azuinit: azuinit.o die.o
	$(CC) azuinit.o die.o -o $@ $(LDFLAGS)

azurun: azurun.o die.o
	$(CC) azurun.o die.o -o $@ $(LDFLAGS)

azustat: azustat.o die.o
	$(CC) azustat.o die.o -o $@ $(LDFLAGS)

azuwatch: azuwatch.o die.o
	$(CC) azuwatch.o die.o -o $@ $(LDFLAGS)

clean:
	rm -f azuctl azuinit azurun azustat azuwatch $(OBJ) \
		azurill-$(VERSION).tar.gz

dist: clean
	mkdir -p azurill-$(VERSION)
	cp -f README LICENCE Makefile config.mk $(SRC) $(HEAD) $(MAN) \
		azurill-$(VERSION)
	tar -cf - azurill-$(VERSION) | gzip -c > azurill-$(VERSION).tar.gz
	rm -rf azurill-$(VERSION)

install: all
	mkdir -p $(PREFIX)/bin $(MANPREFIX)/man1
	for TARG in $(MAN) ; do cp -f $${TARG%%.*} $(PREFIX)/bin ; \
		chmod 755 $(PREFIX)/bin/$${TARG%%.*} ; done
	for PAGE in $(MAN) ; do \
		sed 's/VERSION/$(VERSION)/g' < $$PAGE > \
			$(MANPREFIX)/man1/$$PAGE ; \
		chmod 644 $(MANPREFIX)/man1/$$PAGE ; done

uninstall:
	for TARG in $(MAN) ; do rm -f $(PREFIX)/bin/$${TARG%%.*} ; done
	for PAGE in $(MAN) ; do rm -f $(MANPREFIX)/man1/$$PAGE ; done

.PHONY: all clean dist install uninstall
