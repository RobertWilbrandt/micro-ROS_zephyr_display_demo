# micro-ROS Zephyr Display demo

This project showcases the use of a display peripheral on the [stm32f429i-disc1](https://www.st.com/en/evaluation-tools/32f429idiscovery.html) board in the [ROS 2 framework](https://index.ros.org/doc/ros2/) using [micro-ROS](https://micro-ros.github.io/), [LVGL](https://lvgl.io/) and the [Zephyr RTOS](https://zephyrproject.org/).

## Limitations

This project uses *micro-ROS* on the *stm32f429i-disc1* board. This combination is not yet supported by the *micro-ROS* project.

The *Zephyr* display-driver used in this project only works with the SPI-based interface to the *ILI9341* display driver. This means it cannot take advantage of the powerful 24-bit parallel RGB interface and the integrated graphics accelerator.
