#pragma once

#include <common.hpp>
#include <CellTypes.hpp>
#include <BEField.hpp>
#include <PPUTypes.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class SysMMapper {
public:
    SysMMapper(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;
    
    u64 sysMMapperAllocateMemory();
};