#ifndef MICRO_ROS_ZEPHYR_DISPLAY_DEMO_UROS_H
#define MICRO_ROS_ZEPHYR_DISPLAY_DEMO_UROS_H

#include <rclc/rclc.h>
#include <zephyr.h>

struct uros_add_pub_node
{
  sys_snode_t node;
  const rosidl_message_type_support_t* type;
  const char* topic_name;
};

size_t uros_add_publisher(struct uros_add_pub_node* node);
rcl_ret_t uros_publish(size_t pub_index, const void* msg);

int uros_init();
int uros_start();

#endif  // MICRO_ROS_ZEPHYR_DISPLAY_DEMO_UROS_H
