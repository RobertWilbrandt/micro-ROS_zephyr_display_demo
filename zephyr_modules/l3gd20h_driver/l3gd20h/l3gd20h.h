#ifndef MICRO_ROS_ZEPYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20H_L3GD20H_H
#define MICRO_ROS_ZEPYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20H_L3GD20H_H

#include <device.h>
#include <zephyr/types.h>

struct l3gd20h_data {
  int16_t x_sample;
};

struct l3gd20h_config {};

#endif  // MICRO_ROS_ZEPYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20H_L3GD20H_H
