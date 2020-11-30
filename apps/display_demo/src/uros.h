#ifndef MICRO_ROS_ZEPHYR_DISPLAY_DEMO_UROS_H
#define MICRO_ROS_ZEPHYR_DISPLAY_DEMO_UROS_H

#include <rclc/rclc.h>
#include <zephyr.h>
#include <rcl/timer.h>

int uros_init();
int uros_start();

// UROS Publishers
// ----------------
struct uros_pub_handle
{
  sys_snode_t node;
  const rosidl_message_type_support_t* type;
  const char* topic_name;

  size_t pub_idx;
};

void uros_add_pub(struct uros_pub_handle* handle);
rcl_ret_t uros_pub(struct uros_pub_handle* handle, const void* msg);

// UROS Timers
struct uros_timer_handle
{
  sys_snode_t node;
  unsigned int timeout_ms;
  rcl_timer_callback_t cb;

  size_t timer_idx;
};

void uros_add_timer(struct uros_timer_handle* handle);

#endif  // MICRO_ROS_ZEPHYR_DISPLAY_DEMO_UROS_H
