#include "CellFs.hpp"
#include "PlayStation3.hpp"


u64 CellFs::cellFsStat() {
    const u32 path_ptr = ARG0;
    const u32 stat_ptr = ARG1;
    log("cellFsStat(path_ptr: 0x%08x, stat_ptr: 0x%08x)", path_ptr, stat_ptr);
    
    const std::string path = Helpers::readString(ps3->mem.getPtr(path_ptr));
    logNoPrefix(" [path: %s]\n", path.c_str());

    return Result::CELL_OK;
}