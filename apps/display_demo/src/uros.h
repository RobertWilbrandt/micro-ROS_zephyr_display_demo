#ifndef MICRO_ROS_ZEPHYR_DISPLAY_DEMO_UROS_H
#define MICRO_ROS_ZEPHYR_DISPLAY_DEMO_UROS_H

#include <rclc/rclc.h>
#include <zephyr.h>

typedef struct
{
  sys_snode_t node;
  const rosidl_message_type_support_t* type;
  const char* topic_name;
} uros_add_pub_node_t;

size_t uros_add_publisher(uros_add_pub_node_t* node);
rcl_ret_t uros_publish(size_t pub_index, const void* msg);

int uros_init();
int uros_start();

#endif  // MICRO_ROS_ZEPHYR_DISPLAY_DEMO_UROS_H
