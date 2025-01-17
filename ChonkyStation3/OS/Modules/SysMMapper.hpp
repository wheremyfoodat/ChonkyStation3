#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <BEField.hpp>

#include <CellTypes.hpp>
#include <PPUTypes.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class SysMMapper {
public:
    SysMMapper(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;
    MAKE_LOG_FUNCTION(log, sysMMapper);
    
    u64 sysMMapperAllocateMemory();
    u64 sysMMapperFreeMemory();
    u64 sysMMapperUnmapMemory();
};