zephyr_library()

zephyr_library_sources_ifdef(CONFIG_L3GD20 l3gd20.c)
