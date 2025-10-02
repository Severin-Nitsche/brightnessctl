#ifndef BRIGHTNESS_LIB_H
#define BRIGHTNESS_LIB_H

#include <stdbool.h>
#include <stdio.h>

/** This struct holds device information.
 * The `class` and `id` uniquely specify the device.
 * The `max_brightness` is determined and should be read only for the consumer.
 * The `curr_brightness` serves as an interface to read and write the current brightness through means of the `read_*` and  `do_write_device` or `logind_set_brightness` methods.
 */
struct device {
  //! The device class (backlight, leds)
  char *class;
  //! The device id
  char *id;
  //! The current brightness of the device
  unsigned int curr_brightness;
  //! The maximum brightness of the device
  unsigned int max_brightness;
};

#ifdef ENABLE_LOGIND
/** Set the brightness of a device via dbus.
 * Attempts to set the brightness of the specified device to its
 * `curr_brightness`.
 *
 * @param d The device to set the brightness of
 *
 * @returns Whether the request was successful
 */
extern bool logind_set_brightness(struct device *);
#endif

/** Set the brightness of a device via the file system.
 * Attempts to set the brightness of the device by writing to its corresponding
 * file. This operation needs root to work.
 *
 * @param d The device to set the brightness of
 *
 * @returns Whether the write was successful
 */
extern bool do_write_device(struct device *);

/** Read all devices into a buffer.
 * This function identifies all devices from the default classes (backlight and
 * leds) and stores them into the buffer provided by `devs`.
 *
 * @param devs The buffer to store the devices in. Make sure that it is large
 * enough.
 *
 * @returns The number of devices found
 */
extern int read_devices(struct device **);
/** Read all/one device(s) from the specified class.
 * 
 * @param devs The buffer to store the devices in. Make sure that it is large
 * enough.
 * @param class The class to read the devices of.
 * @param read_all Whether to read all devices (`true`) or just one (`false`)
 *
 * @returns The number of devices read
 */
extern int read_class(struct device **, char *, bool);
/** Read a single device into the struct.
 * This function gets the information of the device specified by `class` and
 * `id` and initializes `d` with it.
 *
 * @param d The memory location to hold the device information
 * @param class The class of the device
 * @param id The id of the device
 *
 * @returns Whether the read was successful
 */
extern bool read_device(struct device *, char *, char *);

#endif
