#ifndef BRIGHTNESS_LIB_H
#define BRIGHTNESS_LIB_H

#include <stdbool.h>
#include <stdio.h>

struct device {
  char *class;
  char *id;
  unsigned int curr_brightness;
  unsigned int max_brightness;
  bool matches;
};

#ifdef ENABLE_LOGIND
extern bool logind_set_brightness(struct device *);
#endif

extern bool do_write_device(struct device *);

extern int read_devices(struct device **);
extern int read_class(struct device **, char *, bool);
extern bool read_device(struct device *, char *, char *);

#endif
