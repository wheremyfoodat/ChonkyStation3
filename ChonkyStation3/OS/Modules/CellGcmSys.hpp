#pragma once

#include <common.hpp>
#include <CellTypes.hpp>
#include <BEField.hpp>
#include <functional>


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

    struct CellGcmContextData {
        BEField<u32> begin;
        BEField<u32> end;
        BEField<u32> current;
        BEField<u32> callback;
    };

    struct CellGcmControl {
        BEField<u32> put;
        BEField<u32> get;
        BEField<u32> ref;
    };

    u32 ctx_addr = 0;
    CellGcmContextData* ctx;
    u32 ctrl_addr = 0;
    CellGcmControl* ctrl;

    CellGcmConfig gcm_config;
    u32 dma_ctrl_addr = 0;
    u32 label_addr = 0;

    u64 cellGcmInitBody();
    u64 cellGcmAddressToOffset();
    u64 cellGcmGetControlRegister();
    u64 cellGcmGetConfiguration();
    u64 cellGcmGetLabelAddress();
};