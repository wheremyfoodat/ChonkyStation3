#include "ModuleManager.hpp"
#include "PlayStation3.hpp"


void ModuleManager::call(u32 nid) {
    if (!import_map.contains(nid))
        Helpers::panic("Unimplemented function unk_0x%08x\n", nid);

    ps3->ppu->state.gprs[3] = import_map[nid].handler();
}

void ModuleManager::lle(u32 nid) {
    if (!exports.funcs.contains(nid))
        Helpers::panic("Could not find export for function %s\n", getImportName(nid).c_str());

    ps3->mem.watchpoints_r[ps3->ppu->state.lr + 4] = std::bind(&ModuleManager::printReturnValue, this);
    ps3->mem.markAsSlowMem((ps3->ppu->state.lr + 4) >> PAGE_SHIFT, true, false);
    last_lle_nid = nid;

    ps3->ppu->state.pc = ps3->mem.read<u32>(exports.funcs[nid].addr);
    ps3->ppu->state.gprs[2] = exports.funcs[nid].toc;

    log("%s\n", getImportName(nid).c_str());
}

void ModuleManager::registerImport(u32 addr, u32 nid) {
    imports[addr] = nid;
}

void ModuleManager::registerExportTable(const PRXExportTable& exports) {
    this->exports = exports;
}

std::string ModuleManager::getImportName(const u32 nid) {
    if (import_map.find(nid) == import_map.end())
        return std::format("unk_{:08x}", nid);
    else
        return import_map[nid].name;
}

void ModuleManager::printReturnValue() {
    log("%s returned with 0x%08x\n", getImportName(last_lle_nid).c_str(), ps3->ppu->state.gprs[3]);
}