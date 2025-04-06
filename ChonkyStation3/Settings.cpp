#include "Settings.hpp"


void Settings::load() {
    fs::path path = SDL_GetPrefPath("ChonkyStation", "ChonkyStation3");
    path /= "config.toml";
    printf("Loading configuration file at %s\n", path.generic_string().c_str());

    // TODO: More error checks on the file
    if (!fs::exists(path)) {
        printf("No configuration file found, creating a new one\n");
        save();
        return;
    }

    auto cfg = toml::parse(path);
    if (   !cfg.contains("System")
        || !cfg.contains("LLEModules")
        || !cfg.contains("Filesystem")
        || !cfg.contains("Debug")
       ) {
        // Broken or outdated configuration file, create a new one
        detected_broken_config = true;
        printf("Detected broken or outdated configuration file, creating a new one\n");
        fs::remove(path);
        save();
        return;
    }
    
    system.nickname = cfg["System"]["Nickname"].as_string();
    
    lle.partialLv2LLE   = cfg["LLEModules"]["PartialLv2LLE"].as_boolean();
    lle.sys_fs          = cfg["LLEModules"]["sys_fs"].as_boolean();
    lle.cellResc        = cfg["LLEModules"]["cellResc"].as_boolean();
    lle.cellPngDec      = cfg["LLEModules"]["cellPngDec"].as_boolean();
    lle.cellFont        = cfg["LLEModules"]["cellFont"].as_boolean();
    lle.cellFontFT      = cfg["LLEModules"]["cellFontFT"].as_boolean();
    lle.cell_FreeType2  = cfg["LLEModules"]["cell_FreeType2"].as_boolean();
    lle.cellSync        = cfg["LLEModules"]["cellSync"].as_boolean();
    lle.cellSync2       = cfg["LLEModules"]["cellSync2"].as_boolean();
    lle.cellSpurs       = cfg["LLEModules"]["cellSpurs"].as_boolean();
    lle.cellSpursJq     = cfg["LLEModules"]["cellSpursJq"].as_boolean();
    lle.cellKey2char    = cfg["LLEModules"]["cellKey2char"].as_boolean();
    lle.cellL10n        = cfg["LLEModules"]["cellL10n"].as_boolean();
    lle.cellFiber       = cfg["LLEModules"]["cellFiber"].as_boolean();
    
    filesystem.dev_hdd0_mountpoint      = cfg["Filesystem"]["dev_hdd0_mountpoint"].as_string();
    filesystem.dev_hdd1_mountpoint      = cfg["Filesystem"]["dev_hdd1_mountpoint"].as_string();
    filesystem.dev_flash_mountpoint     = cfg["Filesystem"]["dev_flash_mountpoint"].as_string();
    filesystem.dev_usb000_mountpoint    = cfg["Filesystem"]["dev_usb000_mountpoint"].as_string();

    debug.disable_spu          = cfg["Debug"]["DisableSPU"].as_boolean();
    debug.enable_spu_after_pc  = cfg["Debug"]["EnableSPUAfterPC"].as_string();
    debug.spu_thread_to_enable = cfg["Debug"]["SPUThreadToEnable"].as_string();

    if (lle.cellPngDec) printf("Warning: enabled LLE cellPngDec\n");
}

void Settings::save() {
    fs::path path = SDL_GetPrefPath("ChonkyStation", "ChonkyStation3");
    path /= "config.toml";
    printf("Saving configuration file at %s\n", path.generic_string().c_str());

    std::ofstream file(path);
    auto cfg = toml::parse(path);

    cfg["System"]["Nickname"] = system.nickname;
    
    cfg["LLEModules"]["PartialLv2LLE"]      = lle.partialLv2LLE;
    cfg["LLEModules"]["sys_fs"]             = lle.sys_fs;
    cfg["LLEModules"]["cellResc"]           = lle.cellResc;
    cfg["LLEModules"]["cellPngDec"]         = lle.cellPngDec;
    cfg["LLEModules"]["cellFont"]           = lle.cellFont;
    cfg["LLEModules"]["cellFontFT"]         = lle.cellFontFT;
    cfg["LLEModules"]["cell_FreeType2"]     = lle.cell_FreeType2;
    cfg["LLEModules"]["cellSync"]           = lle.cellSync;
    cfg["LLEModules"]["cellSync2"]          = lle.cellSync2;
    cfg["LLEModules"]["cellSpurs"]          = lle.cellSpurs;
    cfg["LLEModules"]["cellSpursJq"]        = lle.cellSpursJq;
    cfg["LLEModules"]["cellKey2char"]       = lle.cellKey2char;
    cfg["LLEModules"]["cellL10n"]           = lle.cellL10n;
    cfg["LLEModules"]["cellFiber"]          = lle.cellFiber;
    
    cfg["Filesystem"]["dev_hdd0_mountpoint"]    = filesystem.dev_hdd0_mountpoint;
    cfg["Filesystem"]["dev_hdd1_mountpoint"]    = filesystem.dev_hdd1_mountpoint;
    cfg["Filesystem"]["dev_flash_mountpoint"]   = filesystem.dev_flash_mountpoint;
    cfg["Filesystem"]["dev_usb000_mountpoint"]  = filesystem.dev_usb000_mountpoint;
    
    cfg["Debug"]["DisableSPU"]              = debug.disable_spu;
    cfg["Debug"]["EnableSPUAfterPC"]        = debug.enable_spu_after_pc;
    cfg["Debug"]["SPUThreadToEnable"]       = debug.spu_thread_to_enable;

    file << toml::format(cfg);
    file.close();
}