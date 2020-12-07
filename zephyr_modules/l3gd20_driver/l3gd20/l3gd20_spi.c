/*
 * Copyright (c) 2020 Robert Wilbrandt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "l3gd20_spi.h"

#include "l3gd20_util.h"

#include <drivers/spi.h>
#include <errno.h>

int l3gd20_read_reg(const struct device* spi_bus,
                    const struct spi_config* spi_conf, enum l3gd20_reg reg,
                    uint8_t* value)
{
  uint8_t read_cmd =
      reg | (L3GD20_SPI_READ_BIT &
             ~L3GD20_SPI_MS_BIT);  // Read, don't increment address
  const struct spi_buf tx_buf[2] = { { .buf = &read_cmd, .len = 1 },
                                     { .buf = NULL, .len = 1 } };
  const struct spi_buf_set tx = { .buffers = tx_buf, .count = 1 };

  const struct spi_buf rx_buf[2] = { { .buf = NULL, .len = 1 },
                                     { .buf = value, .len = 1 } };
  const struct spi_buf_set rx = { .buffers = rx_buf, .count = 2 };

  L3GD20_RET_VAL_IF_ERR(spi_transceive(spi_bus, spi_conf, &tx, &rx), -EIO);
  return 0;
}

int l3gd20_read_regs(const struct device* spi_bus,
                     const struct spi_config* spi_conf, enum l3gd20_reg from,
                     enum l3gd20_reg to, uint8_t* values)
{
  size_t num_regs = (to - from) + 1;

  uint8_t read_cmd = from | (L3GD20_SPI_READ_BIT |
                             L3GD20_SPI_MS_BIT);  // Read, increment address
  const struct spi_buf tx_buf[2] = { { .buf = &read_cmd, .len = 1 },
                                     { .buf = NULL, .len = num_regs } };
  const struct spi_buf_set tx = { .buffers = tx_buf, .count = 2 };

  const struct spi_buf rx_buf[2] = { { .buf = NULL, .len = 1 },
                                     { .buf = values, .len = num_regs } };
  const struct spi_buf_set rx = { .buffers = rx_buf, .count = 2 };

  L3GD20_RET_VAL_IF_ERR(spi_transceive(spi_bus, spi_conf, &tx, &rx), -EIO);
  return 0;
}

int l3gd20_write_reg(const struct device* spi_bus,
                     const struct spi_config* spi_conf, enum l3gd20_reg reg,
                     uint8_t value)
{
  uint8_t buffer_tx[2] = { reg & ~L3GD20_SPI_READ_BIT, value };
  const struct spi_buf tx_buf = { .buf = buffer_tx, .len = 2 };
  const struct spi_buf_set tx = { .buffers = &tx_buf, .count = 1 };

  const struct spi_buf rx_buf = { .buf = NULL, .len = 1 };
  const struct spi_buf_set rx = { .buffers = &rx_buf, .count = 1 };

  L3GD20_RET_VAL_IF_ERR(spi_transceive(spi_bus, spi_conf, &tx, &rx), -EIO);

  return 0;
}
