#include "pathutil.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

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
