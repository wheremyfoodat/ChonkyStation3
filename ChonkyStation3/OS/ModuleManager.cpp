#include "ModuleManager.hpp"
#include "PlayStation3.hpp"


void ModuleManager::call(u32 nid) {
    if (import_map.find(nid) == import_map.end())
        Helpers::panic("Unimplemented function unk_0x%08x\n", nid);

    ps3->ppu->state.gprs[3] = (u32)import_map[nid].handler();
}

void ModuleManager::registerImport(u32 addr, u32 nid) {
    imports[addr] = nid;
}