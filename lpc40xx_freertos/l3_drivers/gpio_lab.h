#include <stdint.h>
#include <stdbool.h>

#include "lpc40xx.h"

/// Should alter the hardware registers to set the pin as input
void gpiol__set_as_input(uint8_t port_num, uint8_t pin_num);

/// Should alter the hardware registers to set the pin as output
void gpiol__set_as_output(uint8_t port_num, uint8_t pin_num);

/// Should alter the hardware registers to set the pin as high
void gpiol__set_high(uint8_t port_num, uint8_t pin_num);

/// Should alter the hardware registers to set the pin as low
void gpiol__set_low(uint8_t port_num, uint8_t pin_num);
    
/**
 * Should alter the hardware registers to set the pin as low
 *
 * @param {bool} high - true => set pin high, false => set pin low
 */
void gpiol__set(uint8_t port_num, uint8_t pin_num, bool high);
    
/**
 * Should return the state of the pin (input or output, doesn't matter)
 *
 * @return {bool} level of pin high => true, low => false
 */
bool gpiol__get_level(uint8_t port_num, uint8_t pin_num);