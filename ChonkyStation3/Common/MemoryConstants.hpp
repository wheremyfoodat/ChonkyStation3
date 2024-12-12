#pragma once


constexpr u64 PAGE_SIZE = 0x1000;
constexpr u64 PAGE_MASK = PAGE_SIZE - 1;
constexpr u64 RAM_SIZE = 256_MB;
constexpr u64 RAM_START = 0x00010000;
constexpr u64 RAM_END = RAM_START + RAM_SIZE;
constexpr u64 STACK_REGION_START = 0xD0000000;
constexpr u64 STACK_REGION_SIZE = 0x10000000;
constexpr u64 DEFAULT_STACK_SIZE = 0x10000;
constexpr u64 TLS_REGION_START = 0x20000000;    // ?