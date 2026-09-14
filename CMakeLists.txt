cmake_minimum_required(VERSION 3.20)
project(kdemulator CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # para que VSCode tenga buen IntelliSense

set(UNICORN_DIR ${CMAKE_SOURCE_DIR}/emu-core/external/unicorn)

# --- FTXUI ---
include(FetchContent)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG v5.0.0
)
FetchContent_MakeAvailable(ftxui)

file(GLOB_RECURSE CORE_SRCS "emu-core/src/*.cpp")
file(GLOB_RECURSE PY_SRCS   "emu-stubs/*.cpp")
file(GLOB_RECURSE CLI_SRCS "emu-cli/*.cpp")
file(GLOB_RECURSE SDK_SRCS "sdk/*.cpp")
add_executable(kdemulator
    ${CORE_SRCS}
    ${PY_SRCS}
    ${CLI_SRCS}
    ${SDK_SRCS}
)

target_include_directories(kdemulator PRIVATE
    sdk
    emu-core/include
    emu-stubs/include
    ${UNICORN_DIR}/include
)

target_link_directories(kdemulator PRIVATE
    ${UNICORN_DIR}/lib
)

target_link_libraries(kdemulator PRIVATE
    unicorn
    ftxui::screen
    ftxui::dom
    ftxui::component
)

add_custom_command(TARGET kdemulator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${UNICORN_DIR}/bin/libunicorn.dll"
        "$<TARGET_FILE_DIR:kdemulator>"
)