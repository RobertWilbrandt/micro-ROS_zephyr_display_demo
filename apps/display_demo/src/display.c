#include "display.h"

#include <device.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <zephyr.h>

const struct device *display_dev;

lv_style_t spinner_style;

bool display_init() {
  display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
  if (display_dev == NULL) {
    return false;
  }

  // Create Bottom-Left MicroROS label
  lv_obj_t *micro_ros_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(micro_ros_label, "MicroROS");
  lv_obj_align(micro_ros_label, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10);

  // Create Bottom-Right demo name label
  lv_obj_t *demo_name_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(demo_name_label, "Zephyr Display Demo");
  lv_obj_align(demo_name_label, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10);

  // Create Top-Right Connecting spinner
  lv_obj_t *connect_spinner = lv_spinner_create(lv_scr_act(), NULL);
  lv_obj_set_size(connect_spinner, 25, 25);
  lv_obj_set_style_local_line_color(connect_spinner, LV_SPINNER_PART_INDIC,
                                    LV_STATE_DEFAULT, LV_COLOR_GRAY);
  lv_obj_set_style_local_line_width(connect_spinner, LV_SPINNER_PART_BG,
                                    LV_STATE_DEFAULT, 5);
  lv_obj_set_style_local_line_width(connect_spinner, LV_SPINNER_PART_INDIC,
                                    LV_STATE_DEFAULT, 5);
  lv_obj_align(connect_spinner, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 10);

  // Create "Connecting..." label next to spinner
  lv_obj_t *connect_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(connect_label, "Connecting...");
  lv_obj_align(connect_label, connect_spinner, LV_ALIGN_OUT_LEFT_MID, 0, 0);

  lv_task_handler();
  display_blanking_off(display_dev);

  return true;
}

void display_update() { lv_task_handler(); }
