#include "CellSpurs.hpp"
#include "PlayStation3.hpp"


// TODO: SPU :(

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

u64 CellSpurs::cellSpursEventFlagAttachLv2EventQueue() {
    const u32 event_flag_ptr = ARG0;
    log("cellSpursEventFlagAttachLv2EventQueue(event_flag_ptr: 0x%08x) UNIMPLEMENTED\n", event_flag_ptr);

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