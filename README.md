[![Build badge](https://github.com/RobertWilbrandt/micro-ROS_zephyr_display_demo/workflows/CI/badge.svg?branch=foxy&event=push)](https://github.com/RobertWilbrandt/micro-ROS_zephyr_display_demo/actions)

# micro-ROS Zephyr Display demo

This project showcases the use of a display peripheral on the [stm32f429i-disc1](https://www.st.com/en/evaluation-tools/32f429idiscovery.html) board in the [ROS 2 framework](https://index.ros.org/doc/ros2/) using [micro-ROS](https://micro-ros.github.io/), [LVGL](https://lvgl.io/) and the [Zephyr RTOS](https://zephyrproject.org/).

## Limitations

This project uses *micro-ROS* on the *stm32f429i-disc1* board, which is not yet supported by the *micro-ROS* project.
It was developed against an older version of this board, which is why it probably won't work with current versions (most significant: It uses the L3GD20 MEMS gyroscope).

The *Zephyr* display-driver used in this project only works with the SPI-based interface to the *ILI9341* display driver. This means it cannot take advantage of the powerful 24-bit parallel RGB interface and the integrated graphics accelerator.

I hope this project can still serve as a demonstrator and template for *micro-ROS* and *Zephyr* projects.

## Building

This project is set up so that it can be built right inside a ROS 2 workspace. This section assumes you already have a *micro-ROS* workspace set up for this, if you haven't you can take a look at the [micro-ROS tutorial](https://micro-ros.github.io/docs/tutorials/core/first_application_rtos/zephyr/).

Inside this workspace, you start by cloning this repository:

```console
$ ls
build  firmware  install  log  src
$ git clone https://github.com/RobertWilbrandt/micro-ROS_zephyr_display_demo src/micro-ROS_zephyr_display_demo
```

You now need to export ```UROS_CUSTOM_APP_FOLDER``` to let *micro-ROS* know where to look for the microcontroller application. After this, you should be able to build and flash the application to your board:

```console
$ source /opt/ros/foxy/setup.bash
$ colcon build
$ source install/setup.zsh
$ export UROS_CUSTOM_APP_FOLDER=$(pwd)/src/micro-ROS_zephyr_display_demo/uros_apps
$ ros2 run micro_ros_setup build_firmware.sh -f
$ ros2 run micro_ros_setup flash_firmware.sh
```

I recommend to use the ```-f``` switch (means *fast*) nearly every time, as it means the command won't completely rebuild the ```mcu_ws``` every time you just want to test a little change in your code.
