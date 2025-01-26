#include "PRXManager.hpp"
#include "PlayStation3.hpp"
#include <PRX/PRXLoader.hpp>


void PRXManager::require(const std::string name) {
    if (!lle_modules.contains(name))
        Helpers::panic("%s is not a lle module\n", name.c_str());
    required_modules.push_back(lle_modules[name]);

    // Check if library is present
    fs::path lib_path = ps3->fs.guestPathToHost(lib_dir / lle_modules[name]);
    if (!fs::exists(lib_path)) {
        Helpers::panic("Required %s is missing", lle_modules[name].c_str());
    }
}

bool PRXManager::isLibLoaded(const std::string name) {
    bool loaded = false;

    for (auto& i : libs) {
        if (i.filename == name) {
            loaded = true;
            break;
        }
    }
    return loaded;
}

bool PRXManager::loadModules() {
    PRXExportTable exports = ps3->module_manager.getExportTable();
    PRXLoader loader = PRXLoader(ps3);
    bool loaded = false;

    // Load modules
    log("Loading PRXs:\n", required_modules.size());
    for (auto& i : required_modules) {
        if (!isLibLoaded(i)) {
            log("* %s\n", i.c_str());
        }
    }

    // We do this because loading a library might update the list of required modules.
    // Don't want that to happen while we're iterating over the same list
    const auto to_load = required_modules;

    for (auto& i : to_load) {
        if (!isLibLoaded(i)) {
            const fs::path lib_path = ps3->fs.guestPathToHost(lib_dir / i);
            libs.push_back(loader.load(lib_path, exports));
            loaded = true;
        }
    }

    // Update export table
    ps3->module_manager.registerExportTable(exports);
    return loaded;
}

void PRXManager::loadModulesRecursively() {
    while (loadModules()) {}
}

void PRXManager::initializeLibraries() {
    // Initialize libraries
    for (auto& i : libs) {
        log("Initializing lib %s...\n", i.name.c_str());
        if (i.start_func)
            ps3->ppu->runFunc(ps3->mem.read<u32>(i.start_func), ps3->mem.read<u32>(i.start_func + 4));
        log("Done\n");
    }
}