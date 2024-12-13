#include <Syscall.hpp>
#include "PlayStation3.hpp"


u64 Syscall::sysMMapperAllocateAddress() {
    printf("sysMMapperAllocateAddress() UNIMPLEMENTED\n");
    // TODO: I don't know what this does
    return Result::CELL_OK;
}