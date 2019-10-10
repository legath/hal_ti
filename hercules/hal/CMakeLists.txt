if(CONFIG_HAS_HERCULES_HAL)

# Add include path
zephyr_include_directories(.)

# Add sources
zephyr_sources(source/errata_SSWF021_45.c)
zephyr_sources(source/esm.c)
zephyr_sources(source/gio.c)
zephyr_sources(source/notification.c)
zephyr_sources(source/pinmux.c)
zephyr_sources(source/sci.c)
zephyr_sources(source/sys_core.s)
zephyr_sources(source/sys_selftest.c)
zephyr_sources(source/sys_startup.c)
zephyr_sources(source/sys_vim.c)
zephyr_sources(source/system.c)

endif()
