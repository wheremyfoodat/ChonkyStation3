#pragma once

#include <common.hpp>
#include <logger.hpp>

#include <unordered_map>

#include <PRX/PRXLibraryInfo.hpp>


// Circular dependency
class PlayStation3;

class PRXManager {
public:
    PRXManager(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    // This is a list of modules that *need* to be LLEd.
    // If the modules aren't present and the game requires them, the emulator will panic.
    // The map maps the module name to the library filename.
    // TODO: Switch these to the .sprx variants once we can decrypt SELFs/SPRXs.
    // For now you need to decrypt the libraries on your own.
    std::unordered_map<std::string, std::string> lle_modules {
        { "cellResc",       "libresc.prx" },
        { "cellPngDec",     "libpngdec.prx" },
        //{ "cellFont",       "libfont.prx" },
        //{ "cellFontFT",     "libfontFT.prx" },
        //{ "cell_FreeType2", "libfreetype.prx" },
    };
    fs::path lib_dir = "/dev_flash/sys/external/";

    std::vector<std::string> required_modules;  // List of required modules, load them with loadModules()
    std::vector<PRXLibraryInfo> libs;   // List of loaded libraries

    bool isLLEModule(const std::string name) { return lle_modules.contains(name); }
    bool isLibLoaded(const std::string name);
    void require(const std::string name);
    bool loadModules(); // Returns true if at least 1 module was loaded
    void loadModulesRecursively();
    void initializeLibraries();

private:
    MAKE_LOG_FUNCTION(log, manager_prx);
};