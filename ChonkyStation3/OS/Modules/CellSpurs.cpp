#include "CellSpurs.hpp"
#include "PlayStation3.hpp"


// TODO: SPU :(

u64 CellSpurs::_cellSpursLFQueueInitialize() {
    log("_cellSpursLFQueueInitialize() UNIMPLEMENTED\n");

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursAttributeSetNamePrefix() {
    const u32 attr = ARG0;
    const u32 prefix_ptr = ARG1;
    const u32 size = ARG2;
    std::string prefix = Helpers::readString(ps3->mem.getPtr(prefix_ptr));
    log("cellSpursAttributeSetNamePrefix(attr: 0x%08x, prefix_ptr: 0x%08x, size: 0x%08x) [prefix: %s] UNIMPLEMENTED\n", attr, prefix_ptr, size, prefix.c_str());

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursAttributeEnableSpuPrintfIfAvailable() {
    const u32 attr_ptr = ARG0;
    log("cellSpursAttributeEnableSpuPrintfIfAvailable(attr_ptr: 0x%08x) UNIMPLEMENTED\n", attr_ptr);

    return Result::CELL_OK;
}

u64 CellSpurs::_cellSpursTasksetAttributeInitialize() {
    const u32 attr_ptr = ARG0;
    const u32 revision = ARG1;
    const u32 sdk_ver = ARG2;
    const u64 args = ARG3;
    const u32 prio_ptr = ARG4;
    const u32 max_contention = ARG5;
    log("_cellSpursTasksetAttributeInitialize(attr_ptr: 0x%08x, revision: 0x%08x, sdk_ver: 0x%08x, args: 0x%016llx, prio_ptr: 0x%08x, max_contention: %d) UNIMPLEMENTED\n", attr_ptr, revision, sdk_ver, args, prio_ptr, max_contention);

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursLFQueueAttachLv2EventQueue() {
    const u32 queue_ptr = ARG0;
    log("cellSpursLFQueueAttachLv2EventQueue(queue_ptr: 0x%08x) UNIMPLEMENTED\n", queue_ptr);

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursRequestIdleSpu() {
    log("cellSpursRequestIdleSpu() UNIMPLEMENTED\n");

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursCreateTaskWithAttribute() {
    log("cellSpursCreateTaskWithAttribute() UNIMPLEMENTED\n");

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursGetInfo() {
    const u32 spurs_ptr = ARG0;
    const u32 info_ptr = ARG1;
    log("cellSpursGetInfo(spurs_ptr: 0x%08x, info_ptr: 0x%08x) UNIMPLEMENTED\n", spurs_ptr, info_ptr);

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursInitializeWithAttribute2() {
    const u32 spurs_ptr = ARG0;
    const u32 attr_ptr = ARG1;
    log("cellSpursInitializeWithAttribute2(spurs_ptr: 0x%08x, attr_ptr: 0x%08x) UNIMPLEMENTED\n", spurs_ptr, attr_ptr);

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursWorkloadAttributeSetName() {
    const u32 attr_ptr = ARG0;
    const u32 name_class_ptr = ARG1;
    const u32 name_instance_ptr = ARG2;
    std::string name_class = Helpers::readString(ps3->mem.getPtr(name_class_ptr));
    std::string name_instance = Helpers::readString(ps3->mem.getPtr(name_instance_ptr));
    log("cellSpursWorkloadAttributeSetName(attr_ptr: 0x%08x, name_class_ptr: 0x%08x, name_instance_ptr: 0x%08x) [name_class: %s, name_instance: %s] UNIMPLEMENTED\n", attr_ptr, name_class_ptr, name_instance_ptr, name_class.c_str(), name_instance.c_str());

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursCreateTaskset2() {
    const u32 spurs_ptr = ARG0;
    const u32 taskset_ptr = ARG1;
    const u32 attr_ptr = ARG2;
    log("cellSpursCreateTaskset2(spurs_ptr: 0x%08x, taskset_ptr: 0x%08x, attr_ptr: 0x%08x) UNIMPLEMENTED\n", spurs_ptr, taskset_ptr, attr_ptr);

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursEventFlagClear() {
    log("cellSpursEventFlagClear() UNIMPLEMENTED\n");
    
    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursCreateTaskset() {
    const u32 spurs_ptr = ARG0;
    const u32 taskset_ptr = ARG1;
    const u32 attr_ptr = ARG2;
    log("cellSpursCreateTaskset(spurs_ptr: 0x%08x, taskset_ptr: 0x%08x, attr_ptr: 0x%08x) UNIMPLEMENTED\n", spurs_ptr, taskset_ptr, attr_ptr);

    return Result::CELL_OK;
}

u64 CellSpurs::_cellSpursEventFlagInitialize() {
    const u32 spurs_ptr = ARG0;
    const u32 taskset_ptr = ARG1;
    const u32 flag_clear_mode = ARG2;
    const u32 flag_dir = ARG3;
    log("_cellSpursEventFlagInitialize(spurs_ptr: 0x%08x, taskset_ptr: 0x%08x, flag_clear_mode: 0x%08x, flag_dir: 0x%08x) UNIMPLEMENTED\n", spurs_ptr, taskset_ptr, flag_clear_mode, flag_dir);

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursTasksetAttributeSetName() {
    const u32 attr_ptr = ARG0;
    const u32 name_ptr = ARG1;
    std::string name = Helpers::readString(ps3->mem.getPtr(name_ptr));
    log("cellSpursTasksetAttributeSetName(attr_ptr: 0x%08x, name_ptr: 0x%08x) [name: %s] UNIMPLEMENTED\n", attr_ptr, name_ptr, name.c_str());

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursAttributeSetMemoryContainerForSpuThread() {
    const u32 attr_ptr = ARG0;
    const u32 container_id = ARG1;
    log("cellSpursAttributeSetMemoryContainerForSpuThread(attr_ptr: 0x%08x, container_id: %d) UNIMPLEMENTED\n", attr_ptr, container_id);

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursAttributeEnableSystemWorkload() {
    const u32 attr_ptr = ARG0;
    const u32 prio_ptr = ARG1;
    const u32 max_spu = ARG2;
    const u32 is_preemptible_ptr = ARG3;
    std::string prio = Helpers::readString(ps3->mem.getPtr(prio_ptr));
    std::string is_preemptible = Helpers::readString(ps3->mem.getPtr(is_preemptible_ptr));
    log("cellSpursAttributeEnableSystemWorkload(attr_ptr: 0x%08x, prio_ptr: 0x%08x, max_spu: %d, is_preemptible_ptr: 0x%08x) [prio: %s, is_preemptible: %s] UNIMPLEMENTED\n", attr_ptr, prio_ptr, max_spu, is_preemptible_ptr, prio.c_str(), is_preemptible.c_str());

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursEventFlagAttachLv2EventQueue() {
    const u32 event_flag_ptr = ARG0;
    log("cellSpursEventFlagAttachLv2EventQueue(event_flag_ptr: 0x%08x) UNIMPLEMENTED\n", event_flag_ptr);

    return Result::CELL_OK;
}

u64 CellSpurs::_cellSpursLFQueuePushBody() {
    log("_cellSpursLFQueuePushBody() UNIMPLEMENTED\n");
    
    return Result::CELL_OK;
}

u64 CellSpurs::_cellSpursAttributeInitialize() {
    const u32 attr_ptr = ARG0;
    const u32 revision = ARG1;
    const u32 sdk_ver = ARG2;
    const u32 n_spus = ARG3;
    const s32 spu_prio = ARG4;
    const s32 ppu_prio = ARG5;
    const u8 exit_if_no_work = ARG5;
    log("_cellSpursAttributeInitialize(attr_ptr: 0x%08x, revision: 0x%08x, sdk_ver: 0x%08x, n_spus: 0x%08x, spu_prio: 0x%08x, ppu_prio: 0x%08x, exit_if_no_work: 0x%08x) UNIMPLEMENTED\n", attr_ptr, revision, sdk_ver, n_spus, spu_prio, ppu_prio, exit_if_no_work);

    return Result::CELL_OK;
}

u64 CellSpurs::_cellSpursWorkloadFlagReceiver() {
    const u32 spurs_ptr = ARG0;
    const u32 workload_id = ARG1;
    const u32 is_set = ARG2;
    log("_cellSpursWorkloadFlagReceiver(spurs_ptr: 0x%08x, workload_id: %d, is_set: 0x%08x) UNIMPLEMENTED\n", spurs_ptr, workload_id, is_set);

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursAttributeSetSpuThreadGroupType() {
    const u32 attr_ptr = ARG0;
    const s32 type = ARG1;
    log("cellSpursAttributeSetSpuThreadGroupType(attr_ptr: 0x%08x, type: %d) UNIMPLEMENTED\n", attr_ptr, type);

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursInitializeWithAttribute() {
    const u32 spurs_ptr = ARG0;
    const u32 attr_ptr = ARG1;
    log("cellSpursInitializeWithAttribute(spurs_ptr: 0x%08x, attr_ptr: 0x%08x) UNIMPLEMENTED\n", spurs_ptr, attr_ptr);
    
    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursInitialize() {
    const u32 spurs_ptr = ARG0;
    const u32 n_spus = ARG1;
    const u32 spu_prio = ARG2;
    const u32 ppu_prio = ARG3;
    const u8 exit_if_no_work = ARG4;
    log("cellSpursInitialize(spurs_ptr: 0x%08x, n_spus: %d, spu_prio: %d, ppu_prio: %d, exit_if_no_work: %d) UNIMPLEMENTED\n", spurs_ptr, n_spus, spu_prio, ppu_prio, exit_if_no_work);

    return Result::CELL_OK;
}

u64 CellSpurs::_cellSpursTaskAttributeInitialize() {
    log("_cellSpursTaskAttributeInitialize() UNIMPLEMENTED\n");

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursAttachLv2EventQueue() {
    const u32 spurs_ptr = ARG0;
    const u32 queue_id = ARG1;
    const u32 port_ptr = ARG2;
    const s32 is_dynamic = ARG3;
    log("cellSpursAttachLv2EventQueue(spurs_ptr: 0x%08x, queue_id: %d, port_ptr: 0x%08x, is_dynamic: %d) UNIMPLEMENTED\n", spurs_ptr, queue_id, port_ptr, is_dynamic);

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursCreateTask() {
    const u32 taskset_ptr = ARG0;
    const u32 task_id_ptr = ARG1;
    const u32 elf_ptr = ARG2;
    const u32 ctx_ptr = ARG3;
    const u32 size = ARG4;
    const u32 pattern_ptr = ARG5;
    const u32 arg_ptr = ARG6;
    log("cellSpursCreateTask(taskset_ptr: 0x%08x, task_id_ptr: 0x%08x, elf_ptr: 0x%08x, ctx_ptr: 0x%08x, size: 0x%08d, pattern_ptr: 0x%08x, arg_ptr: 0x%08x) UNIMPLEMENTED\n", taskset_ptr, task_id_ptr, elf_ptr, ctx_ptr, size, pattern_ptr, arg_ptr);

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursAddWorkloadWithAttribute() {
    const u32 spurs_ptr = ARG0;
    const u32 workload_id_ptr = ARG1;
    const u32 attr_ptr = ARG2;
    log("cellSpursAddWorkloadWithAttribute(spurs_ptr: 0x%08x, workload_id_ptr: 0x%08x, attr_ptr: 0x%08x) UNIMPLEMENTED\n", spurs_ptr, workload_id_ptr, attr_ptr);

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursCreateTasksetWithAttribute() {
    const u32 spurs_ptr = ARG0;
    const u32 taskset_ptr = ARG1;
    const u32 attr_ptr = ARG2;
    log("cellSpursCreateTasksetWithAttribute(spurs_ptr: 0x%08x, taskset_ptr: 0x%08x, attr_ptr: 0x%08x) UNIMPLEMENTED\n", spurs_ptr, taskset_ptr, attr_ptr);

    return Result::CELL_OK;
}

u64 CellSpurs::_cellSpursTasksetAttribute2Initialize() {
    const u32 attr_ptr = ARG0;
    const u32 revision = ARG1;
    log("_cellSpursTasksetAttribute2Initialize(attr_ptr: 0x%08x, revision: 0x%08x) UNIMPLEMENTED\n", attr_ptr, revision);

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursGetWorkloadFlag() {
    const u32 spurs_ptr = ARG0;
    const u32 flag_ptr = ARG1;
    log("cellSpursGetWorkloadFlag(spurs_ptr: 0x%08x, flag_ptr: 0x%08x) UNIMPLEMENTED @ 0x%08x\n", spurs_ptr, flag_ptr, ps3->ppu->state.lr);

    ps3->mem.write<u32>(flag_ptr, 0x400);  // This should be a pointer to a CellSpursWorkloadFlag I think? It's not the flag itself
    //CellSpursWorkloadFlag* flag = (CellSpursWorkloadFlag*)ps3->mem.getPtr(flag_ptr);
    //flag->flag = -1;

    return Result::CELL_OK;
}

u64 CellSpurs::cellSpursSetExceptionEventHandler() {
    const u32 handler_ptr = ARG0;
    const u32 arg_ptr = ARG1;
    log("cellSpursSetExceptionEventHandler(handler_ptr: 0x%08x, arg_ptr: 0x%08x) UNIMPLEMENTED\n");

    return Result::CELL_OK;
}

u64 CellSpurs::_cellSpursWorkloadAttributeInitialize() {
    const u32 attr_ptr = ARG0;
    const u32 revision = ARG1;
    const u32 sdk_ver = ARG2;
    const u32 pm_ptr = ARG3;
    const u32 size = ARG4;
    const u64 data = ARG5;
    const u32 prio_ptr = ARG6;
    const u32 min_cnt = ARG7;
    //const u32 max_cnt = ARG8;
    log("_cellSpursWorkloadAttributeInitialize(attr_ptr: 0x%08x, revision: 0x%08x, sdk_ver: 0x%08x, pm_ptr: 0x%08x, size: 0x%08x, data: 0x%016llx, prio_ptr: 0x%08x, min_cnt: %d, max_cnt: ???) UNIMPLEMENTED\n", attr_ptr, revision, sdk_ver, pm_ptr, size, data, prio_ptr, min_cnt);
    
    std::memset(ps3->mem.getPtr(attr_ptr), 0, sizeof(CellSpursWorkloadAttribute));
    CellSpursWorkloadAttribute* attr = (CellSpursWorkloadAttribute*)ps3->mem.getPtr(attr_ptr);
    attr->revision = revision;
    attr->sdk_ver = sdk_ver;
    attr->pm_ptr = pm_ptr;
    attr->size = size;
    attr->data = data;

    return Result::CELL_OK;
}