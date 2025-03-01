#pragma once

#include <common.hpp>
#include <logger.hpp>

#include <SPUTypes.hpp>


using namespace SPUTypes;

// Circular dependency
class PlayStation3;

class SPUThread {
public:
    SPUThread(PlayStation3* ps3, std::string name);
    PlayStation3* ps3;

    SPUTypes::State state;
    u32 id;
    std::string name;

private:
    MAKE_LOG_FUNCTION(log, thread_spu);
};