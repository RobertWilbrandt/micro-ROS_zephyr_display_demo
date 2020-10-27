zephyr_library()

zephyr_library_sources_ifdef(CONFIG_L3GD20H l3gd20h.c)
