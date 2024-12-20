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

enum CELL_FS_S : u32 {
    CELL_FS_S_IFDIR = 0040000,	// Directory
    CELL_FS_S_IFREG = 0100000,	// Regular
    CELL_FS_S_IFLNK = 0120000,	// Symbolic link
    CELL_FS_S_IFWHT = 0160000,	// Unknown

    CELL_FS_S_IRUSR = 0000400,	// R for owner
    CELL_FS_S_IWUSR = 0000200,	// W for owner
    CELL_FS_S_IXUSR = 0000100,	// X for owner

    CELL_FS_S_IRGRP = 0000040,	// R for group
    CELL_FS_S_IWGRP = 0000020,	// W for group
    CELL_FS_S_IXGRP = 0000010,	// X for group

    CELL_FS_S_IROTH = 0000004,	// R for other
    CELL_FS_S_IWOTH = 0000002,	// W for other
    CELL_FS_S_IXOTH = 0000001,	// X for other
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