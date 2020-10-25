#include "display.h"

#include <device.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <zephyr.h>

#include "status.h"

const struct device *display_dev;
status_t last_status;

lv_obj_t *connecting_spinner;
lv_obj_t *connecting_label;

bool display_init() {
  display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
  if (display_dev == NULL) {
    return false;
  }

  last_status = STATUS_CONNECTING;

  // Create Bottom-Left MicroROS label
  lv_obj_t *micro_ros_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(micro_ros_label, "MicroROS");
  lv_obj_align(micro_ros_label, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10);

  // Create Bottom-Right demo name label
  lv_obj_t *demo_name_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(demo_name_label, "Zephyr Display Demo");
  lv_obj_align(demo_name_label, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10);

  // Create Top-Right Connecting spinner
  connecting_spinner = lv_spinner_create(lv_scr_act(), NULL);
  lv_obj_set_size(connecting_spinner, 25, 25);
  lv_obj_set_style_local_line_color(connecting_spinner, LV_SPINNER_PART_INDIC,
                                    LV_STATE_DEFAULT, LV_COLOR_GRAY);
  lv_obj_set_style_local_line_width(connecting_spinner, LV_SPINNER_PART_BG,
                                    LV_STATE_DEFAULT, 5);
  lv_obj_set_style_local_line_width(connecting_spinner, LV_SPINNER_PART_INDIC,
                                    LV_STATE_DEFAULT, 5);
  lv_obj_align(connecting_spinner, lv_scr_act(), LV_ALIGN_IN_TOP_RIGHT, -10,
               10);

  // Create "Connecting..." label next to spinner
  connecting_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(connecting_label, "Connecting...");
  lv_obj_align(connecting_label, connecting_spinner, LV_ALIGN_OUT_LEFT_MID, -10,
               0);

  // Driver boilerplate
  lv_task_handler();
  display_blanking_off(display_dev);

  return true;
}

void display_update() {
  status_t cur_status = atomic_get(&status);

  if (cur_status != last_status) {
    // First handle previous state destruction
    switch (last_status) {
      case STATUS_CONNECTING:
        lv_obj_set_hidden(connecting_label, true);
        lv_obj_set_hidden(connecting_spinner, true);
        break;

      default:
        break;
    }

    // Now handle new status
    switch (cur_status) {
      case STATUS_CONNECTING:
        lv_obj_set_hidden(connecting_label, false);
        lv_obj_set_hidden(connecting_spinner, false);
        break;

      default:
        break;
    }

    last_status = cur_status;
  }

  lv_task_handler();
}
