#include "CertaboLedControl.h"

using eboard::CertaboLedControl;

std::vector<uint8_t> CertaboLedControl::LEDS_OFF{0, 0, 0, 0, 0, 0, 0, 0};

CertaboLedControl::CertaboLedControl(ToUsbFunction toUsb) : toUsb(std::move(toUsb)), keepRunning(true) {
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
            if ((lastCommandTime + 600) <= currentTime && !pendingCommands.empty()) {
                auto cmd = pendingCommands.front();
                pendingCommands.pop_front();
                if (cmd != lastCommand) {
                    toUsb(&cmd.front(), cmd.size());
                    lastCommand = cmd;
                    lastCommandTime = currentTime;
                }
            }
        }
    });
}