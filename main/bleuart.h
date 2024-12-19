#include <cstdint>

#include "adapter/lib/ChessnutAdapter.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"

namespace ble {
class BleUart {
  public:
    static uint16_t g_bleuart_attr_read_handle;
    static uint16_t g_bleuart_attr_board_read_handle;
    static uint16_t g_bleuart_attr_write_handle;
    static uint16_t g_bleuart_attr_upload_read_handle;
    static uint16_t g_bleuart_attr_upload_write_handle;
    static uint16_t g_bleuart_attr_unknown_read_handle;
    static uint16_t g_bleuart_attr_unknown_write1_handle;
    static uint16_t g_bleuart_attr_unknown_write2_handle;
    static uint16_t g_bleuart_attr_device_name_read_handle;
    static uint16_t g_bleuart_attr_appearance_read_handle;
    static uint16_t g_bleuart_attr_service_changed_indicate_handle;
    static uint16_t g_console_conn_handle;
    static bool connected;

    BleUart();
    ~BleUart() = default;

    // Write data to ESP32 defined as server
    static int gatt_svr_chr_access_uart_write(uint16_t conn_handle, uint16_t attr_handle,
                                              struct ble_gatt_access_ctxt* ctxt, void* arg);

    static int gatt_svr_chr_access_service_changed_indicate(uint16_t conn_handle, uint16_t attr_handle,
                                                            struct ble_gatt_access_ctxt* ctxt, void* arg);

    static int gatt_svr_chr_access_device_name_read(uint16_t conn_handle, uint16_t attr_handle,
                                                    struct ble_gatt_access_ctxt* ctxt, void* arg);

    static int gatt_svr_chr_access_appearance_read(uint16_t conn_handle, uint16_t attr_handle,
                                                   struct ble_gatt_access_ctxt* ctxt, void* arg);

    /**
     * Enables advertising with the following parameters:
     *     o General discoverable mode.
     *     o Undirected connectable mode.
     */
    static void bleuart_advertise(void);

    void init();
    void notify(const uint8_t* data, size_t data_len);
    bool isConnected();

  private:
    static eboard::ChessnutAdapter chessnutAdapter;

    // The infinite task
    static void host_task(void* param);

    /**
     * bleuart GATT server initialization
     *
     * @param eventq
     * @return 0 on success; non-zero on failure
     */
    int bleuart_gatt_svr_init(void);

    /**
     * Sets the global connection handle
     *
     * @param connection handle
     */
    void bleuart_set_conn_handle(uint16_t conn_handle);

    /** BLE event handling */
    static int bleuart_gap_event(struct ble_gap_event* event, void* arg);

    void ble_task(void* arg);
};
} // namespace ble
