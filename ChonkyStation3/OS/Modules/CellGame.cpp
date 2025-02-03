#include "CellGame.hpp"
#include "PlayStation3.hpp"


void CellGame::setContentPath(fs::path path) {
    content_path = path.generic_string();
}

u64 CellGame::cellGameContentPermit() {
    const u32 content_info_dir_ptr = ARG0;
    const u32 user_dir_ptr = ARG1;
    log("cellGameContentPermit(content_info_dir_ptr: 0x%08x, user_dir_ptr: 0x%08x)\n", content_info_dir_ptr, user_dir_ptr);

    const std::string path = content_path + "\0\0";
    const std::string path_user = content_path + "/USRDIR\0\0";
    std::memcpy(ps3->mem.getPtr(content_info_dir_ptr), path.c_str(), path.length() + 1);
    std::memcpy(ps3->mem.getPtr(user_dir_ptr), path_user.c_str(), path_user.length() + 1);

    return Result::CELL_OK;
}

u64 CellGame::cellGameContentErrorDialog() {
    const u32 type = ARG0;
    const u32 need_space_kb = ARG1;
    const u32 dir_name_ptr = ARG2;
    log("cellGameContentErrorDialog(type: 0x%08x, need_space_kb: %d, dir_name_ptr: 0x%08x)\n", type, need_space_kb, dir_name_ptr);

    return Result::CELL_OK;
}

u64 CellGame::cellGameBootCheck() {
    const u32 type_ptr = ARG0;
    const u32 attrib_ptr = ARG1;
    const u32 size_ptr = ARG2;
    const u32 dir_ptr = ARG3;
    log("cellGameBootCheck(type_ptr: 0x%08x, attrib_ptr: 0x%08x, size_ptr: 0x%08x, dir_ptr: 0x%08x)\n", type_ptr, attrib_ptr, size_ptr, dir_ptr);

    ps3->mem.write<u32>(type_ptr, CELL_GAME_GAMETYPE_HDD);
    ps3->mem.write<u32>(attrib_ptr, 0);
    const std::string path = content_path + "\0\0";
    std::memcpy(ps3->mem.getPtr(dir_ptr), path.c_str(), path.length() + 1);

    return Result::CELL_OK;
}

u64 CellGame::cellDiscGameGetBootDiscInfo() {
    const u32 param_ptr = ARG0;
    log("cellDiscGameGetBootDiscInfo(param_ptr: 0x%08x)\n", param_ptr);
    
    return CELL_DISCGAME_ERROR_NOT_DISCBOOT;
}