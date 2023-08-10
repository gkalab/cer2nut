#include "vcpusb.h"

esp_usb::CP210x* Usb::vcp;
std::mutex Usb::vcp_mutex;