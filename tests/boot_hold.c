#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define LOG_MODULE_DECLARE(...)
#define DT_HAS_COMPAT_STATUS_OKAY(...) 1
#define DT_INST_FOREACH_STATUS_OKAY(...)
#define ZMK_BEHAVIOR_OPAQUE 0
#define CONFIG_ZMK_SPLIT 1
#define IS_ENABLED(x) (x)
#define ZMK_POSITION_STATE_CHANGE_SOURCE_LOCAL 255
#define K_MSEC(x) (x)
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define CONTAINER_OF(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))

struct k_work { int unused; };
struct k_work_delayable { struct k_work work; void (*handler)(struct k_work *); int delay; };
static int64_t now;
static int64_t k_uptime_get(void) { return now; }
static struct k_work_delayable *k_work_delayable_from_work(struct k_work *work) {
    return CONTAINER_OF(work, struct k_work_delayable, work);
}
static void k_work_init_delayable(struct k_work_delayable *work,
                                  void (*handler)(struct k_work *)) {
    work->handler = handler;
}
static int k_work_reschedule(struct k_work_delayable *work, int delay) {
    work->delay = delay;
    return 0;
}
static int k_work_cancel_delayable(struct k_work_delayable *work) {
    work->delay = -1;
    return 0;
}
struct device { const void *config; void *data; };
struct zmk_behavior_binding { const char *behavior_dev; uint32_t param1, param2; };
struct zmk_behavior_binding_event { uint32_t position; int64_t timestamp; uint8_t source; };
struct behavior_driver_api {
    int (*binding_pressed)(struct zmk_behavior_binding *, struct zmk_behavior_binding_event);
    int (*binding_released)(struct zmk_behavior_binding *, struct zmk_behavior_binding_event);
};
static struct device devices[2];
static const struct device *zmk_behavior_get_binding(const char *name) {
    return &devices[name[0] == 'R'];
}
static unsigned calls;
static uint8_t boot_source;
static int zmk_behavior_invoke_binding(const struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event, bool pressed) {
    (void)binding;
    assert(pressed);
    calls++;
    boot_source = event.source;
    return 0;
}
#include "../src/behavior_long_press.c"

int main(void) {
    struct behavior_long_press_config config = {.duration_ms = 2000, .behavior = {"bootloader", 0, 0}};
    struct behavior_long_press_data data[2];
    struct zmk_behavior_binding left = {.behavior_dev = "L"}, right = {.behavior_dev = "R"};
    devices[0] = (struct device){&config, &data[0]};
    devices[1] = (struct device){&config, &data[1]};
    behavior_long_press_init(&devices[0]);
    behavior_long_press_init(&devices[1]);
    struct zmk_behavior_binding_event event = {.timestamp = 100, .source = 255};
    now = 150;
    on_keymap_binding_pressed(&left, event);
    assert(data[0].work.delay == 1950 && calls == 0);
    on_keymap_binding_released(&left, event);
    data[0].work.handler(&data[0].work.work);
    assert(calls == 0);
    on_keymap_binding_pressed(&left, event);
    event.source = 0;
    on_keymap_binding_pressed(&right, event);
    on_keymap_binding_released(&left, event);
    data[1].work.handler(&data[1].work.work);
    assert(calls == 1 && boot_source == 0);
    data[1].work.handler(&data[1].work.work);
    assert(calls == 1);
    event.source = 255;
    on_keymap_binding_pressed(&left, event);
    data[0].work.handler(&data[0].work.work);
    assert(calls == 2 && boot_source == 255);
    puts("PASS: short holds cancel; independent long holds retain each half's source");
}
