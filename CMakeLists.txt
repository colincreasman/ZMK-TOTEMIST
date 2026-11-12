target_sources_ifdef(CONFIG_ZMK_BEHAVIOR_LONG_PRESS app PRIVATE
    src/behavior_long_press.c
)

if ((NOT CONFIG_ZMK_SPLIT) OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
    target_sources_ifdef(CONFIG_ZMK_BEHAVIOR_HYPER_GUARD app PRIVATE
        src/behavior_hyper_guard.c
    )
endif()
