#ifndef PATH_UTIL_H
#define PATH_UTIL_H

#include "brightnesslib.h"

/** Concatenate VARARGS with `c`. 
 * Use the macro `cat_with` instead or terminate your varargs with NULL.
 */
extern char *_cat_with(char, ...);
#define cat_with(...) _cat_with(__VA_ARGS__, NULL)

/** Alias for `cat_with('/', parent, child) */
extern char *dir_child(char *, char *);
/** Get the path of the `device`. */
extern char *device_path(struct device *);
/** Get the path of the `class`. */
extern char *class_path(char *);

/** Like `mkdir -p`
 *
 * @param dir The directory create/check for existence
 *
 * @returns `false` if something goes wrong, `true` otherwise.
 */
extern bool ensure_dir(char *);
/** Makes sure the `run_dir` exists
 * Also sets the `run_dir` according to environment variable `XDG_RUNTIME_DIR`.
 *
 * @returns `false` if something goes wrong, `true` otherwise.
 */
extern bool ensure_run_dir();
/** Makes sure the device directory exists.
 *
 * @param dev The device to ensure
 */
extern bool ensure_dev_dir(struct device *);

extern char *run_dir;

#endif
