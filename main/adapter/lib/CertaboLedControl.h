#pragma once

#include <atomic>
#include <chrono>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

namespace eboard {

/** Callback function for sending data via USB. */
using ToUsbFunction = std::function<void(uint8_t* data, size_t data_len)>;

/**
 * CertaboLedControl ensures the Certabo board does not get flooded with LED commands.
 */
class CertaboLedControl {
  public:
    explicit CertaboLedControl(ToUsbFunction toUsb);

    ~CertaboLedControl();

    void ledCommand(std::vector<uint8_t> const& command);

  private:
    static std::vector<uint8_t> LEDS_OFF;
    void processCommands();

    ToUsbFunction toUsb;
    std::list<std::vector<uint8_t>> pendingCommands;
    uint64_t lastCommandTime = 0;
    std::vector<uint8_t> lastCommand;
    std::atomic_bool keepRunning;
    std::mutex commandMutex;
    std::thread processingThread;
};

} // namespace eboard