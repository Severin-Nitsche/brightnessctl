#include "brightnessutil.h"
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
