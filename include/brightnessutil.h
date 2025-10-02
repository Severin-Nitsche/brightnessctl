#ifndef BRIGHTNESS_UTIL_H
#define BRIGHTNESS_UTIL_H

#include "brightnesslib.h"
#include "brightnessctl.h"
#define printfc(F,...) if (!options->quiet && !options->machine_readable) printf(F,__VA_ARGS__)
#define printc(S) if (!options->quiet && !options->machine_readable) printf(S)

/** Print device information.
 * This function prints the information of `device` and respects the
 * `machine_readable` flag. If it is set, the output will be csv-like otherwise
 * the output will be more user friendly.
 *
 * @param device The device to print
 * @param options The options that govern the printing behaviour
 */
extern void print_device(struct device *, struct options *);
/** Prints device information and optionally saves the data.
 * This function behaves like `print_device`. Additionally it saves the device
 * data in a temporary file if `save` is set.
 *
 * @param device The device to print and possibly save
 * @param options The options that govern printing and saving behaviour
 */
extern void print_and_save_device(struct device *, struct options *);
/** Initializes a struct for the specified device.
 * This function initializes a fresh `struct device *` for the device specified
 * through `class` and `device`. If it is not silenced with `quiet` it prints a
 * complaint if the device is underspecified and the function needs to fill in
 * with defaults. In any case, if that happens an error is registered.
 *
 * @param options The options that govern the device finding
 * @param error An integer that is set to 1 if the device is underspecified.
 *
 * @returns A pointer to a device struct containing information about the
 * queried device.
 */
extern struct device *find_device(struct options *, int *);
/** Converts the absolute brightness value to a percentage.
 * This function converts the brightness `value` of `device` to a percentage
 * while respecting the `exponent` for the brightness curve, where: X%
 * correspond to a brightness of X%^`exponent` * max_brightness.
 *
 * @param value The absolute brightness
 * @param device The device for reference (max_brightness)
 * @param options The options which specify the `exponent`
 *
 * @returns a value between 0 and 100
 */
extern float value_to_percent(float, struct device *, struct options *);
/** Converts the relative brightness to an absolute value.
 * This function is the reverse of `value_to_percent`. But it is to note, that
 * (especially for `exponent` > 1) there does not always exist an absolute value
 * corresponding to the desired percentage. In this case, the value is rounded
 * to the nearest possible value, but relative this can lead to a difference of
 * 5% or more especially on the low end.
 *
 * @param percent The relative brightness
 * @param device The device for reference
 * @param options The options which specify the `exponent`
 *
 * @returns an integer representing the absolute brightness
 */
extern unsigned int percent_to_value(float, struct device *, struct options *);
/** Finds the value to set the brightness to.
 * The cli allows for multiple modes of brightness specification: Absolute
 * values, percentage values, absolute differences and percentage differences.
 * This function parses the `set_to` value and uses the `device` and `options`
 * parameters to determine the desired value of the user.
 *
 * @param device The device for reference
 * @param options The options which specify the `exponent`
 *
 * @returns The absolute brightness the user wants to set
 */
extern int find_value(struct device *, struct options *);
/** This function tries to cope with the inaccuracies of the exponential scaling
 * when using percentages. As discussed for `value_to_percent`, the exponential
 * scaling can lead to quite big inaccuracies when using relative percentage
 * steps. These inaccuracies might accumulate over time and further result in a
 * value drift, meaning a naïve and for that matter ANY implementation will
 * experience cases where in theory perfectly reversible steps (like 10%- +10%)
 * will result in different values. To mitigate this effect, an assumption on
 * the usage of such steps is made: *If a user requests a ±X% step, the current
 * value was also reached by a ±X% step.*
 * This assumption is grounded in the use of brightness-up / brightness-down
 * keys to which utilize this utility.
 * To combat accumulated inaccuracies, this function calculates the maximum
 * possible value that could have reached the `target` with `delta`% decreases.
 * The idea being that instead of going `delta`% up or down as accurately as
 * possible from the current brightness, this base can be used to step `i±1 x
 * target` steps down as accurately as possible from the "original" starting
 * point.
 * Note that even with this quite involved implementation, a value drift is
 * still possible even in the use case described, but there will be less of
 * them.
 *
 * @param i The number of steps to find the base of (set to -1 for automatic
 * determination)
 * @param target The target value to reach from the base
 * @param delta The difference in percent to step each time
 * @param device The device for reference
 * @param options The options which specify the `exponent`
 *
 * @returns A value between 0 and 100 representing the maximal percentage from
 * which the target could have been reached with `i` `delta` steps.
 */
extern float find_base(int *, unsigned int, int delta, struct device *, struct options *);

#endif
