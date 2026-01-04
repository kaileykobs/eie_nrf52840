/*
 * main.c
 */

#include <inttypes.h>

#include "BTN.h"
#include "LED.h"
#include <stdio.h>
#include <string.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/printk.h>

#define SLEEP_MS 1

/* UUID raw values */
#define BT_UUID_EIE_SERVICE_VAL \
    BT_UUID_128_ENCODE(0x12345678, 0x1234, 0x5678, 0x1234, 0x56789abcdef0)

#define BT_UUID_EIE_CHAR_VAL \
    BT_UUID_128_ENCODE(0xabcdef01, 0x2345, 0x6789, 0xabcd, 0xef0123456789)

/* UUID objects */
static const struct bt_uuid_128 eie_service_uuid =
    BT_UUID_INIT_128(BT_UUID_EIE_SERVICE_VAL);

static const struct bt_uuid_128 eie_char_uuid =
    BT_UUID_INIT_128(BT_UUID_EIE_CHAR_VAL);

/* BLE advertising flags */
#define BLE_ADV_FLAGS (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)

/* Advertising data */
static const struct bt_data ble_advertising_data[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BLE_ADV_FLAGS),
    BT_DATA(BT_DATA_NAME_COMPLETE,
            CONFIG_BT_DEVICE_NAME,
            sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

/* Characteristic data */
#define BLE_CUSTOM_CHARACTERISTIC_MAX_DATA_LENGTH 20
static uint8_t ble_custom_characteristic_user_data[
    BLE_CUSTOM_CHARACTERISTIC_MAX_DATA_LENGTH] = {0};

/* Read callback */
static ssize_t ble_custom_characteristic_read_cb(
    struct bt_conn *conn,
    const struct bt_gatt_attr *attr,
    void *buf,
    uint16_t len,
    uint16_t offset)
{
    const char *value = attr->user_data;
    return bt_gatt_attr_read(conn, attr, buf, len, offset,
                             value, strlen(value));
}

/* Write callback */
static ssize_t ble_custom_characteristic_write_cb(
    struct bt_conn *conn,
    const struct bt_gatt_attr *attr,
    const void *buf,
    uint16_t len,
    uint16_t offset,
    uint8_t flags)
{
    uint8_t *value_ptr = attr->user_data;

    if (offset + len >= BLE_CUSTOM_CHARACTERISTIC_MAX_DATA_LENGTH) {
        return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
    }

    memcpy(value_ptr + offset, buf, len);
    value_ptr[offset + len] = 0;

    return len;
}

/* GATT service */
BT_GATT_SERVICE_DEFINE(
    eie_service,
    BT_GATT_PRIMARY_SERVICE(&eie_service_uuid),
    BT_GATT_CHARACTERISTIC(
        &eie_char_uuid.uuid,
        BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
        BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
        ble_custom_characteristic_read_cb,
        ble_custom_characteristic_write_cb,
        ble_custom_characteristic_user_data),
);

int main(void)
{
    int err;

    if (BTN_init() < 0) {
        return 0;
    }

    if (LED_init() < 0) {
        return 0;
    }

    /* Enable Bluetooth */
    err = bt_enable(NULL);
    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return 0;
    }

    printk("Bluetooth initialized\n");

    /* Start advertising */
    err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_1,
                          ble_advertising_data,
                          ARRAY_SIZE(ble_advertising_data),
                          NULL, 0);
    if (err) {
        printk("Advertising failed to start (err %d)\n", err);
        return 0;
    }

    printk("Advertising started\n");

    while (1) {
        k_msleep(SLEEP_MS);
    }
}


