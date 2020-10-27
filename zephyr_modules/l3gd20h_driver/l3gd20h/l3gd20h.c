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

int l3gd20h_init(const struct device *dev) { return 0; }

struct l3gd20h_data l3gd20h_driver;

DEVICE_AND_API_INIT(l3gd20h, DT_INST_LABEL(0), l3gd20h_init, &l3gd20h_driver,
                    NULL, POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,
                    &l3gd20h_driver_api);
