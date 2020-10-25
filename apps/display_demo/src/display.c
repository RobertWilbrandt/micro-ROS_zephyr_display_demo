#include "display.h"

#include <device.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <zephyr.h>

const struct device *display_dev;

bool display_init() {
  display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
  if (display_dev == NULL) {
    return false;
  }

  lv_obj_t *micro_ros_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(micro_ros_label, "MicroROS");
  lv_obj_align(micro_ros_label, NULL, LV_ALIGN_CENTER, 0, 0);

  lv_task_handler();
  display_blanking_off(display_dev);

  return true;
}

void display_update() { lv_task_handler(); }
