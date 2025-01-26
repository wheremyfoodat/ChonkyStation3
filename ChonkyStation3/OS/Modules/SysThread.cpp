#include "SysThread.hpp"
#include "PlayStation3.hpp"


u64 SysThread::sysPPUThreadCreate() {
    const u32 thread_id_ptr = ARG0; // thread_id is u64
    const u32 entry = ARG1;
    const u64 arg = ARG2;
    const s32 prio = ARG3;
    const u32 stack_size = ARG4;
    const u64 flags = ARG5;
    const u32 thread_name_ptr = ARG6;
    log("sysPPUThreadCreate(thread_id_ptr: 0x%08x, entry: 0x%08x, arg: 0x%016llx, prio: %d, stack_size: 0x%08x, flags: 0x%016llx, thread_name_ptr: 0x%08x)\n", thread_id_ptr, entry, arg, prio, stack_size, flags, thread_name_ptr);

    const std::string name = Helpers::readString(ps3->mem.getPtr(thread_name_ptr));
    Thread* thread = ps3->thread_manager.createThread(entry, stack_size, arg, (const u8*)name.c_str(), 0, 0, 0);
    // HACK: sleep spu threads
    if (thread->name == "spu_printf_handler"
        || thread->name == "soundmain")
        thread->status = Thread::THREAD_STATUS::Sleeping;

    ps3->mem.write<u32>(thread_id_ptr, thread->id);

    return Result::CELL_OK;
}

u64 SysThread::sysPPUThreadGetID() {
    const u32 ptr = ARG0;
    Thread* current_thread = ps3->thread_manager.getCurrentThread();
    log("sysThreadGetID(ptr: 0x%08x) [thread_id = 0x%08x]\n", ptr, current_thread->id);

    ps3->mem.write<u64>(ptr, current_thread->id);
    return Result::CELL_OK;
}

// Allocate TLS memory, copy TLS image to the newly allocated area. Returns TLS address in R13
u64 SysThread::sysPPUThreadInitializeTLS() {
    const u64 thread_id = ARG0;
    const u32 tls_seg_addr = ARG1;
    const u32 tls_seg_size = ARG2;
    const u32 tls_mem_size = ARG3;
    log("sysThreadInitializeTLS(thread_id: %lld, tls_seg_addr: 0x%08x, tls_seg_size: 0x%08x, tls_mem_size: 0x%08x)", thread_id, tls_seg_addr, tls_seg_size, tls_mem_size);

    // Was TLS already initialized?
    if (ps3->ppu->state.gprs[13] != 0) {
        logNoPrefix(" [TLS was already initialized]\n");
        return Result::CELL_OK;
    }
    putc('\n', stdout);

    initializeTLS(thread_id, tls_seg_addr, tls_seg_size, tls_mem_size, ps3->ppu->state);
    return Result::CELL_OK;
}

void SysThread::initializeTLS(u64 thread_id, u32 tls_seg_addr, u32 tls_seg_size, u32 tls_mem_size, State& state) {
    // Allocate TLS memory
    const u32 tls_addr = ps3->thread_manager.allocTLS(tls_mem_size);
    const u8* tls_image_ptr = ps3->mem.getPtr(tls_seg_addr);
    u8* tls_area_ptr = ps3->mem.getPtr(tls_addr);
    // Clear system area
    std::memset(tls_area_ptr, 0, 0x30);
    // Copy TLS image to allocated memory
    std::memcpy(tls_area_ptr + 0x30, tls_image_ptr, tls_seg_size);
    // Set the remaining memory to 0
    std::memset(tls_area_ptr + 0x30 + tls_seg_size, 0, tls_mem_size - tls_seg_size);

    state.gprs[13] = tls_addr + 0x7000 + 0x30; // TODO: figure out why you have to add 0x7000... but stuff breaks without it. I know that 0x30 is because the first 0x30 are some sort of reserved system area.
}