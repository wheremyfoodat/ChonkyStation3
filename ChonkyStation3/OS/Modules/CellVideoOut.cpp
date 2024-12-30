#include "CellVideoOut.hpp"
#include "PlayStation3.hpp"


u64 CellVideoOut::cellVideoOutConfigure() {
    const u32 video_out = ARG0;
    const u32 config_ptr = ARG1;
    const u32 option_ptr = ARG2;
    const u32 wait_for_event = ARG3;
    printf("cellVideoOutConfigure(video_out: 0x%08x, config_ptr: 0x%08x, option_ptr: 0x%08x, wait_for_event: 0x%08x)\n", video_out, config_ptr, option_ptr, wait_for_event);
    // TODO: supposedly this enables VSYNC

    CellVideoOutConfiguration* config = (CellVideoOutConfiguration*)ps3->mem.getPtr(config_ptr);
    video_config.res_id = config->res_id;
    video_config.format = config->format;
    video_config.aspect = config->aspect;
    video_config.pitch = config->pitch;

    return Result::CELL_OK;
}

u64 CellVideoOut::cellVideoOutGetState() {
    const u32 video_out = ARG0;
    const u32 device_idx = ARG1;
    const u32 state_ptr = ARG2;
    printf("cellVideoOutGetState(video_out: 0x%08x, device_idx: 0x%08x, state_ptr: 0x%08x)\n", video_out, device_idx, state_ptr);

    CellVideoOutState* state = (CellVideoOutState*)ps3->mem.getPtr(state_ptr);
    state->state = CELL_VIDEO_OUT_OUTPUT_STATE_ENABLED;
    state->color_space = CELL_VIDEO_OUT_COLOR_SPACE_RGB;
    state->display_mode.res_id = video_config.res_id;
    state->display_mode.scan_mode = CELL_VIDEO_OUT_SCAN_MODE_PROGRESSIVE;
    state->display_mode.conversion = CELL_VIDEO_OUT_DISPLAY_CONVERSION_NONE;
    // TODO: If the aspect ratio mode is set to AUTO, we are supposed to figure out what the correct aspect ratio is
    // from the given resolution. For now just pretend everything is 16:9
    state->display_mode.aspect = (video_config.aspect != CELL_VIDEO_OUT_ASPECT_AUTO) ? video_config.aspect : CELL_VIDEO_OUT_ASPECT_16_9;
    state->display_mode.refresh_rates = CELL_VIDEO_OUT_REFRESH_RATE_59_94HZ;

    return Result::CELL_OK;
}

u64 CellVideoOut::cellVideoOutGetResolutionAvailability() {
    const u32 video_out = ARG0;
    const u32 res_id = ARG1;
    const u32 aspect = ARG2;
    const u32 option = ARG3;
    printf("cellVideoOutGetResolutionAvailability(video_out: 0x%08x, res_id: 0x%08x, aspect: 0x%08x, option: 0x%08x) STUBBED\n", video_out, res_id, aspect, option);

    // TODO: implement this
    // 1 means resolution is available
    return 1;
}

u64 CellVideoOut::cellVideoOutGetResolution() {
    const u32 res_id = ARG0;
    const u32 res_ptr = ARG1;
    printf("cellVideoOutGetResolution(res_id: 0x%08x, res_ptr: 0x%08x)\n", res_id, res_ptr);

    if (resolutions.find(res_id) == resolutions.end())
        Helpers::panic("Unknown resolution ID\n");

    const auto res_ = resolutions[res_id];
    CellVideoOutResolution* res = (CellVideoOutResolution*)ps3->mem.getPtr(res_ptr);
    res->width = res_.width;
    res->height = res_.height;

    return Result::CELL_OK;
}