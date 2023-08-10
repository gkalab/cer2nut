#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include "esp_event.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

/* BLE */
#include "esp_nimble_hci.h"
#include "host/ble_att.h"
#include "host/ble_hs.h"
#include "host/ble_hs_adv.h"
#include "host/ble_l2cap.h"
#include "host/ble_sm.h"
#include "host/ble_uuid.h"
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

/* Mandatory services. */
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "bleuart.h"
#include "vcpusb.h"

using ble::BleUart;

uint16_t BleUart::g_bleuart_attr_read_handle = 0;
uint16_t BleUart::g_bleuart_attr_board_read_handle = 1;
uint16_t BleUart::g_bleuart_attr_write_handle = 0;
uint16_t BleUart::g_console_conn_handle = 0;
bool BleUart::connected = false;

/* {1B7E8271-2877-41C3-B46E-CF057C562023} */
static const ble_uuid128_t gatt_svr_svc_main_uuid =
    BLE_UUID128_INIT(0x23, 0x20, 0x56, 0x7c, 0x05, 0xcf, 0x6e, 0xb4, 0xc3, 0x41, 0x77, 0x28, 0x71, 0x82, 0x7e, 0x1b);

/* {1B7E8272-2877-41C3-B46E-CF057C562023} */
static const ble_uuid128_t gatt_svr_chr_main_write_uuid =
    BLE_UUID128_INIT(0x23, 0x20, 0x56, 0x7c, 0x05, 0xcf, 0x6e, 0xb4, 0xc3, 0x41, 0x77, 0x28, 0x72, 0x82, 0x7e, 0x1b);

/* {1B7E8273-2877-41C3-B46E-CF057C562023} */
static const ble_uuid128_t gatt_svr_chr_main_read_uuid =
    BLE_UUID128_INIT(0x23, 0x20, 0x56, 0x7c, 0x05, 0xcf, 0x6e, 0xb4, 0xc3, 0x41, 0x77, 0x28, 0x73, 0x82, 0x7e, 0x1b);

/* {1B7E8261-2877-41C3-B46E-CF057C562023} */
static const ble_uuid128_t gatt_svr_svc_board_uuid =
    BLE_UUID128_INIT(0x23, 0x20, 0x56, 0x7c, 0x05, 0xcf, 0x6e, 0xb4, 0xc3, 0x41, 0x77, 0x28, 0x61, 0x82, 0x7e, 0x1b);

/* {1B7E8262-2877-41C3-B46E-CF057C562023} */
static const ble_uuid128_t gatt_svr_chr_board_read_uuid =
    BLE_UUID128_INIT(0x23, 0x20, 0x56, 0x7c, 0x05, 0xcf, 0x6e, 0xb4, 0xc3, 0x41, 0x77, 0x28, 0x62, 0x82, 0x7e, 0x1b);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /* Service: main */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_main_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]){{
                                                           .uuid = &gatt_svr_chr_main_read_uuid.u,
                                                           .access_cb = BleUart::gatt_svr_chr_access_uart_write,
                                                           .flags = BLE_GATT_CHR_F_NOTIFY,
                                                           .val_handle = &BleUart::g_bleuart_attr_read_handle,
                                                       },
                                                       {
                                                           /* Characteristic: Write */
                                                           .uuid = &gatt_svr_chr_main_write_uuid.u,
                                                           .access_cb = BleUart::gatt_svr_chr_access_uart_write,
                                                           .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP,
                                                           .val_handle = &BleUart::g_bleuart_attr_write_handle,
                                                       },
                                                       {
                                                           .uuid = 0, /* No more characteristics in this service */
                                                       }},
    },
    {
        /* Service: board */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_board_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]){{
                                                           .uuid = &gatt_svr_chr_board_read_uuid.u,
                                                           .access_cb = BleUart::gatt_svr_chr_access_uart_write,
                                                           .flags = BLE_GATT_CHR_F_NOTIFY,
                                                           .val_handle = &BleUart::g_bleuart_attr_board_read_handle,
                                                       },
                                                       {
                                                           .uuid = 0, /* No more characteristics in this service */
                                                       }},
    },
    {
        0, /* No more services */
    },
};
#pragma GCC diagnostic pop

static std::string toHex(unsigned const char* data, int len) {
    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < len; ++i) {
        ss << std::setw(2) << std::setfill('0') << (int)data[i];
    }
    return ss.str();
}

static void toUsb(uint8_t* data, size_t data_len) {
    if (data_len > 0 && Usb::vcp != nullptr) { // TODO check for nullptr should not be necessary here, crashes without
        // std::cout << "-->usb:" << toHex(data, data_len) << std::endl;
        std::lock_guard<std::mutex> guard(Usb::vcp_mutex);
        if (Usb::vcp != nullptr) {
            Usb::vcp->tx_blocking(data, data_len, 600);
        }
    }
}

eboard::ChessnutAdapter BleUart::chessnutAdapter(eboard::ChessnutAdapter(
    [](uint8_t* data, size_t data_len) { toUsb(data, data_len); },
    [](uint8_t* data, size_t data_len, bool isBoardData) {
        if (connected) {
            // std::cout << "-->ble:" << toHex(data, data_len) <<
            // std::endl;
            struct os_mbuf* om;
            om = ble_hs_mbuf_from_flat(data, data_len);
            if (!om) {
                return;
            }
            if (isBoardData) {
                ble_gatts_notify_custom(g_console_conn_handle, g_bleuart_attr_board_read_handle, om);
            } else {
                ble_gatts_notify_custom(g_console_conn_handle, g_bleuart_attr_read_handle, om);
            }
        }
    }));

