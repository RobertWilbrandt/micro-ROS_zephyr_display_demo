#ifndef MICRO_ROS_ZEPHYR_DISPLAY_DEMO_UROS_H
#define MICRO_ROS_ZEPHYR_DISPLAY_DEMO_UROS_H

#include <rclc/rclc.h>
#include <zephyr.h>

struct uros_pub_handle
{
  sys_snode_t node;
  const rosidl_message_type_support_t* type;
  const char* topic_name;

  size_t pub_idx;
};

void uros_add_pub(struct uros_pub_handle* handle);
rcl_ret_t uros_pub(struct uros_pub_handle* handle, const void* msg);

int uros_init();
int uros_start();

#endif  // MICRO_ROS_ZEPHYR_DISPLAY_DEMO_UROS_H
