#include "SceNpTrophy.hpp"
#include "PlayStation3.hpp"


u64 SceNpTrophy::sceNpTrophyCreateHandle() {
    const u32 handle_ptr = ARG0;
    log("sceNpTrophyCreateHandle(handle_ptr: 0x%08x)\n");

    ps3->mem.write<u32>(handle_ptr, ps3->handle_manager.request());

    return Result::CELL_OK;
}

u64 SceNpTrophy::sceNpTrophyCreateContext() {
    const u32 ctx_ptr = ARG0;
    const u32 comm_id_ptr = ARG1;
    const u32 comm_sign_ptr = ARG2;
    const u64 options = ARG3;
    log("sceNpTrophyCreateContext(ctx_ptr: 0x%08x, comm_id_ptr: 0x%08x, comm_sign_ptr: 0x%08x, options: 0x%016x)\n", ctx_ptr, comm_id_ptr, comm_sign_ptr, options);

    ps3->mem.write<u32>(ctx_ptr, ps3->handle_manager.request());

    return Result::CELL_OK;
}