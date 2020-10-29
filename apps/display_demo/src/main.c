/*
 * Copyright (c) 2020 Robert Wilbrandt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <drivers/sensor.h>
#include <zephyr.h>

#include "display.h"
#include "status.h"

#if defined(CONFIG_L3GD20) && (CONFIG_L3GD20 == 1)
#define GYRO_NODE DT_NODELABEL(l3gd20)
#define GYRO_LABEL DT_LABEL(GYRO_NODE)
#endif

void main(void)
{
  atomic_set(&status, STATUS_CONNECTING);

  if (!display_init())
  {
    atomic_set(&status, STATUS_ERROR);
    return;
  }

#if defined(CONFIG_L3GD20) && (CONFIG_L3GD20 == 1)
  const struct device* gyro_dev = device_get_binding(GYRO_LABEL);
  if (gyro_dev == NULL)
  {
    atomic_set(&status, STATUS_ERROR);
  }
#endif

  // Use a timer instead of sleeps to ensure a consistent update rate
  // independent of processing time
  struct k_timer display_update_timer;
  k_timer_init(&display_update_timer, NULL, NULL);
  k_timer_start(&display_update_timer, K_MSEC(25), K_MSEC(25));

  while (true)
  {
    k_timer_status_sync(&display_update_timer);
    display_update();
  }
}
