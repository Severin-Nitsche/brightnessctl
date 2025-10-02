include config.mk

PROGRAM = brightnessctl
VERSION = 0.6

CFLAGS += -std=c99 -g -Wall -Wextra -DVERSION=\"${VERSION}\" -D_POSIX_C_SOURCE=200809L -Iinclude
LDLIBS += -lm

SRCDIR = src
BUILDDIR = build
BINDIR = ${DESTDIR}${PREFIX}/bin
MANDIR = ${DESTDIR}${PREFIX}/share/man

INSTALL_UDEV_1 = install_udev_rules

SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(SRCS:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

all: $(BUILDDIR)/$(PROGRAM) $(PROGRAM).1

$(BUILDDIR)/$(PROGRAM): $(OBJS)
	$(CC) $(LDFLAGS) $^ $(LOADLIBES) $(LDLIBS) -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(BUILDDIR)
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $^ -o $@

install: all ${INSTALL_UDEV_${INSTALL_UDEV_RULES}}
	install -d ${BINDIR} ${MANDIR}/man1
	install -m ${MODE} ${BUILDDIR}/${PROGRAM} ${BINDIR}/
	install -m 0644 ${PROGRAM}.1 ${MANDIR}/man1

install_udev_rules:
	install -d ${DESTDIR}${UDEVDIR}
	install -m 0644 90-${PROGRAM}.rules ${DESTDIR}${UDEVDIR}

clean:
	rm -rf $(BUILDDIR)

.PHONY: all install clean install_udev_rules dir
