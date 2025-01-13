#include "CellSysmodule.hpp"
#include "PlayStation3.hpp"


u64 CellSysmodule::cellSysmoduleLoadModule() {
    const u16 module_id = ARG0;
    const auto module_name = getModule(module_id);
    log("cellSysmoduleLoadModule(module: %s)\n", module_name.c_str());

    if (module_name == "BAD")
        Helpers::panic("cellSysmoduleLoadModule(): bad module id %d\n", module_id);

    return Result::CELL_OK;
}

std::string CellSysmodule::getModule(u16 id) {
    if (!module_names.contains(id))
        return "BAD";
    return module_names[id];
}