#ifndef BRIGHTNESS_UTIL_H
#define BRIGHTNESS_UTIL_H

#include "brightnesslib.h"
#include "brightnessctl.h"
#define printfc(F,...) if (!options->quiet && !options->machine_readable) printf(F,__VA_ARGS__)
#define printc(S) if (!options->quiet && !options->machine_readable) printf(S)

extern void print_device(struct device *, struct options *);
extern float value_in_percent(int, struct device *, struct options *);
extern int percent_in_value(float, struct device *, struct options *);

#endif
