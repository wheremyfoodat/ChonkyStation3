#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <BEField.hpp>

#include <CellTypes.hpp>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

class CellSpurs {
public:
    CellSpurs(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

    struct CellSpursWorkloadFlag {
        BEField<u64> unused0;
        BEField<u32> unused1;
        BEField<u32> flag;
    };

    u64 cellSpursAttributeSetNamePrefix();
    u64 cellSpursRequestIdleSpu();
    u64 cellSpursGetInfo();
    u64 cellSpursInitializeWithAttribute2();
    u64 cellSpursWorkloadAttributeSetName();
    u64 cellSpursCreateTaskset();
    u64 _cellSpursEventFlagInitialize();
    u64 cellSpursAttributeSetMemoryContainerForSpuThread();
    u64 cellSpursEventFlagAttachLv2EventQueue();
    u64 _cellSpursAttributeInitialize();
    u64 cellSpursAttributeEnableSystemWorkload();
    u64 _cellSpursWorkloadFlagReceiver();
    u64 cellSpursInitializeWithAttribute();
    u64 cellSpursInitialize();
    u64 cellSpursAttachLv2EventQueue();
    u64 cellSpursCreateTask();
    u64 cellSpursAddWorkloadWithAttribute();
    u64 cellSpursGetWorkloadFlag();
    u64 cellSpursSetExceptionEventHandler();
    u64 _cellSpursWorkloadAttributeInitialize();

private:
    MAKE_LOG_FUNCTION(log, cellSpurs);
};