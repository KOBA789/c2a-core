## CMake file for S2E integration

cmake_minimum_required(VERSION 3.13)
project(C2A)

# options
# SCI COM for connection to WINGS TMTC IF
# ！！！注意！！！
# これをONにした状態で，SCIの受け口がない場合（TMTC IFが動いてない状態）
# そちらのバッファが詰まってSILSの動作が止まることがあるので注意すること！
option(USE_SCI_COM_WINGS "Use SCI_COM_WINGS" ON)

option(USE_SILS_MOCKUP "Use SILS mockup for build C2A with minimal user in C89 only" OFF)

option(BUILD_C2A_AS_SILS_FW "Build C2A as SILS firmware" ON)

set(USE_ALL_C2A_CORE_APPS OFF)
# Core App のターゲット追加は src_user/Applications/CMakeLists.txt で行う
set(USE_ALL_C2A_CORE_TEST_APPS OFF)

if(USE_SILS_MOCKUP)
  set(BUILD_C2A_AS_CXX OFF)
endif()

if(BUILD_C2A_AS_CXX)
  message("build C2A as C++!")
endif()

set(C2A_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/src_core)
set(C2A_USER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/src_user)

include_directories(src)

# Output debug print to SILS console window
option(SHOW_DEBUG_PRINT_ON_SILS "Show debug print")
set(SHOW_DEBUG_PRINT_ON_SILS ON)
if(SHOW_DEBUG_PRINT_ON_SILS)
  add_definitions(-DSHOW_DEBUG_PRINT_ON_SILS)
  message("Show debug print")
endif()

add_subdirectory(${C2A_CORE_DIR})

add_subdirectory(${C2A_USER_DIR}/Applications)
add_subdirectory(${C2A_USER_DIR}/Drivers)
add_subdirectory(${C2A_USER_DIR}/IfWrapper)
add_subdirectory(${C2A_USER_DIR}/Library)
add_subdirectory(${C2A_USER_DIR}/Settings)
add_subdirectory(${C2A_USER_DIR}/TlmCmd)

set(C2A_SRCS
  ${C2A_USER_DIR}/c2a_main.c
)

if(BUILD_C2A_AS_CXX)
  message("Build as C++!!!")
  set_source_files_properties(${C2A_SRCS} PROPERTIES LANGUAGE CXX)  # C++
endif()

if(USE_SILS_MOCKUP)
  add_definitions(-DDEFINE_MAIN_ON_SILS)
  add_executable(${PROJECT_NAME} ${C2A_SRCS})
else()
  add_library(${PROJECT_NAME} STATIC ${C2A_SRCS})
endif()

set(C2A_USER_MODULES
  C2A_USER_APPS
  C2A_USER_CMD_TLM
  C2A_USER_DRIVERS
  C2A_USER_IF_WRAPPER
  C2A_USER_LIB
  C2A_USER_SETTINGS
)

if(MSVC)
  target_link_options(${PROJECT_NAME} PRIVATE "/WHOLEARCHIVE")
  target_link_libraries(${PROJECT_NAME} PRIVATE
    C2A_CORE
    ${C2A_USER_MODULES}
  )
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE
    -Wl,--whole-archive
    C2A_CORE
    -Wl,--no-whole-archive
    ${C2A_USER_MODULES}
    -lm
  )
endif()

if(WIN32)
  add_custom_command(TARGET ${PROJECT_NAME}
    PRE_BUILD
    COMMAND git_revision.bat
    WORKING_DIRECTORY ${C2A_USER_DIR}/Script)
else()
  add_custom_command(TARGET ${PROJECT_NAME}
    PRE_BUILD
    COMMAND ./git_revision.sh
    WORKING_DIRECTORY ${C2A_USER_DIR}/Script)
endif()

include(${C2A_USER_DIR}/common.cmake)
