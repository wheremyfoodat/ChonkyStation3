#pragma once

#include <common.hpp>
#include <CellTypes.hpp>
#include <BEField.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class CellGcmSys {
public:
    CellGcmSys(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    struct CellGcmConfig {
        BEField<u32> local_addr;    // Pointer to RSX local memory
        BEField<u32> io_addr;
        BEField<u32> local_size;    // Size of RSX memory
        BEField<u32> io_size;
        BEField<u32> memFreq;
        BEField<u32> coreFreq;
    };
    CellGcmConfig gcm_config;

    u64 cellGcmInitBody();
    u64 cellGcmGetConfiguration();
};