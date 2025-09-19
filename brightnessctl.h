#ifndef BRIGHTNESSCTL_H
#define BRIGHTNESSCTL_H

#include <stdbool.h>

enum operation { UNSET, GET, SET };

struct options {
  bool quiet;
  bool pretend;
  bool machine_readable;
  bool help;
  bool version;
  bool save;
  bool restore;
  bool list;
  int exponent;
  unsigned int min_value;
  enum operation operation;
  char *set_to;
  char *device;
  char *class;
};

extern void print_help(struct options *);
extern void print_version();
extern int get(struct options *);
extern int set(struct options *);

#endif
