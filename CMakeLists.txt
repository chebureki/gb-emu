cmake_minimum_required(VERSION 3.22)
project(gb_emu C)

set(CMAKE_C_STANDARD 99)

add_executable(gb_emu main.c bus/bus.c bus/bus.h common/log.h common/datatypes.h cpu/cpu.c cpu/cpu.h memory/memory.c memory/memory.h emulator/emulator.c emulator/emulator.h cpu/instructions.c cpu/instructions.h common/bitwise.h cartridge/cartridge.c cartridge/cartridge.h ppu/ppu.c ppu/ppu.h gui/renderer.c gui/renderer.h gui/gui.h gui/input.c gui/input.h)
target_link_libraries(${PROJECT_NAME} SDL2)