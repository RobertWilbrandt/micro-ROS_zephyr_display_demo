/*
 * Copyright (c) 2020 Robert Wilbrandt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef MICRO_ROS_ZEPHYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20_L3GD20_UTIL_H
#define MICRO_ROS_ZEPHYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20_L3GD20_UTIL_H

#define L3GD20_RET_STATUS_IF_ERR(func)                                                                                 \
  {                                                                                                                    \
    int status = func;                                                                                                 \
    if (status != 0)                                                                                                   \
    {                                                                                                                  \
      return status;                                                                                                   \
    }                                                                                                                  \
  }

#define L3GD20_RET_VAL_IF_ERR(func, val)                                                                               \
  {                                                                                                                    \
    if (func != 0)                                                                                                     \
    {                                                                                                                  \
      return val;                                                                                                      \
    }                                                                                                                  \
  }

#endif  // MICRO_ROS_ZEPHYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20_L3GD20_UTIL_H
