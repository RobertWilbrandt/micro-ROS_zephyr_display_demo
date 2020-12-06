/*
 * Copyright (c) 2020 Robert Wilbrandt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "uros.h"

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <zephyr.h>
#include <stdlib.h>

#define UROS_THREAD_STACK_SIZE 16384
#define UROS_THREAD_PRIORITY 3

#define RCCHECK(fn)                                                                                                    \
  {                                                                                                                    \
    rcl_ret_t temp_rc = fn;                                                                                            \
    if ((temp_rc != RCL_RET_OK))                                                                                       \
    {                                                                                                                  \
      atomic_set(&status, STATUS_ERROR);                                                                               \
      return;                                                                                                          \
    }                                                                                                                  \
  }
#define RCSOFTCHECK(fn)                                                                                                \
  {                                                                                                                    \
    rcl_ret_t temp_rc = fn;                                                                                            \
    if ((temp_rc != RCL_RET_OK)) {}                                                                                    \
  }

#include "status.h"

// Publisher infrastructure
sys_slist_t uros_pub_list = SYS_SFLIST_STATIC_INIT(&uros_pub_list);
size_t uros_pub_list_cnt = 0;
rcl_publisher_t* uros_publishers = NULL;

void uros_add_pub(struct uros_pub_handle* handle)
{
  sys_slist_append(&uros_pub_list, &handle->node);
  handle->pub_idx = uros_pub_list_cnt++;
}

rcl_ret_t uros_pub(struct uros_pub_handle* handle, const void* msg)
{
  return rcl_publish(&uros_publishers[handle->pub_idx], msg, NULL);
}

// Timer infrastructure
sys_slist_t uros_timer_list = SYS_SFLIST_STATIC_INIT(&uros_timer_list);
size_t uros_timer_list_cnt = 0;
rcl_timer_t* uros_timers = NULL;

void uros_add_timer(struct uros_timer_handle* handle)
{
  sys_slist_append(&uros_timer_list, &handle->node);
  handle->timer_idx = uros_timer_list_cnt++;
}

// Thread structures
void uros_thread(void* param1, void* param2, void* param3);
K_THREAD_STACK_DEFINE(uros_thread_stack_area, UROS_THREAD_STACK_SIZE);
struct k_thread uros_thread_stack_data;

int uros_init()
{
  return 0;
}

int uros_start()
{
  k_thread_create(&uros_thread_stack_data, uros_thread_stack_area, K_THREAD_STACK_SIZEOF(uros_thread_stack_area),
                  &uros_thread, NULL, NULL, NULL, UROS_THREAD_PRIORITY, 0, K_NO_WAIT);
  return 0;
}

void uros_thread(void* param1, void* param2, void* param3)
{
  (void)param1;
  (void)param2;
  (void)param3;

  // Start by allocating all required support structures
  if (uros_pub_list_cnt > 0)
  {
    uros_publishers = (rcl_publisher_t*)malloc(uros_pub_list_cnt * sizeof(rcl_publisher_t));
    if (uros_publishers == NULL)
    {
      atomic_set(&status, STATUS_ERROR);
      return;
    }
    memset(uros_publishers, 0, uros_pub_list_cnt * sizeof(rcl_publisher_t));
  }

  if (uros_timer_list_cnt > 0)
  {
    uros_timers = (rcl_timer_t*)malloc(uros_timer_list_cnt * sizeof(rcl_timer_t));
    if (uros_timers == NULL)
    {
      atomic_set(&status, STATUS_ERROR);
      return;
    }
    memset(uros_timers, 0, uros_timer_list_cnt * sizeof(rcl_timer_t));
  }

  // Start uros initialization
  rcl_allocator_t allocator = rcl_get_default_allocator();

  // Initialize support. The loop with error handling is required, as the
  // initialization will fail after some time if no agent is connected
  // (on my stm32f429i-disc1 after ~55 seconds).
  rclc_support_t support;
  while (true)
  {
    rcl_ret_t support_init_ret = rclc_support_init(&support, 0, NULL, &allocator);

    if (support_init_ret == RCL_RET_OK)
    {
      break;
    }
    else
    {
      rclc_support_fini(&support);
      if (support_init_ret != RCL_RET_ERROR)
      {
        atomic_set(&status, STATUS_ERROR);
        return;
      }
    }

    k_sleep(K_MSEC(100));
  }

  rcl_node_t node = rcl_get_zero_initialized_node();
  RCCHECK(rclc_node_init_default(&node, "microros_zephyr_display_demo", "", &support));

  // Add publishers
  size_t cur_node_i = 0;
  for (struct uros_pub_handle* cur_node = SYS_SLIST_PEEK_HEAD_CONTAINER(&uros_pub_list, cur_node, node);
       cur_node != NULL; cur_node = SYS_SLIST_PEEK_NEXT_CONTAINER(cur_node, node))
  {
    rcl_ret_t pub_init_ret =
        rclc_publisher_init_default(&uros_publishers[cur_node_i], &node, cur_node->type, cur_node->topic_name);
    if (pub_init_ret != RCL_RET_OK)
    {
      atomic_set(&status, STATUS_ERROR);
      return;
    }

    ++cur_node_i;
  }

  if (cur_node_i != uros_pub_list_cnt)
  {
    atomic_set(&status, STATUS_ERROR);
    return;
  }

  // Add timers
  size_t cur_timer_i = 0;
  for (struct uros_timer_handle* cur_timer = SYS_SLIST_PEEK_HEAD_CONTAINER(&uros_timer_list, cur_timer, node);
       cur_timer != NULL; cur_timer = SYS_SLIST_PEEK_NEXT_CONTAINER(cur_timer, node))
  {
    rcl_ret_t timer_init_ret = rclc_timer_init_default(&uros_timers[cur_timer_i], &support,
                                                       RCL_MS_TO_NS(cur_timer->timeout_ms), cur_timer->cb);
    if (timer_init_ret != RCL_RET_OK)
    {
      atomic_set(&status, STATUS_ERROR);
      return;
    }

    ++cur_timer_i;
  }

  if (cur_timer_i != uros_timer_list_cnt)
  {
    atomic_set(&status, STATUS_ERROR);
    return;
  }

  // Create executor
  rclc_executor_t executor;
  rclc_executor_init(&executor, &support.context, uros_timer_list_cnt, &allocator);
  for (size_t i = 0; i < uros_timer_list_cnt; ++i)
  {
    RCCHECK(rclc_executor_add_timer(&executor, &uros_timers[i]));
  }

  atomic_set(&status, STATUS_RUNNING);
  rclc_executor_spin(&executor);

  RCCHECK(rclc_executor_fini(&executor));
  RCCHECK(rcl_node_fini(&node));
  RCCHECK(rclc_support_fini(&support));
}
