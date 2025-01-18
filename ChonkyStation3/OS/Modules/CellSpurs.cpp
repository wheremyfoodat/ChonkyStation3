#include "CellSpurs.hpp"
#include "PlayStation3.hpp"


u64 CellSpurs::cellSpursInitialize() {
    const u32 spurs_ptr = ARG0;
    const u32 n_spus = ARG1;
    const u32 spu_prio = ARG2;
    const u32 ppu_prio = ARG3;
    const u8 exit_if_no_work = ARG4;
    log("cellSpursInitialize(spurs_ptr: 0x%08x, n_spus: %d, spu_prio: %d, ppu_prio: %d, exit_if_no_work: %d) UNIMPLEMENTED\n", spurs_ptr, n_spus, spu_prio, ppu_prio, exit_if_no_work);


    return Result::CELL_OK;
}