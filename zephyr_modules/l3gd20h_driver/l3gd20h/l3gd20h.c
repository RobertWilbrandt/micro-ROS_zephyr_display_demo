#define DT_DRV_COMPAT st_l3gd20h

#include "l3gd20h.h"

#include <drivers/sensor.h>
#include <errno.h>
#include <init.h>

int l3gd20h_sample_fetch(const struct device *dev, enum sensor_channel chan) {
  return -ENOTSUP;
}

static int l3gd20h_channel_get(const struct device *dev,
                               enum sensor_channel chan,
                               struct sensor_value *val) {
  return -ENOTSUP;
}

static const struct sensor_driver_api l3gd20h_driver_api = {
    .sample_fetch = l3gd20h_sample_fetch,
    .channel_get = l3gd20h_channel_get,
};

int l3gd20h_init(const struct device *dev) {
  struct l3gd20h_data *l3gd20h = dev->data;
  const struct l3gd20h_config *cfg = dev->config;

  // Get spi bus
  l3gd20h->bus = device_get_binding(cfg->spi_bus_label);
  if (!l3gd20h->bus) {
    return -EINVAL;
  }

  // Get CS pin from gpio
  l3gd20h->cs_ctrl.gpio_dev = device_get_binding(cfg->spi_cs_label);
  if (!l3gd20h->cs_ctrl.gpio_dev) {
    return -ENODEV;
  }

  // Read WHO_AM_I register to confirm we got the right device
  uint8_t reg_who_am_i;

  uint8_t buffer_tx[2] = {L3GD20H_SPI_READ_BIT | L3GD20H_REG_WHOAMI, 0};
  const struct spi_buf tx_buf = {.buf = buffer_tx, .len = 2};
  const struct spi_buf_set tx = {.buffers = &tx_buf, .count = 1};

  const struct spi_buf rx_buf[2] = {{.buf = NULL, .len = 1},
                                    {.buf = &reg_who_am_i, .len = 1}};
  const struct spi_buf_set rx = {.buffers = rx_buf, .count = 2};

  if (spi_transceive(l3gd20h->bus, &cfg->spi_conf, &tx, &rx) != 0) {
    return -EIO;
  }

  // Check that we are on the correct device
  if (reg_who_am_i != L3GD20H_WHOAMI) {
    return -EINVAL;
  }

  return 0;
}

// This registers the driver correctly for a given instance, called for all
// instances below
#define L3GD20H_DEFINE(inst)                                                 \
  static struct l3gd20h_data l3gd20h_data_##inst = {                         \
      .cs_ctrl = {.gpio_pin = DT_INST_SPI_DEV_CS_GPIOS_PIN(inst),            \
                  .gpio_dt_flags = DT_INST_SPI_DEV_CS_GPIOS_FLAGS(inst)}};   \
                                                                             \
  static const struct l3gd20h_config l3gd20h_config_##inst = {               \
      .spi_conf = {.frequency = DT_INST_PROP(inst, spi_max_frequency),       \
                   .operation =                                              \
                       (SPI_OP_MODE_MASTER | SPI_MODE_CPOL | SPI_MODE_CPHA | \
                        SPI_TRANSFER_MSB | SPI_WORD_SET(8)),                 \
                   .slave = DT_INST_REG_ADDR(inst),                          \
                   .cs = &l3gd20h_data_##inst.cs_ctrl},                      \
      .spi_bus_label = DT_INST_BUS_LABEL(inst),                              \
      .spi_cs_label = DT_INST_SPI_DEV_CS_GPIOS_LABEL(inst)};                 \
                                                                             \
  DEVICE_AND_API_INIT(l3gd20h_##inst, DT_INST_LABEL(inst), &l3gd20h_init,    \
                      &l3gd20h_data_##inst, &l3gd20h_config_##inst,          \
                      POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,              \
                      &l3gd20h_driver_api);

// Create instances for each devicetree mention
DT_INST_FOREACH_STATUS_OKAY(L3GD20H_DEFINE)
