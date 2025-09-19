#include "restoreutil.h"
#include "pathutil.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static char *run_dir = "/tmp/brightnessctl";

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

static bool ensure_run_dir() {
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

bool save_device_data(struct device *dev) {
  char c[16];
  size_t s = sprintf(c, "%u", dev->curr_brightness);
  char *d_path = NULL;
  FILE *fp;
  mode_t old = 0;
  int error = 0;
  errno = 0;
  if (s <= 0) {
    fprintf(stderr, "Error converting device data.");
    error++;
    goto fail;
  }
  if (!ensure_dev_dir(dev))
    goto fail;
  d_path = cat_with('/', run_dir, dev->class, dev->id);
  old = umask(0);
  fp = fopen(d_path, "w");
  umask(old);
  if (!fp)
    goto fail;
  if (fwrite(c, 1, s, fp) < s) {
    fprintf(stderr, "Error writing to '%s'.\n", d_path);
    error++;
  }
  fclose(fp);
fail:
  free(d_path);
  if (errno) {
    perror("Error saving device data");
    error++;
  }
  return !error;
}

bool restore_device_data(struct device *dev) {
  char buf[16];
  char *filename = NULL;
  char *end;
  FILE *fp;
  memset(buf, 0, 16);
  errno = 0;
  if (!ensure_dev_dir(dev))
    goto fail;
  filename = cat_with('/', run_dir, dev->class, dev->id);
  if (!(fp = fopen(filename, "r")))
    goto fail;
  if (!fread(buf, 1, 15, fp))
    goto rfail;
  dev->curr_brightness = strtol(buf, &end, 10);
  if (end == buf)
    errno = EINVAL;
rfail:
  fclose(fp);
fail:
  free(filename);
  if (errno) {
    perror("Error restoring device data");
    return false;
  }
  return true;
}


