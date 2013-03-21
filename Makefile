# udesk - The Unix Desktop API
# See LICENSE file for copyright and license details.

##### CONFIGURATION START ######

PREFIX = /usr/local
INCDIR = include
UDESKDIR = udesk

INSTALL = install

##### CONFIGURATION END   ######

OUTDIR = ${DESTDIR}/${PREFIX}/${INCDIR}/${UDESKDIR}
SOURCES = include/udesk.h include/udeskext.h include/udeskplatform.h
DESTS = `echo ${SOURCES} | sed -e 's|include|${OUTDIR}|g'`

all: options

options:
	@echo
	@echo udesk configuration options:
	@echo ============================
	@echo "INCLUDES = ${OUTDIR}"
	@echo

install:
	mkdir -p ${OUTDIR}
	${INSTALL} -m 0644 -t ${OUTDIR} ${SOURCES}

uninstall:
	rm -f ${DESTS}
	rmdir ${OUTDIR} 2>/dev/null 1>&2
 
.PHONY: all options install uninstall
