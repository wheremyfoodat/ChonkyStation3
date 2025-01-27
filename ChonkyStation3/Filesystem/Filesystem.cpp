#include "Filesystem.hpp"
#include "PlayStation3.hpp"


void Filesystem::mount(Filesystem::Device device, fs::path path) {
    if (!fs::exists(path)) {
        Helpers::panic("Mount point %s for device %s does not exist\n", path.generic_string().c_str(), deviceToString(device).c_str());
    }

    mounted_devices[device] = path;
    log("Mounted device %s at %s\n", deviceToString(device).c_str(), path.generic_string().c_str());
}

u32 Filesystem::open(fs::path path) {
    // TODO: There are probably going to be instances where opening a file that doesn't exist is supposed to happen.
    // For now if this happens it likely means something is wrong, so just crash.
    const fs::path host_path = ps3->fs.guestPathToHost(path);
    if (!fs::exists(host_path)) {
        //Helpers::panic("Tried to open non-existing file %s\n", path.generic_string().c_str());
        log("WARNING: Tried to open non-existing file %s\n", path.generic_string().c_str());
        return 0;
    }

    const u32 new_file_id = ps3->handle_manager.request();
    FILE* file = std::fopen(host_path.generic_string().c_str(), "rb");
    open_files[new_file_id] = { file, host_path };
    log("Opened file %s\n", host_path.generic_string().c_str());

    return new_file_id;
}

void Filesystem::close(u32 file_id) {
    FILE* file = getFileFromID(file_id).file;
    std::fclose(file);
    open_files.erase(file_id);
}

u64 Filesystem::read(u32 file_id, u8* buf, u64 size) {
    FILE* file = getFileFromID(file_id).file;
    return std::fread(buf, sizeof(u8), size, file);
}

u64 Filesystem::seek(u32 file_id, s32 offs, u32 mode) {
    FILE* file = getFileFromID(file_id).file;
    std::fseek(file, offs, mode);
    return std::ftell(file);
}

u64 Filesystem::getFileSize(u32 file_id) {
    auto file = getFileFromID(file_id);
    return fs::file_size(file.path);
}

u64 Filesystem::getFileSize(fs::path path) {
    return fs::file_size(guestPathToHost(path));
}

bool Filesystem::isDirectory(fs::path path) {
    return fs::is_directory(guestPathToHost(path));
}

Filesystem::File& Filesystem::getFileFromID(u32 id) {
    if (!open_files.contains(id))
        Helpers::panic("File id %d does not exist\n", id);
    return open_files[id];
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