#pragma once

#include <common.hpp>


struct PRXLibraryInfo {
    std::string name;
    u32 toc;
    u32 start_func;
    u32 stop_func;
};