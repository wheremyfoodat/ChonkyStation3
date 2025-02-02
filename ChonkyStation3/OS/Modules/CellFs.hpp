#pragma once

#include <common.hpp>
#include <logger.hpp>
#include <BEField.hpp>

#include <CellTypes.hpp>

#include <unordered_map>


// Circular dependency
class PlayStation3;

using namespace CellTypes;

static constexpr u64 CELL_ENOTMOUNTED = 0x8001003A;

class CellFs {
public:
    CellFs(PlayStation3* ps3) : ps3(ps3) {}
    PlayStation3* ps3;

#pragma pack(push, 1)
    struct CellFsStat {
        BEField<u32> mode;
        BEField<u32> uid;
        BEField<u32> gid;
        BEField<u64> atime;
        BEField<u64> mtime;
        BEField<u64> ctime;
        BEField<u64> size;
        BEField<u64> blksize;
    };
#pragma pack(pop)

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

    u64 cellFsClose();
    u64 cellFsRead();
    u64 cellFsOpen();
    u64 cellFsStat();
    u64 cellFsLseek();
    u64 cellFsFstat();

private:
    MAKE_LOG_FUNCTION(log, cellFs);
};