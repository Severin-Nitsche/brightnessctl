#include "brightnessctl.h"
#include "brightnesslib.h"

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
    "  -s, --save              save previous state in a temporary file\n";
  const char *set_options = 
    "`set` specific options:\n"
    "  -q, --quiet             suppress output\n"
    "  -n, --min-value=n       set minimum brightness\n"
    "  -e, --exponent[=K]      set exponent of brightnesscurve\n"
    "                          (Default: 1, linear)\n"
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
  printf("version");
}

int get(struct options *options) {
  printf("get");
  return 1;
}

int set(struct options *options) {
  printf("set");
  return 1;
}
