#include "CellRtc.hpp"
#include "PlayStation3.hpp"


u64 CellRtc::cellRtcGetCurrentClockLocalTime() {
    const u32 clock_ptr = ARG0;
    log("cellRtcGetCurrentClockLocalTime(clock_ptr: 0x%08x)\n");

    CellRtcDateTime* clock = (CellRtcDateTime*)ps3->mem.getPtr(clock_ptr);
    const auto time = std::chrono::system_clock::now();
    const auto local_time = std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now()).get_local_time();
    const auto days = std::chrono::floor<std::chrono::days>(local_time);
    const std::chrono::year_month_day ymd(days);
    const std::chrono::hh_mm_ss hms(std::chrono::floor<std::chrono::milliseconds>(local_time - days));
    clock->day = (u32)ymd.day();
    clock->month = (u32)ymd.month();
    clock->year = (s32)ymd.year();
    clock->hour = hms.hours().count();
    clock->minute = hms.minutes().count();
    clock->second = hms.seconds().count();
    clock->microsecond = 0; // TODO

    log("Time is %d/%d/%d %d:%d:%d:%d\n", (u32)clock->day, (u32)clock->month, (u32)clock->year, (u32)clock->hour, (u32)clock->minute, (u32)clock->second, (u32)clock->microsecond);
    return Result::CELL_OK;
}

u64 CellRtc::cellRtcGetCurrentTick() {
    const u32 tick_ptr = ARG0;
    log("cellRtcGetCurrentTick(tick_ptr: 0x%08x)\n", tick_ptr);

    ps3->mem.write<u64>(tick_ptr, ps3->scheduler.time);
    
    return Result::CELL_OK;
}

u64 CellRtc::cellRtcGetTime_t() {
    const u32 clock_ptr = ARG0;
    const u32 time_ptr = ARG1;  // time is s64

    CellRtcDateTime* clock = (CellRtcDateTime*)ps3->mem.getPtr(clock_ptr);
    // TODO: This is an approximation
    // We are supposed to turn clock into a time_t, but because this function is probably going to be called
    // right after getting the current time, we can just get the current tick again here with std::chrono
    ps3->mem.write<u64>(time_ptr, std::chrono::system_clock::now().time_since_epoch().count() / 1000000);

    return Result::CELL_OK;
}