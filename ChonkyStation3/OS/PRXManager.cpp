#include "PRXManager.hpp"
#include "PlayStation3.hpp"
#include <PRX/PRXLoader.hpp>


void PRXManager::require(const std::string name) {
    if (!lle_modules.contains(name))
        Helpers::panic("%s is not a lle module\n", name.c_str());
    required_modules.push_back(lle_modules[name]);

    // Check if library is present
    fs::path lib_path = lle_lib_dir / lle_modules[name];
    if (!fs::exists(lib_path)) {
        Helpers::panic("Required %s is missing", lle_modules[name].c_str());
    }
}

void PRXManager::loadModules() {
    PRXExportTable exports;
    PRXLoader loader = PRXLoader(ps3);

    // Load modules
    log("Loading %d PRXs:\n", required_modules.size());
    for (auto& i : required_modules)
        log("* %s\n", i);

    for (auto& i : required_modules)
        libs.push_back(loader.load(lle_lib_dir / i, exports));

    // Update export table
    ps3->module_manager.registerExportTable(exports);
}

void PRXManager::initializeLibraries() {
    // Initialize libraries
    for (auto& i : libs) {
        log("Initializing lib %s...\n", i.name.c_str());
        ps3->ppu->runFunc(ps3->mem.read<u32>(i.start_func), i.toc);
        log("Done\n");
    }
}