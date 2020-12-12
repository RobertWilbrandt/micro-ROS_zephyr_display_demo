/*
 * Copyright (c) 2020 Robert Wilbrandt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef MICRO_ROS_ZEPHYR_DISPLAY_DEMO_DISPLAY_H
#define MICRO_ROS_ZEPHYR_DISPLAY_DEMO_DISPLAY_H

#include <zephyr.h>

int display_init();
void display_update();

void display_set_label_gyro(const char* text);

#endif  // MICRO_ROS_ZEPHYR_DISPLAY_DEMO_DISPLAY_H
