#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <BEField.hpp>
#include <elfio/elfio.hpp>

#include <unordered_map>

#include <OS/Syscalls/sys_spu.hpp>


// Circular dependency
class PlayStation3;

static constexpr size_t EHDR_SIZE = 42;

static constexpr s32 SYS_SPU_SEGMENT_TYPE_COPY = 0x0001;
static constexpr s32 SYS_SPU_SEGMENT_TYPE_FILL = 0x0002;
static constexpr s32 SYS_SPU_SEGMENT_TYPE_INFO = 0x0004;

using namespace sys_spu;

class SPULoader {
public:
    SPULoader(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;    

    void load(u32 img_ptr, sys_spu_image* img);

private:
    MAKE_LOG_FUNCTION(log, loader_spu);
};