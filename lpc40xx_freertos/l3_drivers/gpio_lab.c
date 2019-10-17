#include "gpio_lab.h"

static const LPC_GPIO_TypeDef *gpio_regs_pointers[] = {LPC_GPIO0, LPC_GPIO1, LPC_GPIO2, LPC_GPIO3, LPC_GPIO4, LPC_GPIO5};
static LPC_GPIO_TypeDef *get_lpc_gpio_reg(uint8_t port_num) {
    return (LPC_GPIO_TypeDef *)gpio_regs_pointers[port_num];
}

/// Should alter the hardware registers to set the pin as input
void gpiol__set_as_input(uint8_t port_num, uint8_t pin_num)
{
    get_lpc_gpio_reg(port_num)->DIR &= ~(1<<pin_num);
}

/// Should alter the hardware registers to set the pin as output
void gpiol__set_as_output(uint8_t port_num, uint8_t pin_num)
{
    get_lpc_gpio_reg(port_num)->DIR |= (1<<pin_num);
}

/// Should alter the hardware registers to set the pin as high
void gpiol__set_high(uint8_t port_num, uint8_t pin_num)
{
    get_lpc_gpio_reg(port_num)->SET = (1<<pin_num);
}

/// Should alter the hardware registers to set the pin as low
void gpiol__set_low(uint8_t port_num, uint8_t pin_num)
{
    get_lpc_gpio_reg(port_num)->CLR = (1<<pin_num);
}
    
/**
 * Should alter the hardware registers to set the pin as low
 *
 * @param {bool} high - true => set pin high, false => set pin low
 */
void gpiol__set(uint8_t port_num, uint8_t pin_num, bool high)
{
    if (high) {
        get_lpc_gpio_reg(port_num)->SET = (1<<pin_num);
    } else {
        get_lpc_gpio_reg(port_num)->CLR = (1<<pin_num);
    }
}

/**
 * Should return the state of the pin (input or output, doesn't matter)
 *
 * @return {bool} level of pin high => true, low => false
 */
bool gpiol__get_level(uint8_t port_num, uint8_t pin_num)
{
    if(get_lpc_gpio_reg(port_num)->PIN & (1<<pin_num)) {
        return true;
    } else {
        return false;
    }
}