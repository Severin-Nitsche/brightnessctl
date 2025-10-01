include config.mk

VERSION = 0.6
CFLAGS += -std=c99 -g -Wall -Wextra -DVERSION=\"${VERSION}\" -D_POSIX_C_SOURCE=200809L
LDLIBS += -lm

BINDIR = ${DESTDIR}${PREFIX}/bin
MANDIR = ${DESTDIR}${PREFIX}/share/man

INSTALL_UDEV_1 = install_udev_rules

all: brightnessctl brightnessctl.1

brightnessctl: pathutil.o brightnesslib.o brightnessprog.o brightnessutil.o restoreutil.o

install: all ${INSTALL_UDEV_${INSTALL_UDEV_RULES}}
	install -d ${BINDIR} ${MANDIR}/man1
	install -m ${MODE} brightnessctl   ${BINDIR}/
	install -m 0644    brightnessctl.1 ${MANDIR}/man1

install_udev_rules:
	install -d ${DESTDIR}${UDEVDIR}
	install -m 0644 90-brightnessctl.rules ${DESTDIR}${UDEVDIR}

clean:
	rm -f brightnessctl
	rm -f *.o

.PHONY: all install clean install_udev_rules
