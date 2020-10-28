#define DT_DRV_COMPAT st_l3gd20h

#include "l3gd20h.h"

#include <drivers/sensor.h>
#include <init.h>

int l3gd20h_sample_fetch(const struct device *dev, enum sensor_channel chan) {
  struct l3gd20h_data *data = dev->data;

  data->x_sample = 123;

  return 0;
}

static int l3gd20h_channel_get(const struct device *dev,
                               enum sensor_channel chan,
                               struct sensor_value *val) {
  val->val1 = 42;
  val->val2 = 123;
  return 0;
}

static const struct sensor_driver_api l3gd20h_driver_api = {
    .sample_fetch = l3gd20h_sample_fetch,
    .channel_get = l3gd20h_channel_get,
};

int l3gd20h_init(const struct device *dev) {
  struct l3gd20h_data *l3gd20h = dev->data;
  return 0;
}

// This registers the driver correctly for a given instance, called for all
// instances below
#define L3GD20H_DEFINE(inst)                                              \
  static struct l3gd20h_data l3gd20h_data_##inst;                         \
  static const struct l3gd20h_config l3gd20h_config_##inst = {            \
                                                                          \
  };                                                                      \
  DEVICE_AND_API_INIT(l3gd20h_##inst, DT_INST_LABEL(inst), &l3gd20h_init, \
                      &l3gd20h_data_##inst, &l3gd20h_config_##inst,       \
                      POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,           \
                      &l3gd20h_driver_api);

// Create instances for each devicetree mention
DT_INST_FOREACH_STATUS_OKAY(L3GD20H_DEFINE)
