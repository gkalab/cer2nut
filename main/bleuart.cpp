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
uint16_t BleUart::g_bleuart_attr_write_handle = 2;
uint16_t BleUart::g_bleuart_attr_upload_read_handle = 3;
uint16_t BleUart::g_bleuart_attr_upload_write_handle = 4;
uint16_t BleUart::g_bleuart_attr_unknown_read_handle = 5;
uint16_t BleUart::g_bleuart_attr_unknown_write1_handle = 6;
uint16_t BleUart::g_bleuart_attr_unknown_write2_handle = 7;
uint16_t BleUart::g_bleuart_attr_device_name_read_handle = 8;
uint16_t BleUart::g_bleuart_attr_appearance_read_handle = 9;
uint16_t BleUart::g_bleuart_attr_service_changed_indicate_handle = 10;
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

/* {1B7E8281-2877-41C3-B46E-CF057C562023} */
static const ble_uuid128_t gatt_svr_svc_upload_uuid =
    BLE_UUID128_INIT(0x23, 0x20, 0x56, 0x7c, 0x05, 0xcf, 0x6e, 0xb4, 0xc3, 0x41, 0x77, 0x28, 0x81, 0x82, 0x7e, 0x1b);

/* {1B7E8282-2877-41C3-B46E-CF057C562023} */
static const ble_uuid128_t gatt_svr_chr_upload_write_uuid =
    BLE_UUID128_INIT(0x23, 0x20, 0x56, 0x7c, 0x05, 0xcf, 0x6e, 0xb4, 0xc3, 0x41, 0x77, 0x28, 0x82, 0x82, 0x7e, 0x1b);

/* {1B7E8283-2877-41C3-B46E-CF057C562023} */
static const ble_uuid128_t gatt_svr_chr_upload_read_uuid =
    BLE_UUID128_INIT(0x23, 0x20, 0x56, 0x7c, 0x05, 0xcf, 0x6e, 0xb4, 0xc3, 0x41, 0x77, 0x28, 0x83, 0x82, 0x7e, 0x1b);

/* {9e5d1e47-5c13-43a0-8635-82ad38a1386f} */
static const ble_uuid128_t gatt_svr_svc_unknown_uuid =
    BLE_UUID128_INIT(0x6f, 0x38, 0xa1, 0x38, 0xad, 0x82, 0x35, 0x86, 0xa0, 0x43, 0x13, 0x5c, 0x47, 0x1e, 0x5d, 0x9e);

/* {e3dd50bf-f7a7-4e99-838e-570a086c666b} */
static const ble_uuid128_t gatt_svr_chr_unknown_indicate_notify_write_uuid =
    BLE_UUID128_INIT(0x6b, 0x66, 0x6c, 0x08, 0x0a, 0x57, 0x8e, 0x83, 0x99, 0x4e, 0xa7, 0xf7, 0xbf, 0x50, 0xdd, 0xe3);

/* {92e86c7a-d961-4091-b74f-2409e72efe36} */
static const ble_uuid128_t gatt_svr_chr_unknown_write_uuid =
    BLE_UUID128_INIT(0x36, 0xfe, 0x2e, 0xe7, 0x09, 0x24, 0x4f, 0xb7, 0x91, 0x40, 0x61, 0xd9, 0x7a, 0x6c, 0xe8, 0x92);

/* {347f7608-2e2d-47eb-913b-75d4edc4de3b} */
static const ble_uuid128_t gatt_svr_chr_unknown_read_uuid =
    BLE_UUID128_INIT(0x3b, 0xde, 0xc4, 0xed, 0xd4, 0x75, 0x3b, 0x91, 0xeb, 0x47, 0x2d, 0x2e, 0x08, 0x76, 0x7f, 0x34);

