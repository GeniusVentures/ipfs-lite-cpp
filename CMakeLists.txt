cmake_minimum_required(VERSION 3.5.1)
set(
    CMAKE_TOOLCHAIN_FILE
    "${CMAKE_SOURCE_DIR}/cmake/toolchain/cxx17.cmake"
    CACHE
    FILEPATH
    "Default toolchain"
)
# Project
project(ipfs-lite-cpp C CXX)

if (NOT EXISTS "${CMAKE_TOOLCHAIN_FILE}")
  # https://cgold.readthedocs.io/en/latest/tutorials/toolchain/globals/cxx-standard.html#summary
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
endif ()

# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
# set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
# set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

# set(CompilerFlags
#         CMAKE_CXX_FLAGS
#         CMAKE_CXX_FLAGS_DEBUG
#         CMAKE_CXX_FLAGS_RELEASE
#         CMAKE_C_FLAGS
#         CMAKE_C_FLAGS_DEBUG
#         CMAKE_C_FLAGS_RELEASE
#         )
# foreach(CompilerFlag ${CompilerFlags})
#   string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
# endforeach()

include(cmake/functions.cmake)
include(cmake/print.cmake)
print("C flags: ${CMAKE_C_FLAGS}")
print("CXX flags: ${CMAKE_CXX_FLAGS}")
print("C Debug flags: ${CMAKE_C_FLAGS_DEBUG}")
print("CXX Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
print("C Release flags: ${CMAKE_C_FLAGS_RELEASE}")
print("CXX Release flags: ${CMAKE_CXX_FLAGS_RELEASE}")

# BOOST library 
set(Boost_USE_STATIC_LIBS ON) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME ON) 
find_package(Boost REQUIRED COMPONENTS date_time filesystem random regex) # header only libraries must not be added here

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${_FINDPACKAGE_GSL_INCLUDE_DIR}
  deps/tinycbor/src
)

# # find packages for common
# message("libp2p_DIR=" ${libp2p_DIR})
find_package(GTest CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(tsl_hat_trie CONFIG REQUIRED)
find_package(libp2p CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(leveldb CONFIG REQUIRED)

option(TESTING "Build tests" ON)

add_subdirectory(deps)
add_subdirectory(primitives)
add_subdirectory(clock)
add_subdirectory(codec)
add_subdirectory(common)
add_subdirectory(crypto)
add_subdirectory(data_transfer)
add_subdirectory(ipfs_lite)

if (TESTING)
  enable_testing()
  add_subdirectory(test)
endif ()
