#include "CellSaveData.hpp"
#include "PlayStation3.hpp"
#include <Loaders/SFO/SFOLoader.hpp>


u64 CellSaveData::cellSaveDataAutoSave2() {
    log("cellSaveDataAutoSave2() UNIMPLEMENTED\n");

    return CELL_OK;
}

u64 CellSaveData::cellSaveDataUserAutoLoad() {
    // Just forward it to cellSaveDataAutoLoad2
    // Fix the parameters
    ARG1 = ARG2;
    ARG2 = ARG3;
    ARG3 = ARG4;
    ARG4 = ARG5;
    ARG5 = ARG6;
    ARG6 = ARG7;
    ARG7 = ARG8;
    return cellSaveDataAutoLoad2();
}

u64 CellSaveData::cellSaveDataAutoLoad2() {
    const u32 ver = ARG0;
    const u32 dir_ptr = ARG1;
    const u32 err_dialog = ARG2;
    const u32 set_buf_ptr = ARG3;
    const u32 stat_cb_ptr = ARG4;
    const u32 file_cb_ptr = ARG5;
    const u32 container = ARG6;
    const u32 userdata_ptr = ARG7;
    const auto dir = Helpers::readString(ps3->mem.getPtr(dir_ptr));
    log("cellSaveDataAutoLoad2(ver: 0x%08x, dir_ptr: 0x%08x, err_dialog: 0x%08x, set_buf_ptr: 0x%08x, stat_cb_ptr: 0x%08x, file_cb_ptr: 0x%08x, container: 0x%08x, userdata_ptr: 0x%08x) [dir: %s]\n", ver, dir_ptr, err_dialog, set_buf_ptr, stat_cb_ptr, file_cb_ptr, container, userdata_ptr, dir.c_str());

    return CELL_OK; // Disabled until I fix it

    SFOLoader sfo_loader = SFOLoader(ps3->fs);
    CellSaveDataSetBuf* set_buf = (CellSaveDataSetBuf*)ps3->mem.getPtr(set_buf_ptr);

    const u32 res_ptr = ps3->mem.alloc(sizeof(CellSaveDataCBResult))->vaddr;
    const u32 stat_get_ptr = ps3->mem.alloc(sizeof(CellSaveDataStatGet))->vaddr;
    const u32 stat_set_ptr = ps3->mem.alloc(sizeof(CellSaveDataStatSet))->vaddr;
    const u32 file_get_ptr = ps3->mem.alloc(sizeof(CellSaveDataFileGet))->vaddr;
    const u32 file_set_ptr = ps3->mem.alloc(sizeof(CellSaveDataFileSet))->vaddr;
    CellSaveDataCBResult* res = (CellSaveDataCBResult*)ps3->mem.getPtr(res_ptr);
    CellSaveDataStatGet* stat_get = (CellSaveDataStatGet*)ps3->mem.getPtr(stat_get_ptr);
    CellSaveDataStatSet* stat_set = (CellSaveDataStatSet*)ps3->mem.getPtr(stat_set_ptr);
    CellSaveDataFileGet* file_get = (CellSaveDataFileGet*)ps3->mem.getPtr(file_get_ptr);
    CellSaveDataFileSet* file_set = (CellSaveDataFileSet*)ps3->mem.getPtr(file_set_ptr);
    std::memset(res, 0, sizeof(CellSaveDataCBResult));
    std::memset(stat_get, 0, sizeof(CellSaveDataStatGet));
    std::memset(stat_set, 0, sizeof(CellSaveDataStatSet));

    // Get full path of savedata
    const fs::path savedata_path = "/dev_hdd0/home/" + ps3->getCurrentUserID() + "/savedata/" + dir;
    fs::create_directory(ps3->fs.guestPathToHost(savedata_path));
    
    // Check if a PARAM.SFO file already exists (if not, it means there is no savedata)
    bool is_new_data = !ps3->fs.exists(savedata_path / "PARAM.SFO");
    stat_get->is_new_data = is_new_data;
    stat_get->hdd_free_size_kb = 40_GB - 1_GB;

    // TODO: atime, mtime, ctime
    stat_get->dir.atime = 123456789;
    stat_get->dir.mtime = 123456789;
    stat_get->dir.ctime = 123456789;
    std::strcpy(stat_get->dir.dir_name, dir.c_str());

    stat_get->bind = 0;  // ?

    // Grab data from the SFO
    auto sfo = sfo_loader.parse(savedata_path / "PARAM.SFO");
    if (!is_new_data) {
        std::strcpy(stat_get->get_param.title, (char*)sfo.strings["TITLE"].c_str());
        std::strcpy(stat_get->get_param.sub_title, (char*)sfo.strings["SUB_TITLE"].c_str());
        std::strcpy(stat_get->get_param.detail, (char*)sfo.strings["DETAIL"].c_str());
        std::strcpy(stat_get->get_param.list_param, (char*)sfo.strings["SAVEDATA_LIST_PARAM"].c_str());
    }

    // Iterate over files in the savedata directory and populate the file list

    // file_num contains the total amount of files in the directory, file_list_num contains the number of files in the file list.
    // They can differ because in CellSaveDataSetBuf you can limit the file list size, but we should still report the correct amount of files in file_num.
    stat_get->file_num = 0;
    stat_get->file_list_num = 0;
    stat_get->file_list_ptr = set_buf->buf_ptr;
    stat_get->size_kb = 0;
    stat_get->sys_size_kb = 35;  // Always 35kb
    
    CellSaveDataFileStat* file_list = (CellSaveDataFileStat*)ps3->mem.getPtr(stat_get->file_list_ptr);
    for (auto& i : fs::directory_iterator(ps3->fs.guestPathToHost(savedata_path))) {
        // Skip PARAM.SFO
        if (i.path().filename() == "PARAM.SFO") continue;

        stat_get->file_num = stat_get->file_num + 1;

        // Check if we went over the file list limit in the buffer settings
        if (stat_get->file_list_num > set_buf->file_list_max) continue;  // Don't continue populating the list, but keep updating file_num
        CellSaveDataFileStat& file = file_list[stat_get->file_list_num];
        stat_get->file_list_num = stat_get->file_list_num + 1;

        std::strcpy(file.file_name, i.path().filename().generic_string().c_str());
        file.size = fs::file_size(i.path());
        if (file.size < 1024) file.size = 1024;
        stat_get->size_kb = stat_get->size_kb + file.size;
        
        // TODO: atime, mtime, ctime
        file.atime = 123456789;
        file.mtime = 123456789;
        file.ctime = 123456789;

        if (i.path().filename() == "ICON0.PNG") file.filetype = CELL_SAVEDATA_FILETYPE_CONTENT_ICON0;
        else if (i.path().filename() == "ICON1.PAM") file.filetype = CELL_SAVEDATA_FILETYPE_CONTENT_ICON1;
        else if (i.path().filename() == "PIC1.PNG") file.filetype = CELL_SAVEDATA_FILETYPE_CONTENT_PIC1;
        else if (i.path().filename() == "SND0.AT3") file.filetype = CELL_SAVEDATA_FILETYPE_CONTENT_SND0;
        else if (sfo.ints.contains("*" + i.path().filename().generic_string())) {
            if (sfo.ints["*" + i.path().filename().generic_string()])
                file.filetype = CELL_SAVEDATA_FILETYPE_SECUREFILE;
            else file.filetype = CELL_SAVEDATA_FILETYPE_NORMALFILE;
        }
        else file.filetype = CELL_SAVEDATA_FILETYPE_NORMALFILE;
    }

    // Convert from bytes to kb and add sys_size
    stat_get->size_kb = stat_get->size_kb / 1024 + stat_get->sys_size_kb;

    // Log data
    log("Listing savedata %s...\n", dir.c_str());
    for (int i = 0; i < stat_get->file_list_num; i++) {
        log("- name: \"%s\" size: %lldkb %s\n", file_list[i].file_name, file_list[i].size / 1024, file_list[i].filetype == CELL_SAVEDATA_FILETYPE_SECUREFILE ? "(secure)" : "");
    }

    res->userdata_ptr = userdata_ptr;
    const u32 stat_cb_addr  = ps3->mem.read<u32>(stat_cb_ptr + 0);
    const u32 stat_cb_toc   = ps3->mem.read<u32>(stat_cb_ptr + 4);
    const u32 file_cb_addr  = ps3->mem.read<u32>(file_cb_ptr + 0);
    const u32 file_cb_toc   = ps3->mem.read<u32>(file_cb_ptr + 4);
    log("Stat cb func = addr: 0x%08x, toc: 0x%08x\n", stat_cb_addr, stat_cb_toc);
    log("File cb func = addr: 0x%08x, toc: 0x%08x\n", file_cb_addr, file_cb_toc);
    
    // Call the stat callback function
    ARG0 = res_ptr;
    ARG1 = stat_get_ptr;
    ARG2 = stat_set_ptr;
    ps3->ppu->runFunc(stat_cb_addr, stat_cb_toc);
    if (res->result != CELL_SAVEDATA_CBRESULT_OK_NEXT) {
        //Helpers::panic("cellSaveDataAutoLoad2: stat cb returned %d\n", (u32)res->result);
        log("cellSaveDataAutoLoad2: stat cb returned %d\n", (u32)res->result);
    }
    log("Stat cb: returned OK\n");

    // Call the file operation callback to figure out what and how many files we need to read
    if (file_cb_ptr) {
        log("Loading savedata (path: %s)...\n", savedata_path.generic_string().c_str());
        while (true) {
            std::memset(ps3->mem.getPtr(res_ptr), 0, sizeof(CellSaveDataCBResult) - sizeof(u32));   // Don't clear the userdata
            std::memset(ps3->mem.getPtr(file_get_ptr), 0, sizeof(CellSaveDataFileGet));
            std::memset(ps3->mem.getPtr(file_set_ptr), 0, sizeof(CellSaveDataFileSet));
            ARG0 = res_ptr;
            ARG1 = file_get_ptr;
            ARG2 = file_set_ptr;
            ps3->ppu->runFunc(file_cb_addr, file_cb_toc);

            // Are we done loading?
            if (res->result == CELL_SAVEDATA_CBRESULT_OK_LAST || res->result == CELL_SAVEDATA_CBRESULT_OK_LAST_NOCONFIRM) {
                log("Savedata loaded successfully\n");
                break;
            }

            // Was there an error?
            if (res->result != CELL_SAVEDATA_CBRESULT_OK_NEXT) {
                //Helpers::panic("cellSaveDataAutoLoad2: file cb returned %d\n", (u32)res->result);
                log("cellSaveDataAutoLoad2: file cb returned %d\n", (u32)res->result);
                return 0x8002b407;  // CELL_SAVEDATA_ERROR_FAILURE
            }

            if (file_set->file_name) {
                std::string filename;
                switch (file_set->filetype) {

                case CELL_SAVEDATA_FILETYPE_CONTENT_ICON0:  filename = "ICON0.PNG"; break;
                case CELL_SAVEDATA_FILETYPE_CONTENT_ICON1:  filename = "ICON1.PAM"; break;
                case CELL_SAVEDATA_FILETYPE_CONTENT_PIC1:   filename = "PIC1.PNG";  break;
                case CELL_SAVEDATA_FILETYPE_CONTENT_SND0:   filename = "SND0.AT3";  break;
                case CELL_SAVEDATA_FILETYPE_SECUREFILE:
                case CELL_SAVEDATA_FILETYPE_NORMALFILE:     filename = Helpers::readString(ps3->mem.getPtr(file_set->file_name));   break;
                default: Helpers::panic("cellSaveDataAutoLoad2: unimplemented filetype %d\n", (u32)file_set->filetype);

                }

                const fs::path file_path = savedata_path / filename;
                log("- Requested file \"%s\"\n", filename.c_str());

                // TODO: Load the file
            }
        }
    }

    return CELL_OK;
}