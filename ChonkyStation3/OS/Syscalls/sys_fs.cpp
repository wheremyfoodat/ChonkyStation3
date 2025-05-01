#include <Syscall.hpp>
#include "PlayStation3.hpp"


MAKE_LOG_FUNCTION(log_sys_fs, sys_fs);

u64 Syscall::sys_fs_test() {
    const u32 arg0 = ARG0;
    const u32 arg1 = ARG1;
    const u32 arg2 = ARG2;
    const u32 arg3 = ARG3;
    const u32 buf_ptr = ARG4;
    const u32 buf_size = ARG5;
    log_sys_fs("sys_fs_test(arg0: %d, arg1: %d, arg2: 0x%08x, arg3: %d, buf_ptr: 0x%08x, buf_size: %d)\n", arg0, arg1, arg2, arg3, buf_ptr, buf_size);

    const u32 fd = ps3->mem.read<u32>(arg2);
    auto file = ps3->fs.getFileFromID(fd);

    std::memset((char*)ps3->mem.getPtr(buf_ptr), 0, buf_size);
    std::strncpy((char*)ps3->mem.getPtr(buf_ptr), file.guest_path.generic_string().c_str(), buf_size);

    return CELL_OK;
}

u64 Syscall::sys_fs_fcntl() {
    const u32 fd = ARG0;
    const u32 op = ARG1;
    const u32 arg_ptr = ARG2;
    const u32 size = ARG3;
    log_sys_fs("sys_fs_fcntl(fd: %d, op: 0x%08x, arg_ptr: 0x%08x, size: %d) UNIMPLEMENTED\n", fd, op, arg_ptr, size);

    return CELL_OK;
}