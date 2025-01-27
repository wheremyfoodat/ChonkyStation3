#pragma once

#include <common.hpp>

#include <unordered_map>
#include <format>

#include <Import.hpp>
#include <PRX/PRXExport.hpp>

#include <Modules/SysPrxForUser.hpp>
#include <Modules/SysThread.hpp>
#include <Modules/SysLwMutex.hpp>
#include <Modules/SysMMapper.hpp>
#include <Modules/CellGcmSys.hpp>
#include <Modules/CellVideoOut.hpp>
#include <Modules/CellSysutil.hpp>
#include <Modules/CellSysmodule.hpp>
#include <Modules/CellResc.hpp>
#include <Modules/CellGame.hpp>
#include <Modules/CellSpurs.hpp>
#include <Modules/CellRtc.hpp>
#include <Modules/CellFs.hpp>
#include <Modules/CellPngDec.hpp>
#include <Modules/SceNpTrophy.hpp>
#include <Modules/CellSaveData.hpp>


// Circular dependency
class PlayStation3;

class ModuleManager {
public:
    ModuleManager(PlayStation3* ps3) :  ps3(ps3), sysPrxForUser(ps3), sysThread(ps3), sysLwMutex(ps3), sysMMapper(ps3), cellGcmSys(ps3), cellVideoOut(ps3), cellSysutil(ps3),
                                        cellSysmodule(ps3), cellResc(ps3), cellGame(ps3), cellSpurs(ps3), cellRtc(ps3), cellFs(ps3), cellPngDec(ps3), sceNpTrophy(ps3),
                                        cellSaveData(ps3) {}
    PlayStation3* ps3;

    void call(u32 nid);
    void lle(u32 nid);
    // Map address to import nid
    void registerImport(u32 addr, u32 nid);
    void registerExportTable(const PRXExportTable& exports);
    PRXExportTable getExportTable() { return exports; }
    std::unordered_map<u32, u32> imports = {};
    PRXExportTable exports;