BleUart::BleUart() {}

int BleUart::bleuart_gap_event(struct ble_gap_event* event, void* arg) {
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI("GAP", "BLE GAP event connect %s", event->connect.status == 0 ? "OK" : "FAILED");
        /* A new connection was established or a connection attempt failed. */
        if (event->connect.status == 0) {
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            g_console_conn_handle = event->connect.conn_handle;
            connected = true;
            if (chessnutAdapter.isCalibrated()) {
                chessnutAdapter.ledCommand({0, 0, 0, 0, 0, 0, 0, 0});
            }
        } else {
            /* Connection failed; resume advertising. */
            connected = false;
            bleuart_advertise();
            if (chessnutAdapter.isCalibrated()) {
                chessnutAdapter.ledCommand({0, 0, 0, 0x18, 0x18, 0, 0, 0});
            }
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI("GAP", "Connection terminated; resume advertising");
        /* Connection terminated; resume advertising. */
        connected = false;
        bleuart_advertise();
        if (chessnutAdapter.isCalibrated()) {
            chessnutAdapter.ledCommand({0, 0, 0, 0x18, 0x18, 0, 0, 0});
        }
        return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI("GAP", "Advertising terminated; resume advertising");
        /* Advertising terminated; resume advertising. */
        bleuart_advertise();
        return 0;

    case BLE_GAP_EVENT_REPEAT_PAIRING:
        /* We already have a bond with the peer, but it is attempting to
         * establish a new secure link.  This app sacrifices security for
         * convenience: just throw away the old bond and accept the new link.
         */

        /* Delete the old bond. */
        rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
        assert(rc == 0);
        ble_store_util_delete_peer(&desc.peer_id_addr);

        /* Return BLE_GAP_REPEAT_PAIRING_RETRY to indicate that the host should
         * continue with the pairing operation.
         */
        return BLE_GAP_REPEAT_PAIRING_RETRY;
    }

    return 0;
}

void BleUart::notify(uint8_t* data, size_t data_len) {
    // std::cout << "usb<--:" << toHex(data, data_len) << std::endl;
    chessnutAdapter.fromUsb(data, data_len);
}

int BleUart::gatt_svr_chr_access_uart_write(uint16_t conn_handle, uint16_t attr_handle,
                                            struct ble_gatt_access_ctxt* ctxt, void* arg) {
    struct os_mbuf* om = ctxt->om;
    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        while (om) {
            // std::cout << "ble<--:" << toHex(ctxt->om->om_data, ctxt->om->om_len);
            chessnutAdapter.fromBle(ctxt->om->om_data, ctxt->om->om_len);
            om = SLIST_NEXT(om, om_next);
        }
        std::cout << std::endl;
        return 0;
    default:
        assert(0);
        return BLE_ATT_ERR_UNLIKELY;
    }
    return 0;
}

int BleUart::bleuart_gatt_svr_init(void) {
    int rc;

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        goto err;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

err:
    return rc;
}

void BleUart::bleuart_set_conn_handle(uint16_t conn_handle) { g_console_conn_handle = conn_handle; }

void BleUart::bleuart_advertise(void) {
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    int rc;

    /*
     *  Set the advertisement data included in our advertisements:
     *     o Flags (indicates advertisement type and other general info).
     *     o Advertising tx power.
     *     o 128 bit UUID
     */

    memset(&fields, 0, sizeof fields);

    /* Advertise two flags:
     *     o Discoverability in forthcoming advertisement (general)
     *     o BLE-only (BR/EDR unsupported).
     */
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    /* Indicate that the TX power level field should be included; have the
     * stack fill this value automatically.  This is done by assiging the
     * special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
     */
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        return;
    }

    memset(&fields, 0, sizeof fields);
    fields.name = (uint8_t*)ble_svc_gap_device_name();
    fields.name_len = strlen((char*)fields.name);
    fields.name_is_complete = 1;

    rc = ble_gap_adv_rsp_set_fields(&fields);
    if (rc != 0) {
        return;
    }

    /* Begin advertising. */
    memset(&adv_params, 0, sizeof adv_params);
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, nullptr, BLE_HS_FOREVER, &adv_params, bleuart_gap_event, nullptr);
    if (rc != 0) {
        return;
    }
}

// The infinite task
void BleUart::host_task(void* param) {
    nimble_port_run(); // This function will return only when nimble_port_stop() is executed
}

void BleUart::ble_task(void* arg) {
    nimble_port_freertos_init(host_task); // Run the thread
}

void BleUart::init() {
    nvs_flash_init();
    nimble_port_init();
    /* Initialize the BLE host. */
    ble_hs_cfg.sync_cb = BleUart::bleuart_advertise;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;
    assert(bleuart_gatt_svr_init() == 0);

    /* Set the default device name. */
    assert(ble_svc_gap_device_name_set("Chessnut Air") == 0);

    nimble_port_freertos_init(BleUart::host_task); // Run the thread
}

bool BleUart::isConnected() { return BleUart::connected; }