/*
 * Copyright (c) 2020 Robert Wilbrandt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#define DT_DRV_COMPAT st_l3gd20

#include "l3gd20.h"

#include "l3gd20_spi.h"
#include "l3gd20_util.h"

#include <string.h>
#include <drivers/sensor.h>
#include <sys/byteorder.h>
#include <errno.h>
#include <init.h>

void l3gd20_convert_temp(const struct l3gd20_data* l3gd20_data,
                         uint8_t sample_raw, struct sensor_value* val)
{
  val->val1 = (int8_t)sample_raw;
  val->val2 = 0;
}

void l3gd20_convert_gyro(const struct l3gd20_data* l3gd20_data,
                         uint8_t sample_l, uint8_t sample_h,
                         struct sensor_value* val)
{
  uint16_t sample_raw = sys_be16_to_cpu((sample_l << 8) + sample_h);
  val->val1 = (int16_t)sample_raw;
}

int l3gd20_sample_fetch(const struct device* dev, enum sensor_channel chan)
{
  struct l3gd20_data* data = dev->data;
  const struct l3gd20_config* cfg = dev->config;

  // Handle temperature data
  if (chan == SENSOR_CHAN_DIE_TEMP || chan == SENSOR_CHAN_ALL)
  {
    uint8_t temp_sample_raw;
    L3GD20_RET_STATUS_IF_ERR(l3gd20_read_reg(
        data->bus, &cfg->spi_conf, L3GD20_REG_OUT_TEMP, &temp_sample_raw));
    data->last_sample.temp =
        data->temp_offset + (data->temp_offset - temp_sample_raw);

    if (chan == SENSOR_CHAN_DIE_TEMP)
    {
      return 0;
    }
  }

  // Handle gyro data
  uint8_t from_reg, to_reg;
  size_t sample_base_idx;
  switch (chan)
  {
    case SENSOR_CHAN_GYRO_X:
      from_reg = L3GD20_REG_OUT_X_L;
      to_reg = L3GD20_REG_OUT_X_H;
      sample_base_idx = L3GD20_SAMPLE_GYRO_X_L;
      break;
    case SENSOR_CHAN_GYRO_Y:
      from_reg = L3GD20_REG_OUT_Y_L;
      to_reg = L3GD20_REG_OUT_Y_H;
      sample_base_idx = L3GD20_SAMPLE_GYRO_Y_L;
      break;
    case SENSOR_CHAN_GYRO_Z:
      from_reg = L3GD20_REG_OUT_Z_L;
      to_reg = L3GD20_REG_OUT_Z_H;
      sample_base_idx = L3GD20_SAMPLE_GYRO_Z_L;
      break;
    case SENSOR_CHAN_GYRO_XYZ:
    case SENSOR_CHAN_ALL:
      from_reg = L3GD20_REG_OUT_X_L;
      to_reg = L3GD20_REG_OUT_Z_H;
      sample_base_idx = L3GD20_SAMPLE_GYRO_X_L;
      break;

    default:
      return -ENOTSUP;
  }

  L3GD20_RET_STATUS_IF_ERR(
      l3gd20_read_regs(data->bus, &cfg->spi_conf, from_reg, to_reg,
                       &data->last_sample.gyro[sample_base_idx]));
  return 0;
}

static int l3gd20_channel_get(const struct device* dev,
                              enum sensor_channel chan,
                              struct sensor_value* val)
{
  struct l3gd20_data* data = dev->data;
  switch (chan)
  {
    case SENSOR_CHAN_DIE_TEMP:
      l3gd20_convert_temp(data, data->last_sample.temp, val);
      return 0;
    case SENSOR_CHAN_GYRO_X:
      l3gd20_convert_gyro(data, data->last_sample.gyro[L3GD20_SAMPLE_GYRO_X_L],
                          data->last_sample.gyro[L3GD20_SAMPLE_GYRO_X_H], val);
      return 0;
    case SENSOR_CHAN_GYRO_Y:
      l3gd20_convert_gyro(data, data->last_sample.gyro[L3GD20_SAMPLE_GYRO_Y_L],
                          data->last_sample.gyro[L3GD20_SAMPLE_GYRO_Y_H], val);
      return 0;
    case SENSOR_CHAN_GYRO_Z:
      l3gd20_convert_gyro(data, data->last_sample.gyro[L3GD20_SAMPLE_GYRO_Z_L],
                          data->last_sample.gyro[L3GD20_SAMPLE_GYRO_Z_H], val);
      return 0;
    case SENSOR_CHAN_GYRO_XYZ:
      for (size_t i = 0; i < 3; ++i)
      {
        l3gd20_convert_gyro(
            data, data->last_sample.gyro[L3GD20_SAMPLE_GYRO_X_L + 2 * i],
            data->last_sample.gyro[L3GD20_SAMPLE_GYRO_X_H + 2 * i], &val[i]);
      }
      return 0;

    default:
      return -ENOTSUP;
  }
}

static const struct sensor_driver_api l3gd20_driver_api = {
  .sample_fetch = l3gd20_sample_fetch,
  .channel_get = l3gd20_channel_get,
};

int l3gd20_init(const struct device* dev)
{
  struct l3gd20_data* data = dev->data;
  const struct l3gd20_config* cfg = dev->config;

  // Get spi bus
  data->bus = device_get_binding(cfg->spi_bus_label);
  if (!data->bus)
  {
    return -EINVAL;
  }

  // Get CS pin from gpio
  data->cs_ctrl.gpio_dev = device_get_binding(cfg->spi_cs_label);
  if (!data->cs_ctrl.gpio_dev)
  {
    return -ENODEV;
  }

  // Read WHO_AM_I register to confirm we got the right device
  uint8_t reg_who_am_i;
  L3GD20_RET_VAL_IF_ERR(l3gd20_read_reg(data->bus, &cfg->spi_conf,
                                        L3GD20_REG_WHOAMI, &reg_who_am_i),
                        -EIO);

  if (reg_who_am_i != L3GD20_WHOAMI)
  {
    return -EINVAL;
  }

  // Set CTRL_REG_1
  const uint8_t ctrl1_word =
      L3GD20_X_EN_BIT | L3GD20_Y_EN_BIT | L3GD20_Z_EN_BIT | L3GD20_PD_BIT;
  L3GD20_RET_VAL_IF_ERR(l3gd20_write_reg(data->bus, &cfg->spi_conf,
                                         L3GD20_REG_CTRL_REG1, ctrl1_word),
                        -EIO);

  // Read temperature offset
  L3GD20_RET_VAL_IF_ERR(l3gd20_read_reg(data->bus, &cfg->spi_conf,
                                        L3GD20_REG_OUT_TEMP, &data->temp_offset),
                        -EIO);

  // Initialize last sample
  memset(data->last_sample.gyro, 0, L3GD20_SAMPLE_MAX * sizeof(uint8_t));

  return 0;
}

// This registers the driver correctly for a given instance, called for all
// instances below
#define L3GD20_DEFINE(inst)                                                    \
  static struct l3gd20_data l3gd20_data_##inst = {                             \
    .cs_ctrl = { .gpio_pin = DT_INST_SPI_DEV_CS_GPIOS_PIN(inst),               \
                 .gpio_dt_flags = DT_INST_SPI_DEV_CS_GPIOS_FLAGS(inst) }       \
  };                                                                           \
                                                                               \
  static const struct l3gd20_config l3gd20_config_##inst = {                   \
    .spi_conf = { .frequency = DT_INST_PROP(inst, spi_max_frequency),          \
                  .operation =                                                 \
                      (SPI_OP_MODE_MASTER | SPI_MODE_CPOL | SPI_MODE_CPHA |    \
                       SPI_TRANSFER_MSB | SPI_WORD_SET(8)),                    \
                  .slave = DT_INST_REG_ADDR(inst),                             \
                  .cs = &l3gd20_data_##inst.cs_ctrl },                         \
    .spi_bus_label = DT_INST_BUS_LABEL(inst),                                  \
    .spi_cs_label = DT_INST_SPI_DEV_CS_GPIOS_LABEL(inst)                       \
  };                                                                           \
                                                                               \
  DEVICE_AND_API_INIT(l3gd20_##inst, DT_INST_LABEL(inst), &l3gd20_init,        \
                      &l3gd20_data_##inst, &l3gd20_config_##inst, POST_KERNEL, \
                      CONFIG_SENSOR_INIT_PRIORITY, &l3gd20_driver_api);

// Create instances for each devicetree mention
DT_INST_FOREACH_STATUS_OKAY(L3GD20_DEFINE)
