include(FetchContent)

# TODO: See Catch2 docs for CTest integration so I can maybe run the tests from neovim easier
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.0.1
)

FetchContent_MakeAvailable(Catch2)

SET(TEST_FILES memory_tests.cpp
               arithmetic_instruction_tests.cpp
               branch_instruction_tests.cpp
               load_instruction_tests.cpp
               flag_instruction_tests.cpp
               logic_instruction_tests.cpp
               stack_instruction_tests.cpp)
SET(NES_FILES ${CMAKE_SOURCE_DIR}/src/cpu.cpp
              ${CMAKE_SOURCE_DIR}/src/memory.cpp)

add_executable(nes-tests ${TEST_FILES} ${NES_FILES})
target_link_libraries(nes-tests PRIVATE Catch2::Catch2WithMain fmt::fmt project_warnings)
target_include_directories(nes-tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
