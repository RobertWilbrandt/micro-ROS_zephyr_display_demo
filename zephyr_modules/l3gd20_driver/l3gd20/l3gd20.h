/*
 * Copyright (c) 2020 Robert Wilbrandt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef MICRO_ROS_ZEPYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20_L3GD20_H
#define MICRO_ROS_ZEPYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20_L3GD20_H

#include <device.h>
#include <drivers/spi.h>
#include <sys/util.h>
#include <zephyr/types.h>

#define L3GD20_SPI_READ_BIT BIT(7)

#define L3GD20_REG_WHOAMI 0x0F
#define L3GD20_REG_CTRL_REG1 0x20
#define L3GD20_REG_OUT_TEMP 0x26

#define L3GD20_WHOAMI 0xD4

#define L3GD20_PD_BIT BIT(3)
#define L3GD20_Z_EN_BIT BIT(2)
#define L3GD20_Y_EN_BIT BIT(1)
#define L3GD20_X_EN_BIT BIT(0)

struct l3gd20_data
{
  const struct device* bus;
  struct spi_cs_control cs_ctrl;

  uint8_t temp_sample;
};

struct l3gd20_config
{
  struct spi_config spi_conf;
  const char* spi_bus_label;
  const char* spi_cs_label;
};

#endif  // MICRO_ROS_ZEPYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20_L3GD20_H
