#pragma once

#include <common.hpp>
#include <BEField.hpp>


#define ARG0 ps3->ppu->state.gprs[3]
#define ARG1 ps3->ppu->state.gprs[4]
#define ARG2 ps3->ppu->state.gprs[5]
#define ARG3 ps3->ppu->state.gprs[6]
#define ARG4 ps3->ppu->state.gprs[7]
#define ARG5 ps3->ppu->state.gprs[8]
#define ARG6 ps3->ppu->state.gprs[9]
#define ARG7 ps3->ppu->state.gprs[10]

namespace CellTypes {

enum Result : u32 {
    CELL_OK     = 0x00000000,
    CELL_CANCEL = 0x00000001,
    CELL_EAGAIN = 0x80010001,
    CELL_EINVAL = 0x80010002,
    CELL_ENOSYS = 0x80010003,
    CELL_ENOMEM = 0x80010004,
    CELL_ENOENT = 0x80010006,
    CELL_EPERM  = 0x80010009,
    CELL_EBUSY  = 0x8001000A,
    CELL_EFAULT = 0x8001000D,
    CELL_EALIGN = 0x80010010,
    CELL_BADF   = 0x8001002A,
    CELL_EIO    = 0x8001002B,
};

struct CellFsStat {
    BEField<u32> mode;
    BEField<s32> uid;
    BEField<s32> gid;
    BEField<s64> atime;
    BEField<s64> mtime;
    BEField<s64> ctime;
    BEField<u64> size;
    BEField<u64> blksize;
};

}   // End namespce OSTypes