/*
 * Copyright (c) 2020 Robert Wilbrandt
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <zephyr.h>

#define UROS_THREAD_STACK_SIZE 16384
#define UROS_THREAD_PRIORITY 3

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

#include "status.h"

void uros_thread(void *param1, void *param2, void *param3);
K_THREAD_DEFINE(uros_thread_tid, UROS_THREAD_STACK_SIZE, &uros_thread, NULL,
                NULL, NULL, UROS_THREAD_PRIORITY, 0, 0);

void uros_thread(void *param1, void *param2, void *param3) {
  (void)param1;
  (void)param2;
  (void)param3;

  while (atomic_get(&status) == STATUS_STARTING) {
    k_sleep(K_MSEC(10));
  }

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

    k_sleep(K_MSEC(100));
  }

  rcl_node_t node = rcl_get_zero_initialized_node();
  RCCHECK(rclc_node_init_default(&node, "microros_zephyr_display_demo", "",
                                 &support));

  atomic_set(&status, STATUS_RUNNING);
  while (1) {
    k_sleep(K_MSEC(1000));
  }

  RCCHECK(rcl_node_fini(&node));
  RCCHECK(rclc_support_fini(&support));
}
