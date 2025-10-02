#ifndef BRIGHTNESSCTL_H
#define BRIGHTNESSCTL_H

#include <stdbool.h>

/** The operations of the cli. 
 * The cli determines which operation to perform based on this value.
 * It is specified as the first parameter (if at all)
 */
enum operation { 
  UNSET, /**< No operation specified */
  GET, /**< `get` operation specified */
  SET /**< `set` operation specified */
};

/** The options supported by the cli.
 * This struct contains all options the cli supports. Upon parsing they will be
 * inserted into a value of this type and during execution they govern the
 * behaviour of the program.
 */
struct options {
  bool quiet;
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

/** Print a helpful message to the user. 
 *
 * @param options The specified options (in particular the specified
 * `operation`) specify which help page is printed (General, SET or GET)
 */
extern void print_help(struct options *);
/** Prints the program version */
extern void print_version();
/** Retrieves the device information.
 * Prints information on all devices iff `list` is true. If in addition to
 * `list`, a `class` is set, it prints only the information for this class.
 * Otherwise it only retrieves information for the device specified through
 * `device` and `class`. If they are underspecified it prints a complaint in
 * addition unless `quiet` is set too. If only printing information about a
 * specific device and if `save` is set, this saves the current brightness
 * somewhere temporary.
 *
 * @param options The cli options that govern the behaviour
 *
 * @returns 0 if everything went ok and something else if an error occurred.
 */
extern int get(struct options *);
/** Sets the device information.
 * Based on compilation flags, this uses either the dbus or the filesystem to
 * set the brightness of the device specified through `class` and `device`. If
 * they are underspecified it prints a complaint unless silenced via `quiet`. If
 * **not** silenced via `quiet` it also prints the brightness information of the
 * device after applying the change dictated by `set_to` or `restore`. If
 * `restore` is set, a previous configuration saved with `save` is restored. If
 * `save` is set, it saves the brightness before setting a value but restores
 * (if set) the brightness saved previously.
 *
 * @param options The cli options which determine the behaviour
 *
 * @returns 0 if everything went ok and something else if an error occurred.
 */
extern int set(struct options *);

#endif
