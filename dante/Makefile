# dante - the udesk reference implementation
# See LICENSE file for copyright and license details.

##### BUILD CONFIGURATION START ######

# paths, standard PREFIX, LIBDIR might be used to install library
# in lib64 or lib32, as desired, for example:
# PREFIX = /usr
# LIBDIR = lib64
# installs in: /usr/lib64
PREFIX = /usr/local
LIBDIR = lib

# SDL2
SDL2CFLAGS = `sdl2-config --cflags 2>/dev/null`
SDL2LDFLAGS = `sdl2-config --libs 2>/dev/null`

# compiler, linker, libtool and install commands
CC = cc
LIBTOOL = libtool
INSTALL = install

##### BUILD CONFIGURATION END   ######

# convenience macros:
VERSION = 0.1
SRC = context.c query.c
HEADERS = dante.h
LIBS = -lm
BUILDFLAGS = ${CFLAGS} -pedantic -Wall -DVERSION=\"${VERSION}\" ${SDL2CFLAGS}
LINKFLAGS = ${LDFLAGS} -s ${LIBS} ${SDL2LDFLAGS}
OUTDIR = ${PREFIX}/${DESTDIR}/${LIBDIR}
OBJ = ${SRC:.c=.o}
LOBJ = ${SRC:.c=.lo}

# rules:
all: options libdante.la

options:
	@echo
	@echo dante build options:
	@echo ========================
	@echo "CFLAGS   = ${BUILDFLAGS}"
	@echo "LDFLAGS  = ${LINKFLAGS}"
	@echo "CC       = ${CC}"
	@echo ========================
	@echo

.c.o:
	${LIBTOOL} --tag=CC --mode=compile ${CC} -c ${BUILDFLAGS} $<

${OBJ}: ${HEADERS}

libdante.la: ${OBJ}
	${LIBTOOL} --tag=CC --mode=link ${CC} -o $@ ${LOBJ} -rpath ${OUTDIR} ${LINKFLAGGS}

clean:
	${LIBTOOL} --mode=clean rm -f libdante.la ${OBJ} ${LOBJ} dante-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p dante-${VERSION}
	@cp -R LICENSE Makefile README ${SRC} ${HEADERS} dante-${VERSION}
	@tar -cf dante-${VERSION}.tar dante-${VERSION}
	@gzip dante-${VERSION}.tar
	@rm -rf dante-${VERSION}

install: all
	${LIBTOOL} --mode=install ${INSTALL} -s -c libdante.la ${OUTDIR}/libdante.la

uninstall:
	${LIBTOOL} --mode=uninstall rm -f ${OUTDIR}/libdante.la

.PHONY: all options clean dist install uninstall
