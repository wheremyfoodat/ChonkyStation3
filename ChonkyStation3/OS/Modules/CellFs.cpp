#include "CellFs.hpp"
#include "PlayStation3.hpp"


u64 CellFs::cellFsOpen() {
    const u32 path_ptr = ARG0;
    const s32 flags = ARG1;
    const u32 file_id_ptr = ARG2;
    const u32 arg_ptr = ARG3;
    const u64 size = ARG4;
    const std::string path = Helpers::readString(ps3->mem.getPtr(path_ptr));
    log("cellFsOpen(path_ptr: 0x%08x, flags: %d, file_id_ptr: 0x%08x, arg_ptr: 0x%08x, size: %d) [path: %s]\n", path_ptr, flags, file_id_ptr, arg_ptr, size, path.c_str());

    // TODO: There are probably going to be instances where opening a file that doesn't exist is supposed to happen.
    // For now if this happens it likely means something is wrong, so just crash.
    const fs::path host_path = ps3->fs.guestPathToHost(path);
    if (!fs::exists(host_path)) {
        Helpers::panic("Tried to open non-existing file %s\n", path.c_str());
    }

    const u32 new_file_id = ps3->handle_manager.request();
    FILE* file = std::fopen(host_path.generic_string().c_str(), "rb");
    open_files[new_file_id] = file;
    log("Opened file %s\n", host_path.generic_string().c_str());

    return Result::CELL_OK;
}

u64 CellFs::cellFsStat() {
    const u32 path_ptr = ARG0;
    const u32 stat_ptr = ARG1;
    const std::string path = Helpers::readString(ps3->mem.getPtr(path_ptr));
    log("cellFsStat(path_ptr: 0x%08x, stat_ptr: 0x%08x) [path: %s]\n", path_ptr, stat_ptr, path.c_str());

    return Result::CELL_OK;
}