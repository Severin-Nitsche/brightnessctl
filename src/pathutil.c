#include "pathutil.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <errno.h>
#include <sys/stat.h>

char *path = "/sys/class";

char *run_dir = "/tmp/brightnessctl";

bool ensure_dir(char *dir) {
  struct stat sb;
  if (stat(dir, &sb)) {
    if (errno != ENOENT)
      return false;
    errno = 0;
    if (mkdir(dir, 0777)) {
      return false;
    }
    if (stat(dir, &sb))
      return false;
  }
  if (!S_ISDIR(sb.st_mode)) {
    errno = ENOTDIR;
    return false;
  }
  return true;
}

bool ensure_run_dir() {
  static bool set;
  if (!set) {
    char *sys_run_dir = getenv("XDG_RUNTIME_DIR");
    if (sys_run_dir)
      run_dir = dir_child(sys_run_dir, "brightnessctl");
    set = true;
  }
  return ensure_dir(run_dir);
}

bool ensure_dev_dir(struct device *dev) {
  char *cpath;
  bool ret;
  if (!ensure_run_dir())
    return false;
  cpath = dir_child(run_dir, dev->class);
  ret = ensure_dir(cpath);
  free(cpath);
  return ret;
}

char *_cat_with(char c, ...) {
  size_t size = 32;
  size_t length = 0;
  char *buf = calloc(1, size + 1);
  char *curr;
  char split[2] = {c, '\0'};
  va_list va;
  va_start(va, c);
  curr = va_arg(va, char *);
  while (curr) {
    length += strlen(curr);
    while (length + 2 > size)
      buf = realloc(buf, size *= 2);
    strcat(buf, curr);
    if ((curr = va_arg(va, char*))) {
      length++;
      strcat(buf, split);
    }
  }
  return buf;
}

char *dir_child(char *parent, char *child) {
  return cat_with('/', parent, child);
}

char *device_path(struct device *dev) {
  return cat_with('/', path, dev->class, dev->id);
}

char *class_path(char *class) {
  return dir_child(path, class);
}
