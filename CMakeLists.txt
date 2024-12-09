cmake_minimum_required(VERSION 3.8)

project("ChonkyStation3")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(ChonkyStation3)
include_directories(ChonkyStation3/Common)
include_directories(ChonkyStation3/Loaders)
include_directories(ChonkyStation3/Memory)
include_directories(ChonkyStation3/PPU)
include_directories(ChonkyStation3/OS)

include_directories(Dependencies/ELFIO)
include_directories(Dependencies/Dolphin)

add_executable(ChonkyStation3 "ChonkyStation3/ChonkyStation3.cpp" "ChonkyStation3/ChonkyStation3.hpp" "ChonkyStation3/Loaders/ELF/ELFLoader.hpp" "ChonkyStation3/Loaders/ELF/ELFLoader.cpp" "ChonkyStation3/Common/common.hpp" "ChonkyStation3/PlayStation3.hpp" "ChonkyStation3/PlayStation3.cpp" "ChonkyStation3/Memory/Memory.cpp" "ChonkyStation3/Memory/Memory.hpp" "ChonkyStation3/Common/BEField.hpp" "ChonkyStation3/PPU/PPU.cpp" "ChonkyStation3/PPU/PPU.hpp" "ChonkyStation3/PPU/Backends/PPUInterpreter.hpp" "ChonkyStation3/PPU/Backends/PPUInterpreter.cpp" "Dependencies/Dolphin/BitField.hpp" "ChonkyStation3/PPU/PPUDisassembler.hpp" "ChonkyStation3/PPU/PPUTypes.hpp" "ChonkyStation3/PPU/PPUDisassembler.cpp" "ChonkyStation3/OS/ModuleManager.cpp" "ChonkyStation3/OS/ModuleManager.hpp" "ChonkyStation3/OS/Module.hpp" "ChonkyStation3/OS/Syscall.hpp" "ChonkyStation3/OS/Syscall.cpp" "ChonkyStation3/OS/Modules/SysPrxForUser.hpp" "ChonkyStation3/OS/Thread.hpp" "ChonkyStation3/OS/Thread.cpp" "ChonkyStation3/OS/ThreadManager.hpp" "ChonkyStation3/OS/ThreadManager.cpp" "ChonkyStation3/Common/MemoryConstants.hpp")
