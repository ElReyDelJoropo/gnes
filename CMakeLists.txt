if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "In-source builds are disabled.
    Please create a subfolder and use `cmake ..` inside it.
    NOTE: cmake will now create CMakeCache.txt and CMakeFiles/*.
          You must delete them, or cmake will refuse to work.")
endif()

cmake_minimum_required(VERSION 3.8)

project(Gnes
    LANGUAGES CXX
    DESCRIPTION "A simple nes emulator"
    VERSION 0.1)

add_compile_options(
    -Wall
    -Wextra
    )

set(SOURCES
    "src/main.cpp"
    "src/Emulator.cpp"
    "src/Cartrigde.cpp"
    "src/Cpu.cpp"
    "src/CpuBus.cpp"
    "src/mappers/Mapper_000.cpp"
    "src/Ppu.cpp"
    "src/VirtualScreen.cpp"
    "src/PpuBus.cpp"
    )
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

find_package(SFML COMPONENTS audio graphics system window REQUIRED)

add_executable(gnes ${SOURCES})
set_target_properties(gnes
    PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
    )
target_include_directories(gnes PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(gnes PUBLIC sfml-graphics sfml-system sfml-window)
