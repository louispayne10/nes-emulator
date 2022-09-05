add_executable(nes-emulator main.cpp
                            cpu.cpp
                            memory.cpp)

target_link_libraries(nes-emulator PRIVATE fmt::fmt SDL2 project_warnings)
