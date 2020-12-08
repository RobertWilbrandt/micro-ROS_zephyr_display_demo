/*
 * Copyright (c) 2020 Robert Wilbrandt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef MICRO_ROS_ZEPYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20_L3GD20_H
#define MICRO_ROS_ZEPYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20_L3GD20_H

#include <device.h>
#include <drivers/spi.h>
#include <zephyr/types.h>

#define L3GD20_WHOAMI 0xD4

#define L3GD20_CTRL_REG1_PD_BIT BIT(3)
#define L3GD20_CTRL_REG1_Z_EN_BIT BIT(2)
#define L3GD20_CTRL_REG1_Y_EN_BIT BIT(1)
#define L3GD20_CTRL_REG1_X_EN_BIT BIT(0)

enum l3gd20_sample_gyro_idx
{
  L3GD20_SAMPLE_GYRO_X_L = 0,
  L3GD20_SAMPLE_GYRO_X_H,
  L3GD20_SAMPLE_GYRO_Y_L,
  L3GD20_SAMPLE_GYRO_Y_H,
  L3GD20_SAMPLE_GYRO_Z_L,
  L3GD20_SAMPLE_GYRO_Z_H,
  L3GD20_SAMPLE_MAX
};

struct l3gd20_sample
{
  uint8_t temp;
  uint8_t gyro[L3GD20_SAMPLE_MAX];
};

struct l3gd20_data
{
  const struct device* bus;
  struct spi_cs_control cs_ctrl;

  struct l3gd20_sample last_sample;

  // Temperature sensor data
  uint8_t temp_offset;
};

struct l3gd20_config
{
  struct spi_config spi_conf;
  const char* spi_bus_label;
  const char* spi_cs_label;
};

#endif  // MICRO_ROS_ZEPYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20_L3GD20_H
