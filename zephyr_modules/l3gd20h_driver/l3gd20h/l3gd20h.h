#ifndef MICRO_ROS_ZEPYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20H_L3GD20H_H
#define MICRO_ROS_ZEPYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20H_L3GD20H_H

#include <device.h>
#include <drivers/spi.h>
#include <sys/util.h>
#include <zephyr/types.h>

#define L3GD20H_SPI_READ_BIT BIT(7)

#define L3GD20H_REG_WHOAMI 0x0F

#define L3GD20H_WHOAMI 0xD4

struct l3gd20h_data {
  const struct device *bus;
  struct spi_cs_control cs_ctrl;

  int16_t x_sample;
};

struct l3gd20h_config {
  struct spi_config spi_conf;
  const char *spi_bus_label;
  const char *spi_cs_label;
};

#endif  // MICRO_ROS_ZEPYR_DISPLAY_DEMO_DRIVERS_SENSORS_L3GD20H_L3GD20H_H
