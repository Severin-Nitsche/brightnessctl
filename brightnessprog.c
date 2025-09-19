#include "brightnessctl.h"
#include "brightnesslib.h"
#include "brightnessutil.h"

#include <string.h>

void print_help(struct options *options) {
  const char *general = "Usage: brightnessctl [operation] [options...]\n";
  const char *get = "Usage: brightnessctl get [options...]\n";
  const char *set = "Usage: brightnessctl set [options...] [value]\n";
  const char *general_options =
    "Options:\n"
    "  -p, --pretend           do not perform write operations\n"
    "  -m, --machine-readable  produce machine readable output\n"
    "  -d, --device=DEVICE     specify device name (can be a wildcard)\n"
    "  -c, --class=CLASS       specify device class\n"
    "  -h, --help              print this help message\n"
    "  -v, --version           print version and exit\n"
    "  -e, --exponent[=K]      set exponent of brightnesscurve\n"
    "                          (Default: 1, linear; 4 if provided without K)\n"
    "  -s, --save              save previous state in a temporary file\n";
  const char *set_options = 
    "`set` specific options:\n"
    "  -q, --quiet             suppress output\n"
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
      print_device(devs[i], options);
    }
  } else if (options->device || options->class) {
    if (!options->class || !options->device) {
      error = 1;
      printfc(
        "A device complete specification of a device requires both of:\n"
        " --class and --device.\n"
        "You specified:\n"
        "  %s=%s\n",
        options->class ? "--class" : "--device",
        options->class ? options->class : options->device
      );
      if (!options->class) {
        printc("Defaulting to --class=backlight for you.");
        options->class = strdup("backlight");
      }
    }
    struct device *device;
    if (options->device) {
      read_device(device, options->class, options->device);
    } else {
      printfc("Taking first device from --class=%s for you.\n\n",options->class);
      read_class(&device, options->class, false);
    }
    print_device(device, options);
  } else {
    printf("Congratulations. You encountered an error.");
    error = 1;
  }
  return error;
}

int set(struct options *options) {
  printf("set");
  return 1;
}
