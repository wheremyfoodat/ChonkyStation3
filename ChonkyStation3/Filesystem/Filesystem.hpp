#pragma once

#include <common.hpp>
#include <logger.hpp>

#include <unordered_map>


// Circular dependency
class PlayStation3;

class Filesystem {
public:
    Filesystem(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    enum class Device {
        DEV_FLASH,
        DEV_HDD0,
        DEV_HDD1,
        APP_HOME,
        INVALID
    };

    struct File {
        FILE* file;
        fs::path path;
    };
    
    void mount(Device device, fs::path path);
    void initialize();
    u32 open(fs::path path);
    void close(u32 file_id);
    u64 read(u32 file_id, u8* buf, u64 size);
    u64 seek(u32 file_id, s32 offs, u32 mode);
    u64 getFileSize(u32 file_id);
    u64 getFileSize(fs::path path);
    bool isDirectory(u32 file_id);
    bool isDirectory(fs::path path);
    bool exists(fs::path path);
    File& getFileFromID(u32 id);
    bool isDeviceMounted(Device device);
    fs::path guestPathToHost(fs::path path);
    static std::string deviceToString(Device device);
    static Device stringToDevice(std::string device);

    std::unordered_map<Device, fs::path> mounted_devices;
    std::unordered_map<u32, File> open_files;

private:
    MAKE_LOG_FUNCTION(log, filesystem);
};