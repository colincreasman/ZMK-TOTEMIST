#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define LOG_MODULE_DECLARE(...)
#define DT_HAS_COMPAT_STATUS_OKAY(...) 1
#define DT_INST_FOREACH_STATUS_OKAY(...)
#define ZMK_BEHAVIOR_OPAQUE 0

struct device { const void *config; void *data; };
struct zmk_behavior_binding { const char *behavior_dev; uint32_t param1, param2; };
struct zmk_behavior_binding_event { uint32_t position; int64_t timestamp; uint8_t source; };
struct behavior_driver_api {
    int (*binding_pressed)(struct zmk_behavior_binding *, struct zmk_behavior_binding_event);
    int (*binding_released)(struct zmk_behavior_binding *, struct zmk_behavior_binding_event);
};
static struct device device;
static const struct device *zmk_behavior_get_binding(const char *name) {
    (void)name;
    return &device;
}
#include "../src/toucan_os_mode.h"
static enum toucan_os_mode mode;
enum toucan_os_mode toucan_os_mode_get(void) { return mode; }
static unsigned calls;
static bool last_pressed;
static struct zmk_behavior_binding invoked;
static struct zmk_behavior_binding_event invoked_event;
static int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event, bool pressed) {
    invoked = *binding;
    invoked_event = event;
    last_pressed = pressed;
    calls++;
    return 0;
}
#include "../src/behavior_os_select.c"

int main(void) {
    struct behavior_os_select_config config = {
        .bindings = {{"cmd", 11, 12}, {"ctrl", 21, 22}},
    };
    struct behavior_os_select_data data;
    device = (struct device){&config, &data};
    behavior_os_select_init(&device);
    struct zmk_behavior_binding binding = {.behavior_dev = "os_copy"};
    struct zmk_behavior_binding_event event = {.position = 22, .source = 1, .timestamp = 123};
    on_keymap_binding_released(&binding, event);
    assert(calls == 0);
    mode = TOUCAN_OS_MAC;
    on_keymap_binding_pressed(&binding, event);
    assert(last_pressed && strcmp(invoked.behavior_dev, "cmd") == 0);
    assert(invoked.param1 == 11 && invoked.param2 == 12);
    assert(invoked_event.source == 1 && invoked_event.timestamp == 123);
    mode = TOUCAN_OS_WIN;
    on_keymap_binding_released(&binding, event);
    assert(!last_pressed && strcmp(invoked.behavior_dev, "cmd") == 0);
    on_keymap_binding_pressed(&binding, event);
    assert(last_pressed && strcmp(invoked.behavior_dev, "ctrl") == 0);
    mode = TOUCAN_OS_MAC;
    on_keymap_binding_released(&binding, event);
    assert(!last_pressed && strcmp(invoked.behavior_dev, "ctrl") == 0);
    puts("PASS: OS routing preserves matching release across mode changes");
}
