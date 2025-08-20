#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <string.h>
#include <stdio.h>

#include <zmk/event_manager.h>
#include <zmk/ble.h>
#include <zmk/events/ble_active_profile_changed.h>
/* Fallback if CONFIG_BT_DEVICE_NAME_MAX isn't defined in this build */
#ifndef CONFIG_BT_DEVICE_NAME_MAX
#define PROFILE_NAMER_NAME_MAX 31  /* safe default; adjust if you want longer names */
#else
#define PROFILE_NAMER_NAME_MAX CONFIG_BT_DEVICE_NAME_MAX
#endif

#if !defined(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

static void set_name_for_profile(uint8_t index_zero_based) {
    char new_name[PROFILE_NAMER_NAME_MAX];
    snprintf(new_name, sizeof(new_name), "keyboard_%u", (unsigned)(index_zero_based + 1));

    /* 1) Update GAP device name */
    bt_set_name(new_name);

    /* 2) Refresh advertising/scan-response so scanners see the new name */
    /* NOTE: do NOT make these 'static' inside a function, or you'll hit
       "initializer element is not constant" with BT_DATA_BYTES. */
    const struct bt_data ad[] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    };
    struct bt_data sd[] = {
        BT_DATA(BT_DATA_NAME_COMPLETE, new_name, strlen(new_name)),
    };

    int err = bt_le_adv_update_data(ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    if (err) {
        /* Optional: log or handle; you can include <zephyr/logging/log.h> if desired */
        // LOG_WRN("bt_le_adv_update_data failed: %d", err);
    }
}


static int profile_change_listener(const zmk_event_t *eh) {
    const struct zmk_ble_active_profile_changed *evt =
        as_zmk_ble_active_profile_changed(eh);
    if (!evt) return ZMK_EV_EVENT_BUBBLE;

    set_name_for_profile(evt->index);
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(profile_namer, profile_change_listener);
ZMK_SUBSCRIPTION(profile_namer, zmk_ble_active_profile_changed);

#endif
