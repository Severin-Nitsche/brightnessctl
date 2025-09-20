#include "brightnessctl.h"
#include "brightnesslib.h"
#include "brightnessutil.h"
#include "restoreutil.h"

#include <string.h>
#include <stdlib.h>

void print_help(struct options *options) {
  const char *general = "Usage: brightnessctl [operation] [options...]\n";
  const char *get = "Usage: brightnessctl get [options...]\n";
  const char *set = "Usage: brightnessctl set [options...] [value]\n";
  const char *general_options =
    "Options:\n"
    "  -p, --pretend           do not perform write operations\n"
    "  -m, --machine-readable  produce machine readable output\n"
    "  -d, --device=DEVICE     specify device name\n"
    "  -c, --class=CLASS       specify device class\n"
    "  -h, --help              print this help message\n"
    "  -v, --version           print version and exit\n"
    "  -e, --exponent[=K]      set exponent of brightnesscurve\n"
    "                          (Default: 1, linear; 4 if provided without K)\n"
    "  -q, --quiet             suppress output\n"
    "  -s, --save              save previous state in a temporary file\n";
  const char *set_options = 
    "`set` specific options:\n"
    "  -n, --min-value=n       set minimum brightness\n"
    "  -r, --restore           restore previously saved state\n";
  const char *get_options = 
    "`get` specific options:\n"
    "   -l, --list             list info for all devices\n";
  const char *operations = 
    "Operations:\n"
    "  get  get device info\n"
    "  set  set device brightness\n";

  if (options->operation == UNSET) {
    printf("%s\n%s\n%s",general,general_options,operations);
  } else if (options->operation == GET) {
    printf("%s\n%s\n%s",get,general_options,get_options);
  } else if (options->operation == SET) {
    printf("%s\n%s\n%s",set,general_options,set_options);
  } else { // This should never happen
    printf("Something unexpected happened while printing help");
  }
}

void print_version() {
  printf(VERSION);
}

int get(struct options *options) {
  int error = 0;
  if (options->list) {
    struct device *devs[255];
    int device_count = -1;
    if (options->class) {
      device_count = read_class(devs, options->class, true);
    } else {
      device_count = read_devices(devs);
}
    printfc("%d available devices:\n\n",device_count);
    for (int i = 0; i < device_count; i++) {
      print_and_save_device(devs[i], options);
    }
  } else {
    struct device *device = find_device(options, &error);
    print_and_save_device(device, options);
    free(device);
  }
  return error;
}

int set(struct options *options) {
  int error = 0;
  struct device *device = find_device(options, &error);
  if (options->restore) {
    if (options->save) {
      struct device save = *device;
      if (!restore_device_data(device))
        error = 1;
      if (!save_device_data(&save))
        error = 1;
    } else {
      if (!restore_device_data(device))
        error = 1;
    }
  } else if (options->set_to) {
    if (options->save)
      if (!save_device_data(device))
        error = 1;
    device->curr_brightness = find_value(device, options);
  } else {
    printf("Whoopsie Daisy! You encountered my secret little mystery bug.");
    error = 1;
  }


  if (device->curr_brightness < options->min_value) {
    device->curr_brightness = options->min_value;
  }
  if (device->curr_brightness > device->max_brightness) {
    device->curr_brightness = device->max_brightness;
  }
  if (!options->quiet) {
    print_device(device, options);
  }
  #ifdef ENABLE_LOGIND
    logind_set_brightness(device);
  #else
    do_write_device(device);
  #endif
  free(device);
  return error;
}
