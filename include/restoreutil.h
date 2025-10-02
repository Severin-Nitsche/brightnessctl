#ifndef RESTORE_UTIL_H
#define RESTORE_UTIL_H

#include "brightnesslib.h"

/** Save the device configuration somewhere temporary.
 * This function serializes the current brightness of the device and saves it in
 * `run_dir` under a location for the `device`.
 *
 * @param dev The device to save
 *
 * @returns Wheter or not the save was successful
 */
extern bool save_device_data(struct device *);
/** Reads the device configuration from somewhere temporary.
 * This function restores a previously saved brightness for `dev` in the
 * `curr_brightness` value of the struct.
 *
 * @param dev The device to restore.
 *
 * @returns Whether of not the restore was successful
 */
extern bool restore_device_data(struct device *);

#endif
