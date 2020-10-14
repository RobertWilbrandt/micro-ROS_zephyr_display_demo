#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <zephyr.h>

#define RCCHECK(fn)                                                 \
  {                                                                 \
    rcl_ret_t temp_rc = fn;                                         \
    if ((temp_rc != RCL_RET_OK)) {                                  \
      printf("Failed status on line %d: %d. Aborting.\n", __LINE__, \
             (int)temp_rc);                                         \
      return;                                                       \
    }                                                               \
  }
#define RCSOFTCHECK(fn)                                               \
  {                                                                   \
    rcl_ret_t temp_rc = fn;                                           \
    if ((temp_rc != RCL_RET_OK)) {                                    \
      printf("Failed status on line %d: %d. Continuing.\n", __LINE__, \
             (int)temp_rc);                                           \
    }                                                                 \
  }

void main(void) {
  // Initialize micro-ROS
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rclc_support_t support;
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  rcl_node_t node = rcl_get_zero_initialized_node();
  RCCHECK(rclc_node_init_default(&node, "microros_zephyr_display_demo", "",
                                 &support));

  while (1) {
    k_msleep(1000);
  }

  RCCHECK(rcl_node_fini(&node));
}
