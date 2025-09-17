#include "brightnesslib.h"
#include "pathutil.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>

#ifdef ENABLE_LOGIND
# if defined(HAVE_LIBSYSTEMD)
#  include <systemd/sd-bus.h>
# elif defined(HAVE_LIBELOGIND)
#  include <elogind/sd-bus.h>
# elif defined(HAVE_BASU)
#  include <basu/sd-bus.h>
# else
#  error "No dbus provider found"
# endif
#endif

bool do_write_device(struct device *d) {
  FILE *f;
  char c[16];
  size_t s = sprintf(c, "%u", d->curr_brightness);
  errno = 0;
  if (s <= 0) {
    errno = EINVAL;
    goto fail;
  }
  if ((f = fopen(dir_child(device_path(d), "brightness"), "w"))) {
    if (fwrite(c, 1, s, f) < s)
      goto close;
  } else
    goto fail;
  errno = 0;
close:
  fclose(f);
fail:
  if (errno)
    perror("Error writing device");
  return !errno;
}

#ifdef ENABLE_LOGIND

bool logind_set_brightness(struct device *d) {
  sd_bus *bus = NULL;
  int r = sd_bus_default_system(&bus);
  if (r < 0) {
    fprintf(stderr, "Can't connect to system bus: %s\n", strerror(-r));
    return false;
  }
  
  r = sd_bus_call_method(
    bus,
    "org.freedesktop.login1",
    "/org/freedesktop/login1/session/auto",
    "org.freedesktop.login1.Session",
    "SetBrightness",
    NULL,
    NULL,
    "ssu",
    d->class,
    d->id,
    d->curr_brightness);
  if (r < 0)
    fprintf(stderr, "Failed to set brightness: %s\n", strerror(-r));
  
  sd_bus_unref(bus);

  return r >= 0;
}

#endif

int read_devices(struct device **devs) {
  size_t n = 0;
  char *class;
  int cnt = 0;
  while ((class = classes[n++]))
    cnt += read_class(devs + cnt, class, true);
  return cnt;
}


int read_class(struct device **devs, char *class, bool read_all) {
  DIR *dirp;
  struct dirent *ent;
  struct device *dev;
  char *c_path;
  int cnt = 0;
  dirp = opendir(c_path = class_path(class));
  if (!dirp)
    return 0;
  while ((ent = readdir(dirp))) {
    if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
      continue;
    dev = calloc(1, sizeof(struct device));
    if (!read_device(dev, class, ent->d_name)) {
      free(dev);
      continue;
    }
    devs[cnt++] = dev;
    if (!read_all)
      break;
  }
  closedir(dirp);
  free(c_path);
  return cnt;
}


bool read_device(struct device *d, char *class, char *id) {
  DIR *dirp;
  FILE *f;
  char *dev_path = NULL;
  char *ent_path = NULL;
  int error = 0;
  struct dirent *ent;
  bool cur;
  d->class = strdup(class);
  d->id = strdup(id);
  dev_path = device_path(d);
  if (!(dirp = opendir(dev_path)))
    goto dfail;
  while ((ent = readdir(dirp))) {
    if ((cur = !strcmp(ent->d_name, "brightness")) ||
      !strcmp(ent->d_name, "max_brightness")) {
      if (!(f = fopen(ent_path = dir_child(dev_path, ent->d_name), "r")))
        goto fail;
      clearerr(f);
      if (fscanf(f, "%u", cur ? &d->curr_brightness : &d->max_brightness) == EOF) {
        fprintf(stderr, "End-of-file reading %s of device '%s'.",
          cur ? "brightness" : "max brightness", d->id);
        error++;
      } else if (ferror(f)) {
        fprintf(stderr, "Error reading %s of device '%s': %s.",
          cur ? "brightness" : "max brightness", d->id, strerror(errno));
        error++;
      }
      fclose(f);
      free(ent_path);
      ent_path = NULL;
    }
  }
  errno = 0;
fail:
  closedir(dirp);
dfail:
  free(dev_path);
  free(ent_path);
  if (errno) {
    perror("Error reading device");
    error++;
  }
  return !error;
}
