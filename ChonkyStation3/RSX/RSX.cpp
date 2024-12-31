#include "RSX.hpp"
#include "PlayStation3.hpp"


RSX::RSX(PlayStation3* ps3) : ps3(ps3), gcm(ps3->module_manager.cellGcmSys) {

}

u32 RSX::fetch32() {
    u32 data = ps3->mem.read<u32>(gcm.gcm_config.io_addr + curr_cmd);
    curr_cmd += 4;
    gcm.ctrl->get = gcm.ctrl->get + 4;  // Didn't overload += in BEField
    return data;
}

void RSX::runCommandList() {
    auto cmd_count = gcm.ctrl->put - gcm.ctrl->get;
    if (cmd_count <= 0) return;

    printf("Executing commands (%d bytes)\n", cmd_count);

    // Execute while get < put
    curr_cmd = 0;
    while (gcm.ctrl->get < gcm.ctrl->put) {
        u32 cmd = fetch32();
        const auto cmd_num = cmd & 0x3ffff;
        const auto argc = (cmd >> 18) & 0x7ff;

        std::vector<u32> args;
        for (int i = 0; i < argc; i++)
            args.push_back(fetch32());

        if (command_names.contains(cmd_num))
            printf("%s\n", command_names[cmd_num].c_str());

        switch (cmd & 0x3ffff) {

        case NV4097_SET_SEMAPHORE_OFFSET: {
            break;
        }

        default:
            if (command_names.contains(cmd & 0x3ffff))
                Helpers::panic("Unknown RSX command %s\n", command_names[cmd_num].c_str());
            else
                Helpers::panic("Unknown RSX command 0x%08x\n", cmd_num);
        }
    }
}