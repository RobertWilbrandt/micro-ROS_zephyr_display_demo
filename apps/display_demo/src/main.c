#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <zephyr.h>

#include "status.h"

#define RCCHECK(fn)                      \
  {                                      \
    rcl_ret_t temp_rc = fn;              \
    if ((temp_rc != RCL_RET_OK)) {       \
      atomic_set(&status, STATUS_ERROR); \
      return;                            \
    }                                    \
  }
#define RCSOFTCHECK(fn)            \
  {                                \
    rcl_ret_t temp_rc = fn;        \
    if ((temp_rc != RCL_RET_OK)) { \
    }                              \
  }

void main(void) {
  atomic_set(&status, STATUS_CONNECTING);

  rcl_allocator_t allocator = rcl_get_default_allocator();

  // Initialize support. The loop with error handling is required, as the
  // initialization will fail after some time if no agent is connected
  // (on my stm32f429i-disc1 after ~55 seconds).
  rclc_support_t support;
  while (true) {
    rcl_ret_t support_init_ret =
        rclc_support_init(&support, 0, NULL, &allocator);

    if (support_init_ret == RCL_RET_OK) {
      break;
    } else {
      rclc_support_fini(&support);
      if (support_init_ret != RCL_RET_ERROR) {
        atomic_set(&status, STATUS_ERROR);
        return;
      }
    }

    k_msleep(100);
  }

  rcl_node_t node = rcl_get_zero_initialized_node();
  RCCHECK(rclc_node_init_default(&node, "microros_zephyr_display_demo", "",
                                 &support));

  atomic_set(&status, STATUS_RUNNING);
  while (1) {
    k_msleep(1000);
  }

  RCCHECK(rcl_node_fini(&node));
  RCCHECK(rclc_support_fini(&support));
}
