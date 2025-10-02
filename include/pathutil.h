#ifndef PATH_UTIL_H
#define PATH_UTIL_H

#include "brightnesslib.h"

extern char *_cat_with(char, ...);
#define cat_with(...) _cat_with(__VA_ARGS__, NULL)

extern char *dir_child(char *, char *);
extern char *device_path(struct device *);
extern char *class_path(char *);

extern bool ensure_dir(char *dir);
extern bool ensure_run_dir();
extern bool ensure_dev_dir(struct device *);

extern char *run_dir;

#endif
