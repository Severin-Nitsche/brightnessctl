#include "brightnessutil.h"
#include "restoreutil.h"

#include <string.h>
#include <stdlib.h>
#include <math.h>

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
    value_in_percent(device->curr_brightness, device, options),
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

float value_in_percent(int value, struct device *device, struct options *options) {
  if (value <= 0)
    return 0;
  return powf(((float) value) / device->max_brightness, 1.0f / options->exponent) * 100;
}

int percent_in_value(float percent, struct device *device, struct options *options) {
  if (percent <= 0)
    return 0;
  return roundf(powf(percent / 100, options->exponent) * device->max_brightness);
}

int find_value(struct device *device, struct options *options) {
  // TODO look at `options->set_to`
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
  int value = res;


  if (negative) {
    res *= -1;
  }

  if (positive || negative) {
    value = device->curr_brightness;
  }

  if (percent) {
    float percentage = value_in_percent(value, device, options);
    if (positive || negative) {
      percentage += res;
    }
    value = percent_in_value(percentage, device, options);
  } else if (positive || negative) {
    value += res;
  }

  return value;
}
