#include "SysThread.hpp"
#include "PlayStation3.hpp"


u64 SysThread::sysThreadGetID() {
    u32 ptr = ARG0;
    printf("sysThreadGetID(ptr: 0x%08x) [thread_id = 0x%08x]\n", ptr, ps3->thread_manager.current_thread->id);

    ps3->mem.write<u64>(ptr, ps3->thread_manager.current_thread->id);
    return Result::CELL_OK;
}

// Allocate TLS memory, copy TLS image to the newly allocated area. Returns TLS address in R13
u64 SysThread::sysThreadInitializeTLS() {
    const u64 thread_id = ARG0;
    const u32 tls_seg_addr = ARG1;
    const u32 tls_seg_size = ARG2;
    const u32 tls_mem_size = ARG3;
    printf("sysThreadInitializeTLS(thread_id: %lld, tls_seg_addr: 0x%08x, tls_seg_size: 0x%08x, tls_mem_size: 0x%08x)", thread_id, tls_seg_addr, tls_seg_size, tls_mem_size);

    // Was TLS already initialized?
    if (ps3->ppu->state.gprs[13] != 0) {
        printf(" [TLS was already initialized]\n");
        return Result::CELL_OK;
    }
    putc('\n', stdout);

    initializeTLS(thread_id, tls_seg_addr, tls_seg_size, tls_mem_size, ps3->ppu->state);
    return Result::CELL_OK;
}

void SysThread::initializeTLS(u64 thread_id, u32 tls_seg_addr, u32 tls_seg_size, u32 tls_mem_size, State& state) {
    // Allocate TLS memory
    const u32 tls_addr = ps3->thread_manager.allocTLS(tls_mem_size);
    // Copy TLS image to allocated memory
    const u8* tls_image_ptr = ps3->mem.getPtr(tls_seg_addr);
    u8* tls_area_ptr = ps3->mem.getPtr(tls_addr);
    std::memcpy(tls_area_ptr, tls_image_ptr, tls_seg_size);

    state.gprs[13] = tls_addr + 0x7000; // TODO: figure out why you have to add 0x7000... but stuff breaks without it
}