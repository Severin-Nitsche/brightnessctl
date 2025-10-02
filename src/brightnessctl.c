#include "brightnessctl.h"

#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static struct options options = {
  .quiet = false,
  .machine_readable = false,
  .help = false,
  .version = false,
  .save = false,
  .restore = false,
  .list = false,
  .min_value = 0,
  .exponent = 1,
  .set_to = NULL,
  .operation = UNSET,
  .device = NULL,
  .class = NULL
};

int main(int argc, char *const *argv) {
  int opt;

  const char *optstring = "+qmd:c:hvsn:e::rl";

  const struct option long_options[] = {
    {"quiet", no_argument, NULL, 'q'},
    {"machine-readable", no_argument, NULL, 'm'},
    {"device", required_argument, NULL, 'd'},
    {"class", required_argument, NULL, 'c'},
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    {"save", no_argument, NULL, 's'},
    // `set` specific arguments
    {"min-value", required_argument, NULL, 'n'},
    {"exponent", optional_argument, NULL, 'e'},
    {"restore", no_argument, NULL, 'r'},
    {"list", no_argument, NULL, 'l'},
    {NULL, 0, NULL, 0}
  };

  // Options start after the verb/operation
  char *const *opts = argv+1;
  int optc = argc-1;

  // Before argument processing, we check, whether a verb/operation was provided
  // otherwise, we send them to help
  if (argc >= 2) {
    if (!strcmp(argv[1], "get")) { // strcmp returns *0* if strings are equal
      options.operation = GET;
    } else if (!strcmp(argv[1], "set")) {
      options.operation = SET;
    } else {
      // The verb/operation was omitted
      // This can either be a user error, or --help or --version are invoked
      opts = argv;
      optc = argc;
    }
  }

  int error = 0;

  while ((opt = getopt_long(optc, opts, optstring, long_options, NULL)) != -1) {
    switch (opt) {
      case 'q':
        options.quiet = true;
        break;
      case 'm':
        options.machine_readable = true;
        break;
      case 'd':
        options.device = optarg;
        break;
      case 'c':
        options.class = optarg;
        break;
      case 'h':
        options.help = true;
        break;
      case 'v':
        options.version = true;
        break;
      case 's':
        options.save = true;
        break;
      case 'n':
        // Unchecked conversion from long to int
        // (User may not provide a value that is too high;
        // if they do, their minimum is not respected)
        options.min_value = strtol(optarg, NULL, 10);
        break;
      case 'e':
        // Unchecked conversion from long to int
        // (Very high exponents lead to unexpected behavior)
        if (optarg)
          options.exponent = strtol(optarg, NULL, 10);
        else
          options.exponent = 3;
        break;
      case 'r':
        options.restore = true;
        break;
      case 'l':
        options.list = true;
        break;
      default:
        // Something is really off
        error = 1;
        // Fallthrough
      case '?':
        // Problem with parsing `optopt` could provide insights
        // `getopt` prints error message on its own (probably?)
        options.help = true;
    }
  }

  // If applicable (and present) we get the value for the set command
  if (options.operation == SET) {
    if (optind < optc)
      options.set_to = opts[optind];
    else
      options.help |= !options.restore;
  } 

  // Now we determine what to do
  if (error) {
    // The parser behaved in an unexpected manner
    // Did we have a typo in an option or
    // did we forget to handle one?
    fprintf(
      stderr,
      "An unhandled error occured during argument parsing.\n"
      "This should never happen."
    );
  }
  if (options.help || error || (options.operation == UNSET && !options.version)) {
    print_help(&options);
    return error;
  }
  if (options.version) {
    print_version();
    return 0;
  }
  if (options.operation == GET) {
    return get(&options);
  }
  if (options.operation == SET) {
    return set(&options);
  }
  // If we reach this point something went wrong
  printf("Something unexpected happened.");
  return 1;
}
