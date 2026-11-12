target_sources_ifdef(CONFIG_ZMK_BEHAVIOR_LONG_PRESS app PRIVATE
    src/behavior_long_press.c
)
target_sources_ifdef(CONFIG_TOUCAN_OS_MODE app PRIVATE src/toucan_os_mode.c)
target_sources_ifdef(CONFIG_ZMK_BEHAVIOR_OS_MODE app PRIVATE src/behavior_os_mode.c)
target_sources_ifdef(CONFIG_ZMK_BEHAVIOR_OS_SELECT app PRIVATE src/behavior_os_select.c)
