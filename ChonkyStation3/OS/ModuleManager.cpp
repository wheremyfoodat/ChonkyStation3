#include "ModuleManager.hpp"
#include "PlayStation3.hpp"


void ModuleManager::call(u32 nid) {
    if (import_map.find(nid) == import_map.end())
        Helpers::panic("Unimplemented function unk_0x%08x\n", nid);

    ps3->ppu->state.gprs[3] = import_map[nid].handler();
}

void ModuleManager::registerImport(u32 addr, u32 nid) {
    imports[addr] = nid;
}

std::string ModuleManager::getImportName(const u32 nid) {
    if (import_map.find(nid) == import_map.end())
        return std::format("unk_{:08x}", nid);
    else
        return import_map[nid].name;
}