// @file gpio_isr.c
#include "gpio_isr.h"
#include "sys_time.h"

static function_pointer_t gpio0_callbacks[32];
static function_pointer_t gpio2_callbacks[32];
static volatile uint64_t lastDebounceTime = 0;
const static uint64_t debounceDelay_ms = 500;

void gpio0__attach_interrupt(uint32_t pin, gpio_interrupt_e interrupt_type, function_pointer_t callback) {
  // 1) Store the callback based on the pin at gpio0_callbacks
  // 2) Configure GPIO 0 pin for rising or falling edge

  gpio0_callbacks[pin] = callback;

  if (interrupt_type == GPIO_INTR__FALLING_EDGE) {
    LPC_GPIOINT->IO0IntEnF |= (1 << pin);
  } else if (interrupt_type == GPIO_INTR__RISING_EDGE) {
    LPC_GPIOINT->IO0IntEnR |= (1 << pin);
  }
}

void gpio2__attach_interrupt(uint32_t pin, gpio_interrupt_e interrupt_type, function_pointer_t callback) {
  // 1) Store the callback based on the pin at gpio0_callbacks
  // 2) Configure GPIO 0 pin for rising or falling edge

  gpio2_callbacks[pin] = callback;

  if (interrupt_type == GPIO_INTR__FALLING_EDGE) {
    LPC_GPIOINT->IO2IntEnF |= (1 << pin);
  } else if (interrupt_type == GPIO_INTR__RISING_EDGE) {
    LPC_GPIOINT->IO2IntEnR |= (1 << pin);
  }
}

static void gpio0__clear_pin_interrupt(int pin) { LPC_GPIOINT->IO0IntClr |= (1 << pin); }
static void gpio2__clear_pin_interrupt(int pin) { LPC_GPIOINT->IO2IntClr |= (1 << pin); }

// We wrote some of the implementation for you
void gpio0__interrupt_dispatcher(void) {
  // Check which pin generated the interrupt
  int pin_that_generated_interrupt, i;
  for (i = 0; i < 32; i++) {
    if (LPC_GPIOINT->IO0IntStatF & (1 << i) || LPC_GPIOINT->IO0IntStatR & (1 << i)) {
      break;
    }
  }
  pin_that_generated_interrupt = i;
  function_pointer_t attached_user_handler = gpio0_callbacks[pin_that_generated_interrupt];

  // Check Debounce logic
  // if current time minus the last trigger time is greater than
  // the delay (debounce) time, button is completely closed.
  if ((sys_time__get_uptime_ms() - lastDebounceTime) > debounceDelay_ms) {
    lastDebounceTime = sys_time__get_uptime_ms();
    // Invoke the user registered callback
    attached_user_handler();
  }
  // Clear the interrupt
  gpio0__clear_pin_interrupt(pin_that_generated_interrupt);
}

void gpio2__interrupt_dispatcher(void) {
  // Check which pin generated the interrupt
  int pin_that_generated_interrupt, i;
  for (i = 0; i < 32; i++) {
    if (LPC_GPIOINT->IO2IntStatF & (1 << i) || LPC_GPIOINT->IO2IntStatR & (1 << i)) {
      break;
    }
  }
  pin_that_generated_interrupt = i;
  function_pointer_t attached_user_handler = gpio0_callbacks[pin_that_generated_interrupt];

  // Check Debounce logic
  if ((sys_time__get_uptime_ms() - lastDebounceTime) > debounceDelay_ms) {
    lastDebounceTime = sys_time__get_uptime_ms();
    // Invoke the user registered callback
    attached_user_handler();
  }
  // Clear the interrupt
  gpio2__clear_pin_interrupt(pin_that_generated_interrupt);
}

void gpio__interrupt_dispatcher(void) {
  // Check which port (port 0 or port 2) generated the interrupt
  // and call respective Interrupt Dispatcher
  if (LPC_GPIOINT->IntStatus & (1 << 0)) {
    gpio0__interrupt_dispatcher();
  } else if (LPC_GPIOINT->IntStatus & (1 << 2)) {
    gpio2__interrupt_dispatcher();
  }
}