/*
 * Copyright (c) 2020 Robert Wilbrandt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef MICRO_ROS_ZEPHYR_DISPLAY_DEMO_STATUS_H
#define MICRO_ROS_ZEPHYR_DISPLAY_DEMO_STATUS_H

#include <zephyr.h>

typedef enum
{
  STATUS_STARTING = 0,
  STATUS_CONNECTING = 1,
  STATUS_RUNNING = 2,
  STATUS_ERROR = 3
} status_t;
extern atomic_t status;

int status_init();
int status_start();

#endif  // MICRO_ROS_ZEPHYR_DISPLAY_DEMO_STATUS_H
