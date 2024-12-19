#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <list>
#include <mutex>
#include <thread>
#include <vector>

#include "RgbLedCommandTranslator.h"

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

    void setProcessingTime(int processingTimeMillis);

    void ledsDetected(bool hasRgbLeds);

    /** Set brightness of RGB LEDs */
    void setBrightness(int brightnessValue);

  private:
    static std::vector<uint8_t> const LEDS_OFF;
    void processCommands();

    ToUsbFunction toUsb;
    std::list<std::vector<uint8_t>> pendingCommands;
    std::atomic_int processingTimeMs;
    uint64_t lastCommandTime = 0;
    std::vector<uint8_t> lastCommand;
    std::atomic_bool keepRunning;
    std::atomic_bool ledsInitiallyDetected;
    std::atomic_bool hasRgbLeds;
    std::mutex commandMutex;
    std::thread processingThread;
    RgbLedCommandTranslator ledCommandTranslator;
};

} // namespace eboard