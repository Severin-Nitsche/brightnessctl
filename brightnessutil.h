#ifndef BRIGHTNESS_UTIL_H
#define BRIGHTNESS_UTIL_H

#include "brightnesslib.h"
#include "brightnessctl.h"
#define printfc(F,...) if (!options->quiet && !options->machine_readable) printf(F,__VA_ARGS__)
#define printc(S) if (!options->quiet && !options->machine_readable) printf(S)

extern void print_device(struct device *, struct options *);
extern void print_and_save_device(struct device *, struct options *);
extern struct device *find_device(struct options *, int *);
extern float value_to_percent(float, struct device *, struct options *);
extern unsigned int percent_to_value(float, struct device *, struct options *);
extern int find_value(struct device *, struct options *);
extern float find_base(int *, unsigned int, int delta, struct device *, struct options *);

#endif
