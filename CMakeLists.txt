cmake_minimum_required(VERSION 3.20)
project(kdemulator CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # para que el LSP (clangd) tenga IntelliSense

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(UNICORN_DIR ${CMAKE_SOURCE_DIR}/emu-core/external/unicorn)

# El .a de unicorn viene precompilado con MinGW (símbolos Itanium).
# CMake en MinGW preferiria libunicorn.dll.a (import lib) y no hay unicorn.dll
# en el repo, asi que forzamos la ruta estatica completa.
set(UNICORN_LIB ${UNICORN_DIR}/lib/libunicorn.a)

# --- FTXUI ---
include(FetchContent)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG v5.0.0
)
set(FTXUI_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(FTXUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FTXUI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(FTXUI_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(ftxui)

# --- ZYDIS ---
FetchContent_Declare(
  zydis
  GIT_REPOSITORY https://github.com/zyantific/zydis.git
  GIT_TAG v4.1.0
)
set(ZYDIS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ZYDIS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ZYDIS_BUILD_DOXYGEN OFF CACHE BOOL "" FORCE)
set(ZYDIS_BUILD_SHARED_LIB OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(zydis)

file(GLOB_RECURSE CORE_SRCS  CONFIGURE_DEPENDS "emu-core/src/*.cpp")
file(GLOB_RECURSE STUBS_SRCS CONFIGURE_DEPENDS "emu-stubs/*.cpp")
file(GLOB_RECURSE CLI_SRCS   CONFIGURE_DEPENDS "emu-cli/*.cpp")

add_executable(kdemulator
    ${CORE_SRCS}
    ${STUBS_SRCS}
    ${CLI_SRCS}
)

target_include_directories(kdemulator PRIVATE
    ${CMAKE_SOURCE_DIR}                      # "emu-core/src/...", "sdk/hex.hpp"
    ${CMAKE_SOURCE_DIR}/sdk                 # <hex.hpp>
    ${CMAKE_SOURCE_DIR}/emu-core/src         # "cpu/CPU.hpp", "pe/format.hpp", ...
    ${CMAKE_SOURCE_DIR}/emu-core/src/kernel  # "include/structs.hpp"
    ${CMAKE_SOURCE_DIR}/emu-stubs            # "APIDispatcher/APIDispatcher.hpp"
    ${UNICORN_DIR}/include                   # "unicorn/unicorn.h"
)

target_link_libraries(kdemulator PRIVATE
    ${UNICORN_LIB}
    Zydis
    ftxui::screen
    ftxui::dom
    ftxui::component
)

# --- Runtime MinGW junto al .exe ---
# gcc.exe busca sus DLL en el PATH o en la carpeta del ejecutable. Copiandolas
# al build/, el .exe corre sin depender de tener MSYS2 abierto en otra terminal.
if(MINGW)
  get_filename_component(GCC_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
  set(MINGW_RUNTIME_DLLS "")
  foreach(runtime
      libstdc++-6.dll
      libgcc_s_seh-1.dll
      libwinpthread-1.dll
      libgomp-1.dll
      libssp-0.dll)
    if(EXISTS "${GCC_BIN_DIR}/${runtime}")
      list(APPEND MINGW_RUNTIME_DLLS "${GCC_BIN_DIR}/${runtime}")
    endif()
  endforeach()

  if(MINGW_RUNTIME_DLLS)
    message(STATUS "MinGW runtime -> ${MINGW_RUNTIME_DLLS}")
    add_custom_command(TARGET kdemulator POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
              ${MINGW_RUNTIME_DLLS} $<TARGET_FILE_DIR:kdemulator>
      VERBATIM)
  endif()
endif()
