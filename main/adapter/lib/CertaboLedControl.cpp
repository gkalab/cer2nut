#include "CertaboLedControl.h"

using eboard::CertaboLedControl;

std::vector<uint8_t> const CertaboLedControl::LEDS_OFF{0, 0, 0, 0, 0, 0, 0, 0};

CertaboLedControl::CertaboLedControl(ToUsbFunction toUsb)
    : toUsb(std::move(toUsb)), processingTimeMs(600), keepRunning(true), ledsInitiallyDetected(false),
      hasRgbLeds(false) {
    processCommands();
}

CertaboLedControl::~CertaboLedControl() {
    keepRunning = false;
    if (processingThread.joinable()) {
        processingThread.join();
    }
}

void CertaboLedControl::ledCommand(std::vector<uint8_t> const& command) {
    std::lock_guard<std::mutex> guard(commandMutex);
    pendingCommands.push_back(command);
}

void CertaboLedControl::setProcessingTime(int processingTimeMillis) {
    processingTimeMs = processingTimeMillis;
}

void CertaboLedControl::processCommands() {
    processingThread = std::thread([this]() {
        while (keepRunning) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::lock_guard<std::mutex> guard(commandMutex);
            while (pendingCommands.size() > 2) {
                pendingCommands.pop_front();
            }
            if (pendingCommands.size() == 2 && pendingCommands.back() != LEDS_OFF) {
                pendingCommands.pop_front();
            }
            uint64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                                       std::chrono::steady_clock::now().time_since_epoch())
                                       .count();
            if ((lastCommandTime + processingTimeMs) <= currentTime && !pendingCommands.empty()) {
                auto cmd = pendingCommands.front();
                if (cmd != lastCommand) {
                    if (ledsInitiallyDetected) {
                        if (hasRgbLeds) {
                            auto translatedCommand = ledCommandTranslator.translate(cmd);
                            toUsb(&translatedCommand.front(), translatedCommand.size());
                        } else {
                            toUsb(&cmd.front(), cmd.size());
                        }
                    } else {
                        std::this_thread::sleep_for(std::chrono::milliseconds(200));
                        toUsb(&cmd.front(), cmd.size());
                        std::this_thread::sleep_for(std::chrono::milliseconds(400));
                        if ((!ledsInitiallyDetected && !hasRgbLeds) || hasRgbLeds) {
                            auto translatedCommand = ledCommandTranslator.translate(cmd);
                            toUsb(&translatedCommand.front(), translatedCommand.size());
                        }
                    }
                    lastCommand = cmd;
                    lastCommandTime = currentTime;
                }
                pendingCommands.pop_front();
            }
        }
    });
}

void CertaboLedControl::ledsDetected(bool rgbLeds) {
    hasRgbLeds = rgbLeds;
    if (!ledsInitiallyDetected && hasRgbLeds) {
        processingTimeMs = 200;
    }
    ledsInitiallyDetected = true;
}

void CertaboLedControl::setBrightness(int brightnessValue) {
    ledCommandTranslator.setBrightness(brightnessValue);
}