static const ble_uuid16_t gatt_svr_svc_generic_uuid = BLE_UUID16_INIT(0x1800);
static const ble_uuid16_t gatt_svr_chr_device_name_uuid = BLE_UUID16_INIT(0x2a00);
static const ble_uuid16_t gatt_svr_chr_appearance_uuid = BLE_UUID16_INIT(0x2a01);
static const ble_uuid16_t gatt_svr_svc_generic_attrib_uuid = BLE_UUID16_INIT(0x1801);
static const ble_uuid16_t gatt_svr_chr_service_changed_uuid = BLE_UUID16_INIT(0x2a05);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        // Mandatory service 0x1801
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_generic_attrib_uuid.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){{
                                            .uuid = &gatt_svr_chr_service_changed_uuid.u,
                                            .access_cb = BleUart::gatt_svr_chr_access_service_changed_indicate,
                                            .flags = BLE_GATT_CHR_F_INDICATE,
                                            .val_handle = &BleUart::g_bleuart_attr_service_changed_indicate_handle,
                                        },
                                        {
                                            .uuid = 0, // No more characteristics in this service
                                        }},
    },
    {
        // Mandatory service 0x1800
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_generic_uuid.u,
        .characteristics =
            (struct ble_gatt_chr_def[]){{
                                            .uuid = &gatt_svr_chr_device_name_uuid.u,
                                            .access_cb = BleUart::gatt_svr_chr_access_device_name_read,
                                            .flags = BLE_GATT_CHR_F_READ,
                                            .val_handle = &BleUart::g_bleuart_attr_device_name_read_handle,
                                        },
                                        {
                                            .uuid = &gatt_svr_chr_appearance_uuid.u,
                                            .access_cb = BleUart::gatt_svr_chr_access_appearance_read,
                                            .flags = BLE_GATT_CHR_F_READ,
                                            .val_handle = &BleUart::g_bleuart_attr_appearance_read_handle,
                                        },
                                        {
                                            .uuid = 0, // No more characteristics in this service
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
        /* Service: main */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_main_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]){{
                                                           /* Characteristic: Write */
                                                           .uuid = &gatt_svr_chr_main_write_uuid.u,
                                                           .access_cb = BleUart::gatt_svr_chr_access_uart_write,
                                                           .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP,
                                                           .val_handle = &BleUart::g_bleuart_attr_write_handle,
                                                       },
                                                       {
                                                           .uuid = &gatt_svr_chr_main_read_uuid.u,
                                                           .access_cb = BleUart::gatt_svr_chr_access_uart_write,
                                                           .flags = BLE_GATT_CHR_F_NOTIFY,
                                                           .val_handle = &BleUart::g_bleuart_attr_read_handle,
                                                       },
                                                       {
                                                           .uuid = 0, /* No more characteristics in this service */
                                                       }},
    },
    {
        // Service: upload
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_upload_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]){{
                                                           .uuid = &gatt_svr_chr_upload_write_uuid.u,
                                                           .access_cb = BleUart::gatt_svr_chr_access_uart_write,
                                                           .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP,
                                                           .val_handle = &BleUart::g_bleuart_attr_upload_write_handle,
                                                       },
                                                       {
                                                           .uuid = &gatt_svr_chr_upload_read_uuid.u,
                                                           .access_cb = BleUart::gatt_svr_chr_access_uart_write,
                                                           .flags = BLE_GATT_CHR_F_NOTIFY,
                                                           .val_handle = &BleUart::g_bleuart_attr_upload_read_handle,
                                                       },
                                                       {
                                                           .uuid = 0, // No more characteristics in this service
                                                       }},
    },
    {
        // Service: unknown
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_unknown_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]){{
                                                           .uuid = &gatt_svr_chr_unknown_indicate_notify_write_uuid.u,
                                                           .access_cb = BleUart::gatt_svr_chr_access_uart_write,
                                                           .flags = BLE_GATT_CHR_F_INDICATE | BLE_GATT_CHR_F_NOTIFY |
                                                                    BLE_GATT_CHR_F_WRITE,
                                                           .val_handle = &BleUart::g_bleuart_attr_unknown_write2_handle,
                                                       },
                                                       {
                                                           .uuid = &gatt_svr_chr_unknown_write_uuid.u,
                                                           .access_cb = BleUart::gatt_svr_chr_access_uart_write,
                                                           .flags = BLE_GATT_CHR_F_WRITE,
                                                           .val_handle = &BleUart::g_bleuart_attr_unknown_write1_handle,
                                                       },
                                                       {
                                                           .uuid = &gatt_svr_chr_unknown_read_uuid.u,
                                                           .access_cb = BleUart::gatt_svr_chr_access_uart_write,
                                                           .flags = BLE_GATT_CHR_F_READ,
                                                           .val_handle = &BleUart::g_bleuart_attr_unknown_read_handle,
                                                       },
                                                       {
                                                           .uuid = 0, // No more characteristics in this service
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
            // std::cout << "-->ble:" << toHex(data, data_len) << std::endl;
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
            // std::cout << "ble<--:" << toHex(ctxt->om->om_data, ctxt->om->om_len) << std::endl;
            chessnutAdapter.fromBle(ctxt->om->om_data, ctxt->om->om_len);
            om = SLIST_NEXT(om, om_next);
        }
        return 0;
    default:
        return 0;
    }
    return 0;
}

int BleUart::gatt_svr_chr_access_service_changed_indicate(uint16_t conn_handle, uint16_t attr_handle,
                                                          struct ble_gatt_access_ctxt* ctxt, void* arg) {
    return 0;
}

int BleUart::gatt_svr_chr_access_device_name_read(uint16_t conn_handle, uint16_t attr_handle,
                                                  struct ble_gatt_access_ctxt* ctxt, void* arg) {
    static std::string name("Chessnut Air");
    static std::vector<uint8_t> device_name(name.begin(), name.end());
    int rc = os_mbuf_append(ctxt->om, &device_name.front(), device_name.size());
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

int BleUart::gatt_svr_chr_access_appearance_read(uint16_t conn_handle, uint16_t attr_handle,
                                                 struct ble_gatt_access_ctxt* ctxt, void* arg) {
    static std::vector<uint8_t> appearance{0x00, 0x02};
    int rc = os_mbuf_append(ctxt->om, &appearance.front(), appearance.size());
    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
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

    // manufacturer data
    uint8_t mfg_data[16] = {0x50, 0x44, 0x43, 0x53, 0x0D, 0x54, 0x27, 0x64,
                            0x00, 0x00, 0x59, 0x95, 0x4f, 0x10, 0x1b, 0x00};
    fields.mfg_data = mfg_data;
    fields.mfg_data_len = 16;

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