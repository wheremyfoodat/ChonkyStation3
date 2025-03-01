#pragma once

#include <common.hpp>
#include <logger.hpp>

#include <SPUTypes.hpp>
#include <OS/Syscalls/sys_spu.hpp>


using namespace SPUTypes;
using namespace sys_spu;

// Circular dependency
class PlayStation3;

class SPUThread {
public:
    SPUThread(PlayStation3* ps3, std::string name);
    PlayStation3* ps3;

    SPUTypes::State state;
    u8* ls = new u8[16_KB];

    u32 id;
    std::string name;

    void loadImage(sys_spu_image* img);

private:
    MAKE_LOG_FUNCTION(log, thread_spu);
};