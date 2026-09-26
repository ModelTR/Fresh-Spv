cmake_minimum_required(VERSION 4.4.0)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
endif()

project(SpvCompress LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 把 FetchContent 的目录设置在主工作文件夹
set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps_cache)
# 完全关闭更新
#set(FETCHCONTENT_FULLY_DISCONNECTED ON)

find_package(TBB REQUIRED)

include(FetchContent)

# --- zstd ------------------------------------------------------------------

FetchContent_Declare(
    zstd
    GIT_REPOSITORY https://gh-proxy.com/https://github.com/facebook/zstd/
    GIT_TAG v1.5.7
    GIT_SHALLOW TRUE
    UPDATE_DISCONNECTED TRUE
    SYSTEM
)

set(ZSTD_BUILD_STATIC ON)
set(ZSTD_BUILD_SHARED OFF)
set(ZSTD_BUILD_PROGRAMS OFF)
set(ZSTD_BUILD_TESTS OFF)

FetchContent_MakeAvailable(zstd)

# --- Frozen -------------------------------------------------------------------
FetchContent_Declare(
  frozen
  GIT_REPOSITORY https://gh-proxy.com/https://github.com/serge-sans-paille/frozen
  GIT_TAG 1.2.0
  GIT_SHALLOW TRUE
  UPDATE_DISCONNECTED TRUE
  SYSTEM
)

FetchContent_MakeAvailable(frozen)

# --- tl::expected -------------------------------------------------------------
FetchContent_Declare(
  tl-expected
  GIT_REPOSITORY https://gh-proxy.com/https://github.com/TartanLlama/expected
  GIT_TAG v1.3.1
  GIT_SHALLOW TRUE
  UPDATE_DISCONNECTED TRUE
  SYSTEM
)

set(EXPECTED_BUILD_TESTS OFF)

FetchContent_MakeAvailable(tl-expected)

# --- spdlog -------------------------------------------------------------
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://gh-proxy.com/https://github.com/gabime/spdlog
  GIT_TAG v1.17.0
  GIT_SHALLOW TRUE
  UPDATE_DISCONNECTED TRUE
  SYSTEM
)

set(SPDLOG_ENABLE_PCH ON)
set(SPDLOG_USE_STD_FORMAT OFF)
set(SPDLOG_TIDY ON)
set(SPDLOG_BUILD_WARNINGS ON)
set(SPDLOG_SYSTEM_INCLUDES ON)

FetchContent_MakeAvailable(spdlog)

# --- cxxopts ---------------------------------------------------------------
FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://gh-proxy.com/https://github.com/jarro2783/cxxopts
  GIT_TAG v3.3.1
  GIT_SHALLOW TRUE
  UPDATE_DISCONNECTED TRUE
  SYSTEM
)

set(CXXOPTS_BUILD_EXAMPLES OFF)
set(CXXOPTS_BUILD_TESTS OFF)

FetchContent_MakeAvailable(cxxopts)

# --- scnlib ----------------------------------------------------------------

FetchContent_Declare(
  scnlib
  GIT_REPOSITORY https://gh-proxy.com/https://github.com/eliaskosunen/scnlib
  GIT_TAG v4.0.1
  GIT_SHALLOW TRUE
  UPDATE_DISCONNECTED TRUE
  SYSTEM
)

FetchContent_MakeAvailable(scnlib)

# --- smol-v ----------------------------------------------------------------

FetchContent_Declare(
    smol-v
    GIT_REPOSITORY https://gh-proxy.com/https://github.com/aras-p/smol-v
    GIT_TAG 55000efe742f56d8b51223b9ea7775a8f0501881
    GIT_SHALLOW TRUE
    UPDATE_DISCONNECTED TRUE
    SYSTEM
)
FetchContent_MakeAvailable(smol-v)
add_library(smol-v-lib STATIC
    ${smol-v_SOURCE_DIR}/source/smolv.cpp
)
target_include_directories(smol-v-lib PUBLIC
    ${smol-v_SOURCE_DIR}/source
)

file(GLOB SRC_FILES src/*.cpp)

add_executable(SpvCompress
  main.cpp
  ${SRC_FILES}
  )

set(GENERATED_FILE ${CMAKE_CURRENT_SOURCE_DIR}/include/embed.hpp)

find_package(Python3 COMPONENTS Interpreter REQUIRED)

add_custom_target(run_embed_to_hpp
    COMMAND 
        ${Python3_EXECUTABLE} 
        ${CMAKE_CURRENT_SOURCE_DIR}/py/embed_to_hpp.py
        --output ${GENERATED_FILE}
        --shader-dirs ${CMAKE_CURRENT_SOURCE_DIR}/shader
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/py/embed_to_hpp.py
    BYPRODUCTS ${GENERATED_FILE}
    COMMENT "Generating ${GENERATED_FILE}"
    VERBATIM
  )

add_dependencies(SpvCompress run_embed_to_hpp)

target_include_directories(SpvCompress PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(SpvCompress PRIVATE
    zstd
    smol-v-lib
    frozen
    expected
    TBB::tbb
    spdlog::spdlog
    cxxopts::cxxopts
    scn::scn
)


target_compile_options(SpvCompress PRIVATE
    -Wno-c23-extensions
    -Wall -Wextra)
        
target_link_options(SpvCompress PRIVATE
    -fuse-ld=lld)