#include <thread>

#include "esp_event.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "usb/usb_host.h"

#include "vcpusb.h"

#include "bleuart.h"

using ble::BleUart;

using namespace esp_usb;

#define BAUDRATE (38400)
#define STOP_BITS (0) // 0: 1 stopbit, 1: 1.5 stopbits, 2: 2 stopbits
#define PARITY (0)    // 0: None, 1: Odd, 2: Even, 3: Mark, 4: Space
#define DATA_BITS (8)

static const char* TAG = "cer2nut";

static SemaphoreHandle_t device_disconnected_sem;

static void handle_rx(uint8_t* data, size_t data_len, void* arg) {
    BleUart* ble = (BleUart*)arg;
    ble->notify(data, data_len);
}

static void handle_event(const cdc_acm_host_dev_event_data_t* event, void* user_ctx) {
    switch (event->type) {
    case CDC_ACM_HOST_ERROR:
        ESP_LOGE(TAG, "CDC-ACM error has occurred, err_no = %d", event->data.error);
        break;
    case CDC_ACM_HOST_DEVICE_DISCONNECTED:
        ESP_LOGI(TAG, "Device suddenly disconnected");
        xSemaphoreGive(device_disconnected_sem);
        break;
    case CDC_ACM_HOST_SERIAL_STATE:
        ESP_LOGI(TAG, "serial state notif 0x%04X", event->data.serial_state.val);
        break;
    case CDC_ACM_HOST_NETWORK_CONNECTION:
    default:
        break;
    }
}

void usb_lib_task(void* arg) {
    while (1) {
        // Start handling system events
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            ESP_ERROR_CHECK(usb_host_device_free_all());
        }
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            ESP_LOGI(TAG, "USB: All devices freed");
            // Continue handling USB events to allow device reconnection
        }
    }
}

extern "C" void app_main(void) {
    device_disconnected_sem = xSemaphoreCreateBinary();
    assert(device_disconnected_sem);

    // Install USB Host driver.
    ESP_LOGI(TAG, "Installing USB Host");
    const usb_host_config_t host_config = {
        .skip_phy_setup = false,
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };
    ESP_ERROR_CHECK(usb_host_install(&host_config));

    // Create a task that will handle USB library events
    xTaskCreate(usb_lib_task, "usb_lib", 4096, NULL, 10, NULL);

    ESP_LOGI(TAG, "Installing CDC-ACM driver");
    ESP_ERROR_CHECK(cdc_acm_host_install(NULL));

    BleUart ble;
    ble.init();

    while (true) {
        const cdc_acm_host_device_config_t dev_config = {
            .connection_timeout_ms = 10000,
            .out_buffer_size = 64,
            .event_cb = handle_event,
            .data_cb = handle_rx,
            .user_arg = &ble,
        };

        try {
            ESP_LOGI(TAG, "Opening CP210X device");
            {
                std::lock_guard<std::mutex> guard(Usb::vcp_mutex);
                Usb::vcp = CP210x::open_cp210x(CP210X_PID, &dev_config);
            }
        } catch (esp_err_t err) {
            ESP_LOGE(TAG, "The required device was not opened.\nExiting...");
            return;
        }

        ESP_LOGI(TAG, "Setting up line coding");
        cdc_acm_line_coding_t line_coding = {
            .dwDTERate = BAUDRATE,
            .bCharFormat = STOP_BITS,
            .bParityType = PARITY,
            .bDataBits = DATA_BITS,
        };
        ESP_ERROR_CHECK(Usb::vcp->line_coding_set(&line_coding));

        // We are done. Wait for device disconnection and start over
        xSemaphoreTake(device_disconnected_sem, portMAX_DELAY);
        {
            std::lock_guard<std::mutex> guard(Usb::vcp_mutex);
            delete Usb::vcp;
        }
    }
}
