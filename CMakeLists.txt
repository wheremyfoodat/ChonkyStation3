cmake_minimum_required(VERSION 3.8)

project("ChonkyStation3")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_definitions(SDL_MAIN_HANDLED)

add_subdirectory(Dependencies/OpenGL)
add_subdirectory(Dependencies/SDL)
add_subdirectory(Dependencies/xxHash/cmake_unofficial EXCLUDE_FROM_ALL)

add_executable(ChonkyStation3)
target_sources(ChonkyStation3 PRIVATE "ChonkyStation3/ChonkyStation3.cpp"  "ChonkyStation3/Loaders/ELF/ELFLoader.hpp" "ChonkyStation3/Loaders/ELF/ELFLoader.cpp" "ChonkyStation3/Common/common.hpp" "ChonkyStation3/PlayStation3.hpp" "ChonkyStation3/PlayStation3.cpp" "ChonkyStation3/Memory/Memory.cpp" "ChonkyStation3/Memory/Memory.hpp" "ChonkyStation3/Common/BEField.hpp" "ChonkyStation3/PPU/PPU.cpp" "ChonkyStation3/PPU/PPU.hpp" "ChonkyStation3/PPU/Backends/PPUInterpreter.hpp" "ChonkyStation3/PPU/Backends/PPUInterpreter.cpp" "Dependencies/Dolphin/BitField.hpp" "ChonkyStation3/PPU/PPUDisassembler.hpp" "ChonkyStation3/PPU/PPUTypes.hpp" "ChonkyStation3/PPU/PPUDisassembler.cpp" "ChonkyStation3/OS/ModuleManager.cpp" "ChonkyStation3/OS/ModuleManager.hpp"  "ChonkyStation3/OS/Syscall.hpp" "ChonkyStation3/OS/Syscall.cpp" "ChonkyStation3/OS/Modules/SysPrxForUser.hpp" "ChonkyStation3/OS/Thread.hpp" "ChonkyStation3/OS/Thread.cpp" "ChonkyStation3/OS/ThreadManager.hpp" "ChonkyStation3/OS/ThreadManager.cpp" "ChonkyStation3/Common/MemoryConstants.hpp" "ChonkyStation3/OS/Modules/SysPrxForUser.cpp" "ChonkyStation3/Common/CellTypes.hpp" "ChonkyStation3/OS/Import.hpp" "ChonkyStation3/OS/Syscalls/SysMemory.cpp" "ChonkyStation3/OS/Syscalls/SysMMapper.cpp" "ChonkyStation3/OS/Modules/SysThread.hpp" "ChonkyStation3/OS/Modules/SysThread.cpp" "ChonkyStation3/OS/Modules/SysLwMutex.hpp" "ChonkyStation3/OS/Modules/SysLwMutex.cpp" "ChonkyStation3/OS/Modules/SysMMapper.hpp" "ChonkyStation3/OS/Modules/SysMMapper.cpp" "ChonkyStation3/OS/HandleManager.hpp" "ChonkyStation3/Common/ElfSymbolParser.hpp" "ChonkyStation3/OS/Modules/CellGcmSys.hpp" "ChonkyStation3/OS/Modules/CellGcmSys.cpp" "ChonkyStation3/OS/Modules/CellVideoOut.hpp" "ChonkyStation3/OS/Modules/CellVideoOut.cpp" "ChonkyStation3/RSX/RSX.hpp" "ChonkyStation3/RSX/RSX.cpp" "Dependencies/OpenGL/opengl.hpp" "ChonkyStation3/RSX/VertexShaderDecompiler.hpp" "ChonkyStation3/RSX/VertexShaderDecompiler.cpp" "Dependencies/Panda3DS/logger.hpp" "ChonkyStation3/OS/Syscalls/SysTimer.cpp" "ChonkyStation3/Scheduler/Scheduler.cpp" "ChonkyStation3/RSX/FragmentShaderDecompiler.cpp" "ChonkyStation3/OS/Modules/CellSysutil.cpp")
target_include_directories(ChonkyStation3 PUBLIC ChonkyStation3)
target_include_directories(ChonkyStation3 PUBLIC ChonkyStation3/Common)
target_include_directories(ChonkyStation3 PUBLIC ChonkyStation3/Loaders)
target_include_directories(ChonkyStation3 PUBLIC ChonkyStation3/Memory)
target_include_directories(ChonkyStation3 PUBLIC ChonkyStation3/PPU)
target_include_directories(ChonkyStation3 PUBLIC ChonkyStation3/RSX)
target_include_directories(ChonkyStation3 PUBLIC ChonkyStation3/OS)
target_include_directories(ChonkyStation3 PUBLIC ChonkyStation3/Scheduler)

target_include_directories(ChonkyStation3 PUBLIC Dependencies/ELFIO)
target_include_directories(ChonkyStation3 PUBLIC Dependencies/Dolphin)
target_include_directories(ChonkyStation3 PUBLIC Dependencies/OpenGL)
target_include_directories(ChonkyStation3 PUBLIC Dependencies/Panda3DS)
target_include_directories(ChonkyStation3 PUBLIC ${SDL2_INCLUDE_DIR})
target_include_directories(ChonkyStation3 PUBLIC Dependencies/xxHash)

target_link_libraries(ChonkyStation3 PUBLIC glad SDL2-static xxHash::xxhash)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-nonliteral -Wno-format-security -Wno-invalid-offsetof")
endif()