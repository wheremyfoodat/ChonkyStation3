#include "ModuleManager.hpp"


void ModuleManager::call(u32 nid) {
    switch (nid) {
    default:
        std::string import_name;
        if (import_names.find(nid) != import_names.end())
            Helpers::panic("Unimplemented function %s\n", import_names[nid].c_str());
        else
            Helpers::panic("Unimplemented function unk_0x%08x\n", nid);
    }
}

void ModuleManager::registerImport(u32 addr, u32 nid) {
    imports[addr] = nid;
}