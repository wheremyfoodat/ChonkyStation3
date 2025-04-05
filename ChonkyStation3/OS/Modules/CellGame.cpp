#include "CellGame.hpp"
#include "PlayStation3.hpp"


void CellGame::setContentPath(fs::path path) {
    content_path = path.generic_string();
}

u64 CellGame::cellGameGetParamString() {
    const u32 id = ARG0;
    const u32 buf_ptr = ARG1;
    const u32 buf_size = ARG2;
    log("cellGameGetParamString(id: %d, buf_ptr: 0x%08x, buf_size: %d)", id, buf_ptr, buf_size);

    if (ps3->curr_game.sfo.strings.contains(id_to_param[id])) {
        auto str = ps3->curr_game.sfo.strings[id_to_param[id]];
        str += u8'\0';  // Just to be safe
        logNoPrefix(" [string: %s]\n", str.c_str());
        std::strncpy((char*)ps3->mem.getPtr(buf_ptr), (char*)str.c_str(), buf_size);
    }
    else
        Helpers::panic("\ncellGameGetParamString: SFO doesn't contain param id %d\n", id);

    return CELL_OK;
}

u64 CellGame::cellGameContentPermit() {
    const u32 content_info_dir_ptr = ARG0;
    const u32 user_dir_ptr = ARG1;
    log("cellGameContentPermit(content_info_dir_ptr: 0x%08x, user_dir_ptr: 0x%08x)\n", content_info_dir_ptr, user_dir_ptr);

    const std::string path = content_path + "\0\0";
    const std::string path_user = content_path + "/USRDIR\0\0";
    std::memcpy(ps3->mem.getPtr(content_info_dir_ptr), path.c_str(), path.length() + 1);
    std::memcpy(ps3->mem.getPtr(user_dir_ptr), path_user.c_str(), path_user.length() + 1);

    return CELL_OK;
}

u64 CellGame::cellGameContentErrorDialog() {
    const u32 type = ARG0;
    const u32 need_space_kb = ARG1;
    const u32 dir_name_ptr = ARG2;
    log("cellGameContentErrorDialog(type: 0x%08x, need_space_kb: %d, dir_name_ptr: 0x%08x)\n", type, need_space_kb, dir_name_ptr);

    return CELL_OK;
}

u64 CellGame::cellGameGetParamInt() {
    const u32 id = ARG0;
    const u32 val_ptr = ARG1;
    log("cellGameGetParamInt(id: %d, val_ptr: 0x%08x)\n", id, val_ptr);

    if (ps3->curr_game.sfo.ints.contains(id_to_param[id])) {
        u32 val = ps3->curr_game.sfo.ints[id_to_param[id]];
        ps3->mem.write<u32>(val_ptr, val);
        logNoPrefix(" [int: %d]\n", val);
    }
    else
        Helpers::panic("\ncellGameGetParamString: SFO doesn't contain param id %d\n", id);

    return CELL_OK;
}

u64 CellGame::cellGamePatchCheck() {
    const u32 size_ptr = ARG0;
    const u32 reserved = ARG1;
    log("cellGamePatchCheck(size_ptr: 0x%08x, reserved: 0x%08x) STUBBED\n", size_ptr, reserved);

    return CELL_GAME_ERROR_NOTPATCH;
}

u64 CellGame::cellGameDataCheck() {
    const u32 type = ARG0;
    const u32 dir_ptr = ARG1;
    const u32 size_ptr = ARG2;
    log("cellGameDataCheck(type: 0x%08x, dir_ptr: 0x%08x, size_ptr: 0x%08x)", type, dir_ptr, size_ptr);

    if (!dir_ptr) {
        logNoPrefix("\n");
        Helpers::panic("cellGameDataCheck(): dir_ptr is null\n");
    }
    const std::string dir = Helpers::readString(ps3->mem.getPtr(dir_ptr));
    logNoPrefix(" [dir: %s]\n", dir.c_str());

    // TODO: size
    const fs::path sfo_path = "/dev_hdd0/game" / fs::path(dir) / "PARAM.SFO";
    if (!ps3->fs.exists(sfo_path))
        return CELL_GAME_RET_NONE;

    return CELL_OK;
}

u64 CellGame::cellGameBootCheck() {
    const u32 type_ptr = ARG0;
    const u32 attrib_ptr = ARG1;
    const u32 size_ptr = ARG2;
    const u32 dir_ptr = ARG3;
    log("cellGameBootCheck(type_ptr: 0x%08x, attrib_ptr: 0x%08x, size_ptr: 0x%08x, dir_ptr: 0x%08x)\n", type_ptr, attrib_ptr, size_ptr, dir_ptr);

    ps3->mem.write<u32>(type_ptr, CELL_GAME_GAMETYPE_HDD);
    ps3->mem.write<u32>(attrib_ptr, 0);

    if (dir_ptr) {
        const std::string path = content_path + "\0\0";
        std::memcpy(ps3->mem.getPtr(dir_ptr), path.c_str(), path.length() + 1);
    }

    if (size_ptr) {
        CellGameContentSize* size = (CellGameContentSize*)ps3->mem.getPtr(size_ptr);
        size->hdd_free = 1024 * 1024 * 1024;    // 1 GB
        size->size = -1;
        size->sys_size = 1024;  // ?
    }

    return CELL_OK;
}

u64 CellGame::cellDiscGameGetBootDiscInfo() {
    const u32 param_ptr = ARG0;
    log("cellDiscGameGetBootDiscInfo(param_ptr: 0x%08x)\n", param_ptr);
    
    return CELL_DISCGAME_ERROR_NOT_DISCBOOT;
}