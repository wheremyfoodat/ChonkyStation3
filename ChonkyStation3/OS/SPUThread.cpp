#include "SPUThread.hpp"
#include "PlayStation3.hpp"


SPUThread::SPUThread(PlayStation3* ps3, std::string name) : ps3(ps3) {
    id = ps3->handle_manager.request();
    this->name = name;
    log("Created SPU thread %d \"%s\"\n", id, name.c_str());
}

void SPUThread::loadImage(sys_spu_image* img) {
    log("Importing SPU image for thread %d \"%s\"\n", id, name.c_str());
    // Copy SPU image to LS
    sys_spu_segment* segs = (sys_spu_segment*)ps3->mem.getPtr(img->segs_ptr);
    for (int i = 0; i < img->n_segs; i++) {
        switch (segs[i].type) {

        case SYS_SPU_SEGMENT_TYPE_COPY: {
            log("*  Loading segment %d type COPY\n", i);
            std::memcpy(&ls[segs[i].ls_addr], ps3->mem.getPtr(segs[i].src.addr), segs[i].size);
            log("*  Copied segment from main[0x%08x - 0x%08x] to ls[0x%08x - 0x%08x]\n", (u32)segs[i].src.addr, segs[i].src.addr + segs[i].size, (u32)segs[i].ls_addr, segs[i].ls_addr + segs[i].size);
            break;
        }

        default: {
            Helpers::panic("Unimplemented segment type %d\n", (u32)segs[i].type);
        }
        }
    }

    log("Entry point: 0x%08x\n", (u32)img->entry);
    state.pc = img->entry;
}