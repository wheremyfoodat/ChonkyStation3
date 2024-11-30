cmake_minimum_required(VERSION 3.8)

project("ChonkyStation3")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(ChonkyStation3/Common)
include_directories(ChonkyStation3/Loaders)
include_directories(ChonkyStation3/Memory)

include_directories(Dependencies/ELFIO)

add_executable(ChonkyStation3 "ChonkyStation3/ChonkyStation3.cpp" "ChonkyStation3/ChonkyStation3.hpp" "ChonkyStation3/Loaders/ELF/ELFLoader.hpp" "ChonkyStation3/Loaders/ELF/ELFLoader.cpp" "ChonkyStation3/Common/common.hpp" "ChonkyStation3/PlayStation3.hpp" "ChonkyStation3/PlayStation3.cpp" "ChonkyStation3/Memory/Memory.cpp" "ChonkyStation3/Memory/Memory.hpp" "ChonkyStation3/Common/BEField.hpp")
