#include "CellSysCache.hpp"
#include "PlayStation3.hpp"


u64 CellSysCache::cellSysCacheMount() {
    const u32 param_ptr = ARG0;
    log("cellSysCacheMount(param_ptr: 0x%08x)\n", param_ptr);

    CellSysCacheParam* param = (CellSysCacheParam*)ps3->mem.getPtr(param_ptr);
    fs::path cache_path = "/dev_hdd1/caches/" + ps3->curr_game.id + "/" + Helpers::readString(param->cache_id);
    Helpers::debugAssert(cache_path.generic_string().size() <= CELL_SYSCACHE_PATH_MAX, "cellSysCacheMount: path %s is too long\n", cache_path.generic_string().c_str());

    log("Mounting cache at %s\n", cache_path.generic_string().c_str());
    fs::create_directories(ps3->fs.guestPathToHost(cache_path));
    std::strcpy((char*)param->cache_path, cache_path.generic_string().c_str());

    return Result::CELL_OK;
}