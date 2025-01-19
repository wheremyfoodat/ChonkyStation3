#pragma once

#include <common.hpp>
#include <logger.hpp>

#include <unordered_map>


class Filesystem {
public:
    enum class Device {
        DEV_FLASH,
        DEV_HDD0,
        INVALID
    };
    
    void mount(Device device, fs::path path);
    bool isDeviceMounted(Device device);
    fs::path guestPathToHost(fs::path path);
    static std::string deviceToString(Device device);
    static Device stringToDevice(std::string device);

    std::unordered_map<Device, fs::path> mounted_devices;

private:
    MAKE_LOG_FUNCTION(log, filesystem);
};