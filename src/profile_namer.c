#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <string.h>
#include <stdio.h>

#include <zmk/event_manager.h>
#include <zmk/ble.h>
#include <zmk/events/ble_active_profile_changed.h>

#if !defined(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

static void set_name_for_profile(uint8_t index_zero_based) {
    char new_name[CONFIG_BT_DEVICE_NAME_MAX];
    snprintf(new_name, sizeof(new_name), "keyboard_%u", (unsigned)(index_zero_based + 1));

    bt_set_name(new_name);

    static const struct bt_data ad[] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    };
    struct bt_data sd[] = {
        BT_DATA(BT_DATA_NAME_COMPLETE, new_name, strlen(new_name)),
    };
    bt_le_adv_update_data(ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
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
