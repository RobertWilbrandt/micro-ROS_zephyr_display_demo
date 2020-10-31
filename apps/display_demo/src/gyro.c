/*
 * Copyright (c) 2020 Robert Wilbrandt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "gyro.h"

#include <zephyr.h>
#include <drivers/sensor.h>
#include <stdio.h>

#include "display.h"

// Do this if we have a real gyro
#define GYRO_NODE DT_ALIAS(gyro)
#if DT_NODE_HAS_STATUS(GYRO_NODE, okay)

#define GYRO_LABEL DT_LABEL(GYRO_NODE)
#define GYRO_LABEL_FORMAT "Using onboard sensor '%s'"
#define GYRO_LABEL_FULL_LEN sizeof(GYRO_LABEL_FORMAT) - 1 + sizeof(GYRO_LABEL)

const struct device* gyro_dev;

int gyro_init()
{
  gyro_dev = device_get_binding(GYRO_LABEL);
  if (gyro_dev == NULL)
  {
    return -ENODEV;
  }
  return 0;
}

int gyro_start()
{
  char gyro_label_text[GYRO_LABEL_FULL_LEN];
  sprintf(gyro_label_text, "Using onboard sensor '%s'", GYRO_LABEL);
  display_set_label_gyro(gyro_label_text);

  return 0;
}

// We don't have a gyro -> listen to uros instead
#else  // if DT_NODE_HAS_STATUS(GYRO_NODE)

int gyro_init()
{
  return 0;
}

int gyro_start()
{
  display_set_label_gyro("No onboard sensor, listening on /imu");

  return 0;
}

#endif  // if DT_NODE_HAS_STATUS(GYRO_NODE)
