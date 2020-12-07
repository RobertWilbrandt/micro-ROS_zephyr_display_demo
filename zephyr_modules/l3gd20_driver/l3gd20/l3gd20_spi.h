/*
 * Copyright (c) 2020 Robert Wilbrandt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef MICRO_ROS_ZEPHYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20_L3GD20_SPI_H
#define MICRO_ROS_ZEPHYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20_L3GD20_SPI_H

#include <zephyr/types.h>
#include <device.h>
#include <drivers/spi.h>
#include <sys/util.h>

#define L3GD20_SPI_READ_BIT BIT(7)
#define L3GD20_SPI_MS_BIT BIT(6)

enum l3gd20_reg
{
  L3GD20_REG_WHOAMI = 0x0F,
  L3GD20_REG_CTRL_REG1 = 0x20,
  L3GD20_REG_OUT_TEMP = 0x26,
  L3GD20_REG_OUT_X_L = 0x28,
  L3GD20_REG_OUT_X_H = 0x29,
  L3GD20_REG_OUT_Y_L = 0x2A,
  L3GD20_REG_OUT_Y_H = 0x2B,
  L3GD20_REG_OUT_Z_L = 0x2C,
  L3GD20_REG_OUT_Z_H = 0x2D
};

int l3gd20_read_reg(const struct device* spi_bus, const struct spi_config* spi_conf, enum l3gd20_reg reg,
                    uint8_t* value);
int l3gd20_read_regs(const struct device* spi_bus, const struct spi_config* spi_conf, enum l3gd20_reg from,
                     enum l3gd20_reg to, uint8_t* values);

int l3gd20_write_reg(const struct device* spi_bus, const struct spi_config* spi_conf, enum l3gd20_reg reg,
                     uint8_t value);

#endif  // MICRO_ROS_ZEPHYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20_L3GD20_SPI_H
