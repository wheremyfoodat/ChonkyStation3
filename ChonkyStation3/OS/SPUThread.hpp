#pragma once

#include <common.hpp>
#include <logger.hpp>

#include <SPUTypes.hpp>
#include <OS/Syscalls/sys_spu.hpp>


using namespace SPUTypes;
using namespace sys_spu;

// Circular dependency
class PlayStation3;

class SPUThread {
public:
    SPUThread(PlayStation3* ps3, std::string name);
    PlayStation3* ps3;

    SPUTypes::State state;
    u8* ls = new u8[256_KB];

    u32 id;
    std::string name;

    enum class ThreadStatus {
        Ready,
        Running,
        Waiting,
        Terminated
    };
    ThreadStatus status = ThreadStatus::Ready;

    void loadImage(sys_spu_image* img);
    void halt();

    // MFC
    enum MFC_Channel: u32 {
        MFC_WrMSSyncReq     = 9,
        MFC_RdTagMask       = 12,
        MFC_LSA             = 16,
        MFC_EAH             = 17, 
        MFC_EAL             = 18,
        MFC_Size            = 19,
        MFC_TagID           = 20,
        MFC_Cmd             = 21,
        MFC_WrTagMask       = 22,
        MFC_WrTagUpdate     = 23,
        MFC_RdTagStat       = 24,
        MFC_RdListStallStat = 25,
        MFC_WrListStallAck  = 26,
        MFC_RdAtomicStat    = 27,
    };

    enum MFC_Command : u32 {
        GET     = 0x40,
        PUTLLC  = 0xb4,
        GETLLAR = 0xd0,
    };

    u32 lsa = 0;
    u32 eah = 0;
    u32 eal = 0;
    u32 size = 0;
    u32 tag_id = 0;

    std::string channelToString(u32 ch);
    u32  readChannel(u32 ch);
    void writeChannel(u32 ch, u32 val);
    void doCmd(u32 cmd);

private:
    MAKE_LOG_FUNCTION(log, thread_spu);
};