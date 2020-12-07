/*
 * Copyright (c) 2020 Robert Wilbrandt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "status.h"

#include <drivers/gpio.h>
#include <zephyr.h>
#include <errno.h>

atomic_t status = ATOMIC_INIT(STATUS_STARTING);

// Add a utility macro to decide if hardware leds are in use
#undef STATUS_USE_HW
#if defined(CONFIG_GPIO) && (CONFIG_GPIO == 1)
#define STATUS_USE_HW
#endif  // defined(CONFIG_GPIO) && (CONFIG_GPIO==1)

#ifdef STATUS_USE_HW
int status_init_hw();
int status_start_hw();
#endif  // ifdef STATUS_USE_HW

int status_init()
{
#ifdef STATUS_USE_HW
  return status_init_hw();
#else
  return 0;
#endif  // ifdef STATUS_USE_HW
}

int status_start()
{
#ifdef STATUS_USE_HW
  return status_start_hw();
#else
  return 0;
#endif  // ifdef STATUS_USE_HW
}

// Everything from here on is only relevant if we actually use the HW leds
#ifdef STATUS_USE_HW

#define STATUS_LED_THREAD_STACK_SIZE 512
#define STATUS_LED_THREAD_PRIORITY 3

#define STATUS_LED_NODE DT_ALIAS(status_led)
#define STATUS_LED_LABEL DT_GPIO_LABEL(STATUS_LED_NODE, gpios)
#define STATUS_LED_PIN DT_GPIO_PIN(STATUS_LED_NODE, gpios)
#define STATUS_LED_FLAGS DT_GPIO_FLAGS(STATUS_LED_NODE, gpios)

#define ERROR_LED_NODE DT_ALIAS(error_led)
#define ERROR_LED_LABEL DT_GPIO_LABEL(ERROR_LED_NODE, gpios)
#define ERROR_LED_PIN DT_GPIO_PIN(ERROR_LED_NODE, gpios)
#define ERROR_LED_FLAGS DT_GPIO_FLAGS(ERROR_LED_NODE, gpios)

// HW devices used
const struct device* status_led_dev;
const struct device* error_led_dev;

// Thread structures
void status_led_thread(void* param1, void* param2, void* param3);
K_THREAD_STACK_DEFINE(status_led_thread_stack_area,
                      STATUS_LED_THREAD_STACK_SIZE);
struct k_thread status_led_thread_stack_data;

int status_init_hw()
{
  status_led_dev = device_get_binding(STATUS_LED_LABEL);
  if (gpio_pin_configure(status_led_dev, STATUS_LED_PIN,
                         GPIO_OUTPUT_ACTIVE | STATUS_LED_FLAGS) != 0)
  {
    return -EIO;
  }

  error_led_dev = device_get_binding(ERROR_LED_LABEL);
  if (gpio_pin_configure(error_led_dev, ERROR_LED_PIN,
                         GPIO_OUTPUT_ACTIVE | ERROR_LED_FLAGS) != 0)
  {
    return -EIO;
  }
  gpio_pin_set(error_led_dev, ERROR_LED_PIN, (int)false);

  return 0;
}

int status_start_hw()
{
  k_thread_create(&status_led_thread_stack_data, status_led_thread_stack_area,
                  K_THREAD_STACK_SIZEOF(status_led_thread_stack_area),
                  &status_led_thread, NULL, NULL, NULL,
                  STATUS_LED_THREAD_PRIORITY, 0, K_NO_WAIT);
  return 0;
}

void status_led_blink_step(const struct device* status_led_dev, bool status_on,
                           status_t expected_status)
{
  if (atomic_get(&status) != expected_status)
  {
    return;
  }
  gpio_pin_set(status_led_dev, STATUS_LED_PIN, (int)status_on);
  k_sleep(K_MSEC(100));
}

void status_led_thread(void* param1, void* param2, void* param3)
{
  (void)param1;
  (void)param2;
  (void)param3;

  while (true)
  {
    switch (atomic_get(&status))
    {
      case STATUS_CONNECTING:
        status_led_blink_step(status_led_dev, false, STATUS_CONNECTING);
        status_led_blink_step(status_led_dev, true, STATUS_CONNECTING);
        status_led_blink_step(status_led_dev, false, STATUS_CONNECTING);
        status_led_blink_step(status_led_dev, true, STATUS_CONNECTING);
        status_led_blink_step(status_led_dev, false, STATUS_CONNECTING);
        status_led_blink_step(status_led_dev, false, STATUS_CONNECTING);
        status_led_blink_step(status_led_dev, false, STATUS_CONNECTING);
        status_led_blink_step(status_led_dev, false, STATUS_CONNECTING);
        status_led_blink_step(status_led_dev, false, STATUS_CONNECTING);
        status_led_blink_step(status_led_dev, false, STATUS_CONNECTING);
        break;

      case STATUS_RUNNING:
        status_led_blink_step(status_led_dev, true, STATUS_RUNNING);
        status_led_blink_step(status_led_dev, true, STATUS_RUNNING);
        status_led_blink_step(status_led_dev, true, STATUS_RUNNING);
        status_led_blink_step(status_led_dev, true, STATUS_RUNNING);
        status_led_blink_step(status_led_dev, true, STATUS_RUNNING);
        status_led_blink_step(status_led_dev, false, STATUS_RUNNING);
        status_led_blink_step(status_led_dev, false, STATUS_RUNNING);
        status_led_blink_step(status_led_dev, false, STATUS_RUNNING);
        status_led_blink_step(status_led_dev, false, STATUS_RUNNING);
        status_led_blink_step(status_led_dev, false, STATUS_RUNNING);
        break;

      case STATUS_ERROR:
        gpio_pin_set(error_led_dev, ERROR_LED_PIN, (int)true);
        gpio_pin_set(status_led_dev, STATUS_LED_PIN, (int)false);
        return;
    }
  }
}

#endif  // ifdef STATUS_USE_HW
