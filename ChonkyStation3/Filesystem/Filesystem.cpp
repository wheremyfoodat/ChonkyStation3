#include "Filesystem.hpp"


void Filesystem::mount(Filesystem::Device device, fs::path path) {
    if (!fs::exists(path)) {
        Helpers::panic("Mount path %s for device %s does not exist\n", path.generic_string().c_str(), deviceToString(device));
    }

    mounted_devices[device] = path;
    log("Mounted device %s at %s\n", deviceToString(device).c_str(), path.generic_string().c_str());
}

bool Filesystem::isDeviceMounted(Filesystem::Device device) {
    return mounted_devices.contains(device);
}

fs::path Filesystem::guestPathToHost(fs::path path) {
    const std::string path_str = path.generic_string();
    fs::path guest_path;

    // Check if the path is valid
    // TODO: I assume relative paths can exist too...?
    if (path.begin()->generic_string() != "/")
        Helpers::panic("Path %s is not valid\n", path.generic_string().c_str());

    // Check if device is valid
    std::string device_str = std::next(path.begin(), 1)->generic_string();
    Device device = stringToDevice(device_str);
    if (device == Device::INVALID)
        Helpers::panic("Path %s: device %s is not a valid device\n", path_str.c_str(), device_str.c_str());

    // Check if device is mounted
    if (!isDeviceMounted(device))
        Helpers::panic("Path %s: device %s is not mounted\n", path_str.c_str(), device_str.c_str());

    // Convert to host path
    std::string path_no_device_start = std::next(path.begin(), 2)->generic_string();
    fs::path path_no_device = path_str.substr(path_str.find(path_no_device_start));
    
    guest_path = mounted_devices[device] / path_no_device;
    return guest_path;
}

std::string Filesystem::deviceToString(Filesystem::Device device) {
    switch (device) {
    case Device::DEV_FLASH: return "dev_flash";
    case Device::DEV_HDD0:  return "dev_hdd0";
    }
}

Filesystem::Device Filesystem::stringToDevice(std::string device) {
    if (device == "dev_flash") return Device::DEV_FLASH;
    else if (device == "dev_hdd0") return Device::DEV_HDD0;
    else return Device::INVALID;
}