    std::unordered_map<u32, Import> import_map {
        { 0xe6f2c1e7, { "sysProcessExit",                               std::bind(&SysPrxForUser::sysProcessExit, &sysPrxForUser) }},
        { 0x2c847572, { "sysProcessAtExitSpawn",                        std::bind(&SysPrxForUser::sysProcessAtExitSpawn, &sysPrxForUser) }},
        { 0x2d36462b, { "_sys_strlen",                                  std::bind(&SysPrxForUser::sysStrlen, &sysPrxForUser) }},
        { 0x8461e528, { "sysGetSystemTime",                             std::bind(&SysPrxForUser::sysGetSystemTime, &sysPrxForUser) }},
        { 0x96328741, { "sysProcess_At_ExitSpawn",                      std::bind(&SysPrxForUser::sysProcess_At_ExitSpawn, &sysPrxForUser) }},
        { 0x5267cb35, { "sysSpinlockUnlock",                            std::bind(&SysPrxForUser::sysSpinlockUnlock, &sysPrxForUser) }},
        { 0x8c2bb498, { "sysSpinlockInitialize",                        std::bind(&SysPrxForUser::sysSpinlockInitialize, &sysPrxForUser) }},
        { 0x99c88692, { "_sys_strcpy",                                  std::bind(&SysPrxForUser::sysStrcpy, &sysPrxForUser) }},
        { 0xa285139d, { "sysSpinlockLock",                              std::bind(&SysPrxForUser::sysSpinlockLock, &sysPrxForUser) }},
        { 0x68b9b011, { "_sys_memset",                                  std::bind(&SysPrxForUser::sysMemset, &sysPrxForUser) }},
        { 0x6bf66ea7, { "_sys_memcpy",                                  std::bind(&SysPrxForUser::sysMemcpy, &sysPrxForUser) }},
        { 0xfb5db080, { "_sys_memcmp",                                  std::bind(&SysPrxForUser::sysMemcmp, &sysPrxForUser) }},

        { 0x1573dc3f, { "sysLwMutexLock",                               std::bind(&SysLwMutex::sysLwMutexLock, &sysLwMutex) }},
        { 0x1bc200f4, { "sysLwMutexUnlock",                             std::bind(&SysLwMutex::sysLwMutexUnlock, &sysLwMutex) }},
        { 0x2f85c0ef, { "sysLwMutexCreate",                             std::bind(&SysLwMutex::sysLwMutexCreate, &sysLwMutex) }},
        { 0xc3476d0c, { "sysLwMutexDestroy",                            std::bind(&SysLwMutex::sysLwMutexDestroy, &sysLwMutex) }},

        { 0x24a1ea07, { "sysPPUThreadCreate",                           std::bind(&SysThread::sysPPUThreadCreate, &sysThread) }},
        { 0x350d454e, { "sysPPUThreadGetID",                            std::bind(&SysThread::sysPPUThreadGetID, &sysThread) }},
        { 0x744680a2, { "sysPPUThreadInitializeTLS",                    std::bind(&SysThread::sysPPUThreadInitializeTLS, &sysThread) }},
        { 0xaff080a4, { "sysPPUThreadExit",                             std::bind(&SysThread::sysPPUThreadExit, &sysThread) }},

        { 0x409ad939, { "sysMMapperFreeMemory",                         std::bind(&SysMMapper::sysMMapperFreeMemory, &sysMMapper) }},
        { 0x4643ba6e, { "sysMMapperUnmapMemory",                        std::bind(&SysMMapper::sysMMapperUnmapMemory, &sysMMapper) }},
        { 0xb257540b, { "sysMMapperAllocateMemory",                     std::bind(&SysMMapper::sysMMapperAllocateMemory, &sysMMapper) }},

        { 0x055bd74d, { "cellGcmGetTiledPitchSize",                     std::bind(&CellGcmSys::cellGcmGetTiledPitchSize, &cellGcmSys) }},
        { 0x15bae46b, { "cellGcmInitBody",                              std::bind(&CellGcmSys::cellGcmInitBody, &cellGcmSys) }},
        { 0x21ac3697, { "cellGcmAddressToOffset",                       std::bind(&CellGcmSys::cellGcmAddressToOffset, &cellGcmSys) }},
        { 0x4524cccd, { "cellGcmBindTile",                              std::bind(&CellGcmSys::cellGcmBindTile, &cellGcmSys) }},
        { 0x4ae8d215, { "cellGcmSetFlipMode",                           std::bind(&CellGcmSys::cellGcmSetFlipMode, &cellGcmSys) }},
        { 0x72a577ce, { "cellGcmGetFlipStatus",                         std::bind(&CellGcmSys::cellGcmGetFlipStatus, &cellGcmSys) }},
        { 0x983fb9aa, { "cellGcmSetWaitFlip",                           std::bind(&CellGcmSys::cellGcmSetWaitFlip, &cellGcmSys) }},
        { 0x9dc04436, { "cellGcmBindZcull",                             std::bind(&CellGcmSys::cellGcmBindZcull, &cellGcmSys) }},
        { 0xa114ec67, { "cellGcmMapMainMemory",                         std::bind(&CellGcmSys::cellGcmMapMainMemory, &cellGcmSys) }},
        { 0xa53d12ae, { "cellGcmSetDisplayBuffer",                      std::bind(&CellGcmSys::cellGcmSetDisplayBuffer, &cellGcmSys) }},
        { 0xa547adde, { "cellGcmGetControlRegister",                    std::bind(&CellGcmSys::cellGcmGetControlRegister, &cellGcmSys) }},
        { 0xb2e761d4, { "cellGcmResetFlipStatus",                       std::bind(&CellGcmSys::cellGcmResetFlipStatus, &cellGcmSys) }},
        { 0xbd100dbc, { "cellGcmSetTileInfo",                           std::bind(&CellGcmSys::cellGcmSetTileInfo, &cellGcmSys) }},
        { 0xdc09357e, { "cellGcmSetFlip",                               std::bind(&CellGcmSys::cellGcmSetFlip, &cellGcmSys) }},
        { 0xe315a0b2, { "cellGcmGetConfiguration",                      std::bind(&CellGcmSys::cellGcmGetConfiguration, &cellGcmSys) }},
        { 0xf80196c1, { "cellGcmGetLabelAddress",                       std::bind(&CellGcmSys::cellGcmGetLabelAddress, &cellGcmSys) }},

        { 0x0bae8772, { "cellVideoOutConfigure",                        std::bind(&CellVideoOut::cellVideoOutConfigure, &cellVideoOut) }},
        { 0x887572d5, { "cellVideoOutGetState",                         std::bind(&CellVideoOut::cellVideoOutGetState, &cellVideoOut) }},
        { 0xa322db75, { "cellVideoOutGetResolutionAvailability",        std::bind(&CellVideoOut::cellVideoOutGetResolutionAvailability, &cellVideoOut) }},
        { 0xe558748d, { "cellVideoOutGetResolution",                    std::bind(&CellVideoOut::cellVideoOutGetResolution, &cellVideoOut) }},

        { 0x02ff3c1b, { "cellSysutilUnregisterCallback",                std::bind(&CellSysutil::cellSysutilUnregisterCallback, &cellSysutil) }},
        { 0x189a74da, { "cellSysutilCheckCallback",                     std::bind(&CellSysutil::cellSysutilCheckCallback, &cellSysutil) }},
        { 0x40e895d3, { "cellSysutilGetSystemParamInt",                 std::bind(&CellSysutil::cellSysutilGetSystemParamInt, &cellSysutil) }},
        { 0x9d98afa0, { "cellSysutilRegisterCallback",                  std::bind(&CellSysutil::cellSysutilRegisterCallback, &cellSysutil) }},

        { 0x112a5ee9, { "cellSysmoduleUnloadModule",                    std::bind(&CellSysmodule::cellSysmoduleUnloadModule, &cellSysmodule) }},
        { 0x32267a31, { "cellSysmoduleLoadModule",                      std::bind(&CellSysmodule::cellSysmoduleLoadModule, &cellSysmodule) }},

        { 0x01220224, { "cellRescGcmSurface2RescSrc",                   std::bind(&ModuleManager::stub, this) }},
        { 0x0d3c22ce, { "cellRescSetWaitFlip",                          std::bind(&CellResc::cellRescSetWaitFlip, &cellResc) }},
        { 0x10db5b1a, { "cellRescSetDsts",                              std::bind(&CellResc::cellRescSetDsts, &cellResc) }},
        { 0x129922a0, { "cellRescResetFlipStatus",                      std::bind(&CellResc::cellRescResetFlipStatus, &cellResc) }},
        { 0x23134710, { "cellRescSetDisplayMode",                       std::bind(&CellResc::cellRescSetDisplayMode, &cellResc) }},
        { 0x25c107e6, { "cellRescSetConvertAndFlip",                    std::bind(&CellResc::cellRescSetConvertAndFlip, &cellResc) }},
        { 0x516ee89e, { "cellRescInit",                                 std::bind(&CellResc::cellRescInit, &cellResc) }},
        { 0x5a338cdb, { "cellRescGetBufferSize",                        std::bind(&CellResc::cellRescGetBufferSize, &cellResc) }},
        { 0x6cd0f95f, { "cellRescSetSrc",                               std::bind(&ModuleManager::stub, this) }},
        { 0x8107277c, { "cellRescSetBufferAddress",                     std::bind(&CellResc::cellRescSetBufferAddress, &cellResc) }},
        { 0xc47c5c22, { "cellRescGetFlipStatus",                        std::bind(&CellResc::cellRescGetFlipStatus, &cellResc) }},
        { 0xd1ca0503, { "cellRescVideoOutResolutionId2RescBufferMode",  std::bind(&CellResc::cellRescVideoOutResolutionId2RescBufferMode, &cellResc) }},

        { 0x7a0a83c4, { "cellFontInitLibraryFreeTypeWithRevision",      std::bind(&ModuleManager::stub, this) }},
        { 0xf03dcc29, { "cellFontInitializeWithRevision",               std::bind(&ModuleManager::stub, this) }},

        { 0x70acec67, { "cellGameContentPermit",                        std::bind(&CellGame::cellGameContentPermit, &cellGame) }},
        { 0xf52639ea, { "cellGameBootCheck",                            std::bind(&CellGame::cellGameBootCheck, &cellGame) }},

        { 0x52cc6c82, { "cellSpursCreateTaskset",                       std::bind(&CellSpurs::cellSpursCreateTaskset, &cellSpurs) }},
        { 0x5ef96465, { "_cellSpursEventFlagInitialize",                std::bind(&CellSpurs::_cellSpursEventFlagInitialize, &cellSpurs) }},
        { 0x87630976, { "cellSpursEventFlagAttachLv2EventQueue",        std::bind(&CellSpurs::cellSpursEventFlagAttachLv2EventQueue, &cellSpurs) }},
        { 0xacfc8dbc, { "cellSpursInitialize",                          std::bind(&CellSpurs::cellSpursInitialize, &cellSpurs) }},
        { 0xb9bc6207, { "cellSpursAttachLv2EventQueue",                 std::bind(&CellSpurs::cellSpursAttachLv2EventQueue, &cellSpurs) }},
        { 0xbeb600ac, { "cellSpursCreateTask",                          std::bind(&CellSpurs::cellSpursCreateTask, &cellSpurs) }},

        { 0x9dafc0d9, { "cellRtcGetCurrentTick",                        std::bind(&CellRtc::cellRtcGetCurrentTick, &cellRtc) }},

        { 0x2cb51f0d, { "cellFsClose",                                  std::bind(&CellFs::cellFsClose, &cellFs) }},
        { 0x4d5ff8e2, { "cellFsRead",                                   std::bind(&CellFs::cellFsRead, &cellFs) }},
        { 0x718bf5f8, { "cellFsOpen",                                   std::bind(&CellFs::cellFsOpen, &cellFs) }},
        { 0x7de6dced, { "cellFsStat",                                   std::bind(&CellFs::cellFsStat, &cellFs) }},
        { 0xa397d042, { "cellFsLseek",                                  std::bind(&CellFs::cellFsLseek, &cellFs) }},
        { 0xef3efa34, { "cellFsFstat",                                  std::bind(&CellFs::cellFsFstat, &cellFs) }},

        { 0x0b168f92, { "cellAudioInit",                                std::bind(&ModuleManager::stub, this) }},
        { 0x4692ab35, { "cellAudioOutConfigure",                        std::bind(&ModuleManager::stub, this) }},
        { 0x74a66af0, { "cellAudioGetPortConfig",                       std::bind(&ModuleManager::stub, this) }},
        { 0xc01b4e7c, { "cellAudioOutGetSoundAvailability",             std::bind(&ModuleManager::stub, this) }},
        { 0xcd7bc431, { "cellAudioPortOpen",                            std::bind(&ModuleManager::stub, this) }},
        { 0xf4e3caa0, { "cellAudioOutGetState",                         std::bind(&ModuleManager::stub, this) }},

        { 0x1cf98800, { "cellPadInit",                                  std::bind(&ModuleManager::stub, this) }},
        { 0xa703a51d, { "cellPadGetInfo2",                              std::bind(&ModuleManager::stub, this) }},

        { 0x32cf311f, { "sceNpScoreInit",                               std::bind(&ModuleManager::stub, this) }},
        { 0x4885aa18, { "sceNpTerm",                                    std::bind(&ModuleManager::stub, this) }},
        { 0x52a6b523, { "sceNpManagerUnregisterCallback",               std::bind(&ModuleManager::stub, this) }},
        { 0x9851f805, { "sceNpScoreTerm",                               std::bind(&ModuleManager::stub, this) }},
        { 0xa7bff757, { "sceNpManagerGetStatus",                        std::bind(&ModuleManager::stub, this) }},
        { 0xad218faf, { "sceNpDrmIsAvailable",                          std::bind(&ModuleManager::stub, this) }},
        { 0xbd28fdbf, { "sceNpInit",                                    std::bind(&ModuleManager::stub, this) }},
        { 0xe7dcd3b4, { "sceNpManagerRegisterCallback",                 std::bind(&ModuleManager::stub, this) }},
        
        { 0x105ee2cb, { "cellNetCtlTerm",                               std::bind(&ModuleManager::stub, this) }},
        { 0xbd5a59fc, { "cellNetCtlInit",                               std::bind(&ModuleManager::stub, this) }},

        { 0x157d30c5, { "cellPngDecCreate",                             std::bind(&CellPngDec::cellPngDecCreate, &cellPngDec) }},
        { 0x2310f155, { "cellPngDecDecodeData",                         std::bind(&CellPngDec::cellPngDecDecodeData, &cellPngDec) }},
        { 0x5b3d1ff1, { "cellPngDecClose",                              std::bind(&CellPngDec::cellPngDecClose, &cellPngDec) }},
        { 0x820dae1a, { "cellPngDecDestroy",                            std::bind(&CellPngDec::cellPngDecDestroy, &cellPngDec) }},
        { 0x9ccdcc95, { "cellPngDecReadHeader",                         std::bind(&CellPngDec::cellPngDecReadHeader, &cellPngDec) }},
        { 0xd2bc5bfd, { "cellPngDecOpen",                               std::bind(&CellPngDec::cellPngDecOpen, &cellPngDec) }},
        { 0xe97c9bd4, { "cellPngDecSetParameter",                       std::bind(&CellPngDec::cellPngDecSetParameter, &cellPngDec) }},

        { 0x1197b52c, { "sceNpTrophyRegisterContext",                   std::bind(&SceNpTrophy::sceNpTrophyRegisterContext, &sceNpTrophy) } },
        { 0x1c25470d, { "sceNpTrophyCreateHandle",                      std::bind(&SceNpTrophy::sceNpTrophyCreateHandle, &sceNpTrophy) } },
        { 0x370136fe, { "sceNpTrophyGetRequiredDiskSpace",              std::bind(&SceNpTrophy::sceNpTrophyGetRequiredDiskSpace, &sceNpTrophy) } },
        { 0x39567781, { "sceNpTrophyInit",                              std::bind(&ModuleManager::stub, this) } },
        { 0x49d18217, { "sceNpTrophyGetGameInfo",                       std::bind(&SceNpTrophy::sceNpTrophyGetGameInfo, &sceNpTrophy) } },
        { 0xb3ac3478, { "sceNpTrophyGetTrophyUnlockState",              std::bind(&SceNpTrophy::sceNpTrophyGetTrophyUnlockState, &sceNpTrophy) } },
        { 0xe3bf9a28, { "sceNpTrophyCreateContext",                     std::bind(&SceNpTrophy::sceNpTrophyCreateContext, &sceNpTrophy) } },
        
        { 0xfbd5c856, { "cellSaveDataAutoLoad2",                        std::bind(&CellSaveData::cellSaveDataAutoLoad2, &cellSaveData) } },
    };

    std::string getImportName(const u32 nid);
    std::string last_call;

    SysPrxForUser sysPrxForUser;
    SysThread sysThread;
    SysLwMutex sysLwMutex;
    SysMMapper sysMMapper;
    CellGcmSys cellGcmSys;
    CellVideoOut cellVideoOut;
    CellSysutil cellSysutil;
    CellSysmodule cellSysmodule;
    CellResc cellResc;
    CellGame cellGame;
    CellSpurs cellSpurs;
    CellRtc cellRtc;
    CellFs cellFs;
    CellPngDec cellPngDec;
    SceNpTrophy sceNpTrophy;
    CellSaveData cellSaveData;

    u64 stub() {
        unimpl("%s UNIMPLEMENTED\n", last_call.c_str());
        return Result::CELL_OK;
    }

    u32 last_lle_nid = 0;
    void printReturnValue();

private:
    MAKE_LOG_FUNCTION(log, lle_module);
    MAKE_LOG_FUNCTION(unimpl, unimplemented);
};