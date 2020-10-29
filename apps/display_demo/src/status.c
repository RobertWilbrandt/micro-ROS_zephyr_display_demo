/*
 * Copyright (c) 2020 Robert Wilbrandt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "status.h"

#include <drivers/gpio.h>
#include <zephyr.h>

atomic_t status = ATOMIC_INIT(STATUS_STARTING);

#if defined(CONFIG_GPIO) && (CONFIG_GPIO == 1)

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

void status_led_thread(void* param1, void* param2, void* param3);
K_THREAD_DEFINE(status_led_thread_tid, STATUS_LED_THREAD_STACK_SIZE, &status_led_thread, NULL, NULL, NULL,
                STATUS_LED_THREAD_PRIORITY, 0, 0);

void status_led_blink_step(const struct device* status_led_dev, bool status_on, status_t expected_status)
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

  while (atomic_get(&status) == STATUS_STARTING)
  {
    k_sleep(K_MSEC(10));
  }

  const struct device* status_led_dev = device_get_binding(STATUS_LED_LABEL);
  if (gpio_pin_configure(status_led_dev, STATUS_LED_PIN, GPIO_OUTPUT_ACTIVE | STATUS_LED_FLAGS) != 0)
  {
    atomic_set(&status, STATUS_ERROR);
  }

  const struct device* error_led_dev = device_get_binding(ERROR_LED_LABEL);
  if (gpio_pin_configure(error_led_dev, ERROR_LED_PIN, GPIO_OUTPUT_ACTIVE | ERROR_LED_FLAGS) != 0)
  {
    atomic_set(&status, STATUS_ERROR);
  }
  gpio_pin_set(error_led_dev, ERROR_LED_PIN, (int)false);

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

#endif
