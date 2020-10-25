#include "display.h"

#include <device.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <string.h>
#include <zephyr.h>

#include "status.h"

const struct device *display_dev;
status_t last_status;

lv_obj_t *status_elems[4][2];

bool display_init() {
  display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);
  if (display_dev == NULL) {
    return false;
  }

  last_status = STATUS_CONNECTING;
  memset(status_elems, (int)NULL, sizeof(lv_obj_t *) * 4 * 2);

  // Create Bottom-Left MicroROS label
  lv_obj_t *micro_ros_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(micro_ros_label, "MicroROS");
  lv_obj_align(micro_ros_label, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 10, -10);

  // Create Bottom-Right demo name label
  lv_obj_t *demo_name_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(demo_name_label, "Zephyr Display Demo");
  lv_obj_align(demo_name_label, NULL, LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10);

  // Status CONNECTING: Create Top-Right spinner
  lv_obj_t *connecting_spinner = lv_spinner_create(lv_scr_act(), NULL);
  lv_obj_set_size(connecting_spinner, 25, 25);
  lv_obj_set_style_local_line_color(connecting_spinner, LV_SPINNER_PART_INDIC,
                                    LV_STATE_DEFAULT, LV_COLOR_GRAY);
  lv_obj_set_style_local_line_width(connecting_spinner, LV_SPINNER_PART_BG,
                                    LV_STATE_DEFAULT, 5);
  lv_obj_set_style_local_line_width(connecting_spinner, LV_SPINNER_PART_INDIC,
                                    LV_STATE_DEFAULT, 5);
  lv_obj_align(connecting_spinner, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 10);
  status_elems[STATUS_CONNECTING][0] = connecting_spinner;

  // Status CONNECTING: Create label next to spinner
  lv_obj_t *connecting_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(connecting_label, "Connecting...");
  lv_obj_align(connecting_label, connecting_spinner, LV_ALIGN_OUT_LEFT_MID, -10,
               0);
  status_elems[STATUS_CONNECTING][1] = connecting_label;

  // Status RUNNING: Create green led
  lv_obj_t *running_led = lv_led_create(lv_scr_act(), NULL);
  lv_led_on(running_led);
  lv_obj_set_style_local_bg_color(running_led, LV_LED_PART_MAIN,
                                  LV_STATE_DEFAULT, LV_COLOR_GREEN);
  lv_obj_set_size(running_led, 25, 25);
  lv_obj_align(running_led, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 10);
  lv_obj_set_hidden(running_led, true);
  status_elems[STATUS_RUNNING][0] = running_led;

  // Status RUNNING: Create label next to led
  lv_obj_t *running_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(running_label, "Running");
  lv_obj_align(running_label, running_led, LV_ALIGN_OUT_LEFT_MID, -10, 0);
  lv_obj_set_hidden(running_label, true);
  status_elems[STATUS_RUNNING][1] = running_label;

  // Status ERROR: Create red led
  lv_obj_t *error_led = lv_led_create(lv_scr_act(), NULL);
  lv_led_on(error_led);
  lv_obj_set_style_local_bg_color(error_led, LV_LED_PART_MAIN, LV_STATE_DEFAULT,
                                  LV_COLOR_RED);
  lv_obj_set_size(error_led, 25, 25);
  lv_obj_align(error_led, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 0);
  lv_obj_set_hidden(error_led, true);
  status_elems[STATUS_ERROR][0] = error_led;

  // Status ERROR: Create label next to led
  lv_obj_t *error_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(error_label, "ERROR");
  lv_obj_align(error_label, error_led, LV_ALIGN_OUT_LEFT_MID, -10, 0);
  lv_obj_set_hidden(error_label, true);
  status_elems[STATUS_ERROR][1] = error_led;

  // Driver boilerplate
  lv_task_handler();
  display_blanking_off(display_dev);

  return true;
}

void display_update() {
  status_t cur_status = atomic_get(&status);

  if (cur_status != last_status) {
    const size_t num_cols = sizeof(status_elems[0]) / sizeof(lv_obj_t *);

    // Hide all elements related to old status
    for (size_t i = 0; i < num_cols; ++i) {
      if (status_elems[last_status][i] != NULL) {
        lv_obj_set_hidden(status_elems[last_status][i], true);
      }
    }

    // Show all elements for current status
    for (size_t i = 0; i < num_cols; ++i) {
      if (status_elems[cur_status][i] != NULL) {
        lv_obj_set_hidden(status_elems[cur_status][i], false);
      }
    }

    last_status = cur_status;
  }

  lv_task_handler();
}
