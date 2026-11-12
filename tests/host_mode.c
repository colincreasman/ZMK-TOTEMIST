#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CONFIG_ZMK_SPLIT 1
#define CONFIG_ZMK_SPLIT_ROLE_CENTRAL 1
#define CONFIG_SETTINGS 1
#define CONFIG_TOUCAN_OS_MODE_DEFAULT_WIN 0
#define CONFIG_ZMK_SETTINGS_SAVE_DEBOUNCE 1000
#define IS_ENABLED(x) (x)
#define ARG_UNUSED(x) (void)(x)
#define LOG_MODULE_DECLARE(...)
#define LOG_INF(...)
#define LOG_ERR(...)
#define SETTINGS_STATIC_HANDLER_DEFINE(...)
#define SYS_INIT(...)
#define K_MSEC(x) (x)
#define ZMK_TRANSPORT_BLE 1

struct k_work { int unused; };
struct k_work_delayable { void (*handler)(struct k_work *); int delay; };
static void k_work_init_delayable(struct k_work_delayable *work,
                                  void (*handler)(struct k_work *)) {
    work->handler = handler;
}
static int k_work_reschedule(struct k_work_delayable *work, int delay) {
    work->delay = delay;
    return 0;
}
struct zmk_endpoint_instance { int transport; struct { int profile_index; } ble; };
static struct zmk_endpoint_instance endpoint;
static struct zmk_endpoint_instance zmk_endpoints_selected(void) { return endpoint; }
typedef int (*settings_read_cb)(void *, void *, size_t);
static uint8_t persisted[16];
static unsigned writes;
static int settings_save_one(const char *key, const void *value, size_t size) {
    assert(strcmp(key, "toucan/os_modes") == 0);
    assert(size == sizeof(persisted));
    memcpy(persisted, value, size);
    writes++;
    return 0;
}
static bool settings_name_steq(const char *name, const char *key, const char **next) {
    (void)next;
    return strcmp(name, key) == 0;
}
static int read_saved(void *arg, void *value, size_t size) {
    (void)arg;
    memcpy(value, persisted, size);
    return (int)size;
}

#include "../src/toucan_os_mode.c"

int main(void) {
    assert(!toucan_os_mode_settings_loaded());
    toucan_os_mode_init();
    assert(toucan_os_mode_get() == TOUCAN_OS_MAC);
    endpoint = (struct zmk_endpoint_instance){.transport = ZMK_TRANSPORT_BLE,
                                            .ble.profile_index = 2};
    assert(toucan_os_mode_host_slot() == 3);
    toucan_os_mode_set(TOUCAN_OS_WIN);
    assert(toucan_os_mode_get() == TOUCAN_OS_WIN);
    assert(os_mode_save_work.delay == CONFIG_ZMK_SETTINGS_SAVE_DEBOUNCE);
    assert(writes == 0);
    endpoint.ble.profile_index = 1;
    assert(toucan_os_mode_get() == TOUCAN_OS_MAC);
    toucan_os_mode_toggle();
    assert(toucan_os_mode_get() == TOUCAN_OS_WIN);
    endpoint.transport = 0;
    assert(toucan_os_mode_host_slot() == 0);
    assert(toucan_os_mode_get() == TOUCAN_OS_MAC);
    os_mode_save_work.handler(NULL);
    assert(writes == 1 && persisted[0] == TOUCAN_OS_MAC &&
           persisted[2] == TOUCAN_OS_WIN && persisted[3] == TOUCAN_OS_WIN);
    memset(modes, TOUCAN_OS_MAC, sizeof(modes));
    assert(toucan_os_mode_handle_set("os_modes", sizeof(persisted), read_saved, NULL) == 0);
    toucan_os_mode_settings_commit();
    assert(toucan_os_mode_settings_loaded());
    endpoint.transport = ZMK_TRANSPORT_BLE;
    endpoint.ble.profile_index = 2;
    assert(toucan_os_mode_get() == TOUCAN_OS_WIN);
    endpoint.ble.profile_index = 0;
    assert(toucan_os_mode_get() == TOUCAN_OS_MAC);
    assert(toucan_os_mode_handle_set("os_modes", 1, read_saved, NULL) == -EINVAL);
    puts("PASS: per-host Mac/Windows modes, debounced persistence and reload");
}
