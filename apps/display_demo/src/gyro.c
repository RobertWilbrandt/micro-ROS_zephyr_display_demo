/*
 * Copyright (c) 2020 Robert Wilbrandt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "gyro.h"

#include <zephyr.h>
#include <drivers/sensor.h>
#include <stdio.h>
#include <sensor_msgs/msg/temperature.h>

#include "display.h"
#include "uros.h"
#include "status.h"

// Do this if we have a real gyro
#define GYRO_NODE DT_ALIAS(gyro)
#if DT_NODE_HAS_STATUS(GYRO_NODE, okay)

#define GYRO_LABEL DT_LABEL(GYRO_NODE)
#define GYRO_LABEL_FORMAT "Using onboard sensor '%s'"
#define GYRO_LABEL_FULL_LEN sizeof(GYRO_LABEL_FORMAT) - 1 + sizeof(GYRO_LABEL)

const struct device* gyro_dev;

#define GYRO_THREAD_STACK_SIZE 2048
#define GYRO_THREAD_PRIORITY 3

void gyro_thread(void* param1, void* param2, void* param3);
K_THREAD_STACK_DEFINE(gyro_thread_stack_area, GYRO_THREAD_STACK_SIZE);
struct k_thread gyro_thread_stack_data;

uros_add_pub_node_t uros_temp_pub_node;
size_t uros_temp_pub_idx;

int gyro_init()
{
  // Get device binding
  gyro_dev = device_get_binding(GYRO_LABEL);
  if (gyro_dev == NULL)
  {
    return -ENODEV;
  }

  // Create uros publisher
  uros_temp_pub_node.type = ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Temperature);
  uros_temp_pub_node.topic_name = "l3gd20_temp";
  uros_temp_pub_idx = uros_add_publisher(&uros_temp_pub_node);
  return 0;
}

int gyro_start()
{
  char gyro_label_text[GYRO_LABEL_FULL_LEN];
  sprintf(gyro_label_text, "Using onboard sensor '%s'", GYRO_LABEL);
  display_set_label_gyro(gyro_label_text);

  k_thread_create(&gyro_thread_stack_data, gyro_thread_stack_area, K_THREAD_STACK_SIZEOF(gyro_thread_stack_area),
                  &gyro_thread, NULL, NULL, NULL, GYRO_THREAD_PRIORITY, 0, K_NO_WAIT);

  return 0;
}

void gyro_thread(void* param1, void* param2, void* param3)
{
  (void)param1;
  (void)param2;
  (void)param3;

  sensor_msgs__msg__Temperature temp_msg = { 0 };
  temp_msg.header.frame_id.data = "test_frame";
  temp_msg.header.frame_id.size = strlen(temp_msg.header.frame_id.data);
  temp_msg.variance = 0;
  temp_msg.temperature = 42.0;

  while (atomic_get(&status) != STATUS_RUNNING)
  {
    k_sleep(K_MSEC(100));
  }

  while (true)
  {
    if (uros_publish(uros_temp_pub_idx, &temp_msg) != RCL_RET_OK)
    {
      atomic_set(&status, STATUS_ERROR);
      return;
    }

    k_sleep(K_MSEC(500));
  }
}

// We don't have a gyro -> listen to uros instead
#else  // if DT_NODE_HAS_STATUS(GYRO_NODE)

int gyro_init()
{
  return 0;
}

int gyro_start()
{
  display_set_label_gyro("No onboard sensor, listening on /imu");

  return 0;
}

#endif  // if DT_NODE_HAS_STATUS(GYRO_NODE)
