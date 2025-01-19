#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <BEField.hpp>

#include <CellTypes.hpp>
#include <PPUTypes.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class SysThread {
public:
    SysThread(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    u64 sysThreadGetID();
    u64 sysThreadInitializeTLS();

    void initializeTLS(u64 thread_id, u32 tls_seg_addr, u32 tls_seg_size, u32 tls_mem_size, PPUTypes::State& state);

private:
    MAKE_LOG_FUNCTION(log, sysThread);
};