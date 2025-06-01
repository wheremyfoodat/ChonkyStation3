#include "SceNp.hpp"
#include "PlayStation3.hpp"


u64 SceNp::sceNpDrmVerifyUpgradeLicense() {
    log("sceNpDrmVerifyUpgradeLicense()\n");
    
    //return SCE_NP_DRM_ERROR_LICENSE_NOT_FOUND;
    return CELL_OK;
}

u64 SceNp::sceNpBasicGetEvent() {
    //log("sceNpBasicGetEvent() STUBBED\n");

    return SCE_NP_BASIC_ERROR_NO_EVENT;
}

u64 SceNp::sceNpManagerGetNpId() {
    log("sceNpManagerGetNpId() STUBBED\n");

    return SCE_NP_ERROR_OFFLINE;
}
