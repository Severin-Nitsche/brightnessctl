#include "brightnessutil.h"
#include "restoreutil.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>

#define EPS .00001

void print_device(struct device *device, struct options *options) {
  char *format = options->machine_readable ?
    "%s,%s,%d,%f%%,%d\n" :
    "Device '%s' of class '%s':\n"
    "  Current brightness: %d (%.2f%%)\n"
    "  Maximum brightness: %d\n\n";
  printf(
    format,
    device->id,
    device->class,
    device->curr_brightness,
    value_to_percent(device->curr_brightness, device, options),
    device->max_brightness
  );
}

void print_and_save_device(struct device *device, struct options *options) {
  print_device(device, options);
  if (options->save)
    save_device_data(device);
}

struct device *find_device(struct options *options, int *error) {
  if (!options->class || !options->device) {
    *error = 1;
    printfc(
      "A device complete specification of a device requires both of:\n"
      " --class and --device.\n"
      "You specified:\n"
      "  %s=%s\n",
      options->class ? "--class" : "--device",
      options->class ? options->class : options->device
    );
    if (!options->class) {
      printc("Defaulting to --class=backlight for you.\n");
      options->class = strdup("backlight");
    }
  }
  struct device *device;
  if (options->device) {
    device = calloc(1, sizeof(struct device));
    read_device(device, options->class, options->device);
  } else {
    printfc("Taking first device from --class=%s for you.\n\n",options->class);
    read_class(&device, options->class, false);
  }
  return device;
}

float value_to_percent(float value, struct device *device, struct options *options) {
  if (value <= 0)
    return 0;
  return powf(value / device->max_brightness, 1.0f / options->exponent) * 100;
}

unsigned int percent_to_value(float percent, struct device *device, struct options *options) {
  if (percent <= 0)
    return 0;
  if (percent >= 100)
    return device->max_brightness;
  return roundf(powf(percent / 100, options->exponent) * device->max_brightness);
}

int find_value(struct device *device, struct options *options) {
  // look at `options->set_to`
  // if relative (i. e. +X or X-) look at device->curr_brightness
  // if X=Y% convert to value
  
  // Check if relative (part 1/2)
  bool positive = *(options->set_to) == '+';

  char *end;
  long res = strtol(options->set_to, &end, 10);

  // Check if percent / relative (part 2/2)
  bool percent = *end == '%';
  bool negative = *end == '-';
  if (percent)
    negative |= end[1] == '-';

  // Now find real value
  if (!percent && !negative && !positive) // absolute linear
    return res;
  
  if (!percent) // relative linear
    return device->curr_brightness + (negative ? -res : res);

  if (!negative && !positive) // absolute exp
    return percent_to_value(res, device, options);

  if (res == 0) // bad for calc
    return device->curr_brightness;

  int i = -1;
  float max = find_base(&i, device->curr_brightness, res, device, options);

  i = positive ? i-1 : i+1;

  return percent_to_value(max - i*res, device, options);
}

float find_base(int *i, unsigned int target, int delta, struct device *device, struct options *options) {
  float sup = value_to_percent(target + 0.5, device, options);
  float inf = value_to_percent(target - 0.5, device, options);

  unsigned int previousPrevious = -1;

  bool early_exit = *i != -1;
  if (!early_exit)
    *i = (100 + EPS - inf) / delta;
  for (; *i > 0; (*i)--) {
    unsigned int base = percent_to_value(sup + (*i)*delta, device, options) + 1;

search_base:
      base--;
      float max = value_to_percent(base, device, options);
      unsigned int reconstructed = percent_to_value(max - (*i)*delta, device, options);
    if (target < reconstructed) goto search_base;

    if (target > reconstructed) {
      if (early_exit)
        return max;
      continue;
    }

    int j = (*i)-1;

    unsigned int previous = percent_to_value(max - j*delta, device, options);
    if (previous >= previousPrevious)
      goto search_base;

    previousPrevious = previous;

    max = find_base(&j, previous, delta, device, options);

    reconstructed = percent_to_value(max - (*i)*delta, device, options);

    if (target == reconstructed)
      return max;

    goto search_base;
  }

  return value_to_percent(target, device, options);
}

