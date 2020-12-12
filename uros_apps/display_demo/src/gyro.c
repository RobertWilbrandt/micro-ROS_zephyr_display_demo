/*
 * Copyright (c) 2020 Robert Wilbrandt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "gyro.h"

#include <zephyr.h>
#include <stdio.h>

#include "display.h"
#include "uros.h"
#include "status.h"

// Do this if we have a real gyro
#define GYRO_NODE DT_ALIAS(gyro)
#if DT_NODE_HAS_STATUS(GYRO_NODE, okay)

#include <time.h>
#include <sensor_msgs/msg/temperature.h>
#include <sensor_msgs/msg/imu.h>
#include <drivers/sensor.h>

// HW handles & information
#define GYRO_LABEL DT_LABEL(GYRO_NODE)
#define GYRO_LABEL_FORMAT "Using onboard sensor '%s'"
#define GYRO_LABEL_FULL_LEN sizeof(GYRO_LABEL_FORMAT) - 1 + sizeof(GYRO_LABEL)
const struct device* gyro_dev;

// UROS handles
struct uros_pub_handle uros_temp_pub;
struct uros_pub_handle uros_imu_pub;

struct uros_timer_handle uros_temp_timer;
struct uros_timer_handle uros_imu_timer;

void gyro_temp_timer_cb(rcl_timer_t* timer, int64_t last_call_time);
void gyro_imu_timer_cb(rcl_timer_t* timer, int64_t last_call_time);

int gyro_init()
{
  // Get device binding
  gyro_dev = device_get_binding(GYRO_LABEL);
  if (gyro_dev == NULL)
  {
    return -ENODEV;
  }

  // Create uros publisher
  uros_temp_pub.type =
      ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Temperature);
  uros_temp_pub.topic_name = "l3gd20_temp";
  uros_add_pub(&uros_temp_pub);

  uros_imu_pub.type = ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu);
  uros_imu_pub.topic_name = "l3gd20_gyro";
  uros_add_pub(&uros_imu_pub);

  uros_temp_timer.timeout_ms = 1000;
  uros_temp_timer.cb = &gyro_temp_timer_cb;
  uros_add_timer(&uros_temp_timer);

  uros_imu_timer.timeout_ms = 100;
  uros_imu_timer.cb = &gyro_imu_timer_cb;
  uros_add_timer(&uros_imu_timer);

  return 0;
}

int gyro_start()
{
  char gyro_label_text[GYRO_LABEL_FULL_LEN];
  sprintf(gyro_label_text, "Using onboard sensor '%s'", GYRO_LABEL);
  display_set_label_gyro(gyro_label_text);

  return 0;
}

void gyro_temp_timer_cb(rcl_timer_t* timer, int64_t last_call_time)
{
  // Fetch sensor data
  if (sensor_sample_fetch_chan(gyro_dev, SENSOR_CHAN_DIE_TEMP) != 0)
  {
    atomic_set(&status, STATUS_ERROR);
    return;
  }

  // Read sensor values
  struct sensor_value temp;
  if (sensor_channel_get(gyro_dev, SENSOR_CHAN_DIE_TEMP, &temp) != 0)
  {
    atomic_set(&status, STATUS_ERROR);
    return;
  };

  sensor_msgs__msg__Temperature temp_msg = { 0 };
  temp_msg.header.frame_id.data = "";
  temp_msg.header.frame_id.size = strlen(temp_msg.header.frame_id.data);
  temp_msg.temperature = sensor_value_to_double(&temp);

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  temp_msg.header.stamp.sec = ts.tv_sec;
  temp_msg.header.stamp.nanosec = ts.tv_nsec;

  if (uros_pub(&uros_temp_pub, &temp_msg) != RCL_RET_OK)
  {
    atomic_set(&status, STATUS_ERROR);
    return;
  }
}

void gyro_imu_timer_cb(rcl_timer_t* timer, int64_t last_call_time)
{
  // Fetch sensor data
  if (sensor_sample_fetch_chan(gyro_dev, SENSOR_CHAN_GYRO_XYZ) != 0)
  {
    atomic_set(&status, STATUS_ERROR);
    return;
  }

  // Read sensor values
  struct sensor_value data[3];
  if (sensor_channel_get(gyro_dev, SENSOR_CHAN_GYRO_XYZ, data) != 0)
  {
    atomic_set(&status, STATUS_ERROR);
    return;
  }

  sensor_msgs__msg__Imu imu_msg = { 0 };
  imu_msg.header.frame_id.data = "";
  imu_msg.header.frame_id.size = strlen(imu_msg.header.frame_id.data);

  // Indicate that we cannot measure orientation and linear acceleration
  imu_msg.orientation.x = 0.0;
  imu_msg.orientation.y = 0.0;
  imu_msg.orientation.z = 0.0;
  imu_msg.orientation.w = 0.0;
  imu_msg.orientation_covariance[0] = -1.0;
  memset(&imu_msg.orientation_covariance[1], 0, 8);

  imu_msg.linear_acceleration.x = 0.0;
  imu_msg.linear_acceleration.y = 0.0;
  imu_msg.linear_acceleration.z = 0.0;
  imu_msg.linear_acceleration_covariance[0] = -1.0;
  memset(&imu_msg.linear_acceleration_covariance[1], 0, 8);

  // Indicate that we don't know the covariance of our angular velocity
  imu_msg.angular_velocity.x = sensor_value_to_double(&data[0]);
  imu_msg.angular_velocity.y = sensor_value_to_double(&data[1]);
  imu_msg.angular_velocity.z = sensor_value_to_double(&data[2]);
  memset(imu_msg.angular_velocity_covariance, 0, 9);

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  imu_msg.header.stamp.sec = ts.tv_sec;
  imu_msg.header.stamp.nanosec = ts.tv_nsec;

  if (uros_pub(&uros_imu_pub, &imu_msg) != RCL_RET_OK)
  {
    atomic_set(&status, STATUS_ERROR);
    return;
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
