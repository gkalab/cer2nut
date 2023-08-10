#include <mutex>

#include "cp210x_usb.hpp"

class Usb {
  public:
    static esp_usb::CP210x* vcp;
    static std::mutex vcp_mutex;
};