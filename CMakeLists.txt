
cmake_minimum_required(VERSION 3.5.1)

# Project
project(ipfs-lite-cpp C CXX)
include(cmake/functions.cmake)


include(ExternalProject)

# Note: For all external projects, instead of using checked-out code, one could
# specify GIT_REPOSITORY and GIT_TAG to have cmake download the dependency directly,
# without needing to add a submodule to your project.

# Builds GTest project from the git submodule.
ExternalProject_Add(GTest
  PREFIX GTest
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../GTest"
  CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/GTest
)
set(GTest_DIR "${CMAKE_CURRENT_BINARY_DIR}/GTest/lib/cmake/GTest")

find_package(GTest CONFIG REQUIRED)
# find_package(GMock CONFIG REQUIRED)

# Builds protobuf project from the git submodule.
ExternalProject_Add(protobuf
  PREFIX protobuf
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../grpc/third_party/protobuf/cmake"
  CMAKE_CACHE_ARGS
        -Dprotobuf_BUILD_TESTS:BOOL=OFF
        -Dprotobuf_WITH_ZLIB:BOOL=OFF
        -Dprotobuf_MSVC_STATIC_RUNTIME:BOOL=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/protobuf
)


# the location where protobuf-config.cmake will be installed varies by platform
if (WIN32)
  set(_FINDPACKAGE_PROTOBUF_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/protobuf/cmake")
else()
  set(_FINDPACKAGE_PROTOBUF_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/protobuf/lib/cmake/protobuf")
endif()
set(_FINDPACKAGE_PROTOBUF_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/protobuf/include")
set(_FINDPACKAGE_PROTOBUF_LIB_DIR "${CMAKE_CURRENT_BINARY_DIR}/protobuf/lib")

# if OPENSSL_ROOT_DIR is set, propagate that hint path to the external projects with OpenSSL dependency.
set(_CMAKE_ARGS_OPENSSL_ROOT_DIR "")
if (OPENSSL_ROOT_DIR)
  set(_CMAKE_ARGS_OPENSSL_ROOT_DIR "-DOPENSSL_ROOT_DIR:PATH=${OPENSSL_ROOT_DIR}")
endif()

# Builds leveldb
ExternalProject_Add(leveldb
  PREFIX leveldb
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../leveldb"
  CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/leveldb
)
set(_FINDPACKAGE_LEVELDB_DIR "${CMAKE_CURRENT_BINARY_DIR}/leveldb/lib/cmake/leveldb")
# Builds Microsoft.GSL
ExternalProject_Add(Microsoft.GSL
  PREFIX Microsoft.GSL
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../GSL"
  CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/GSL
        -DGSL_TEST:BOOL=OFF
)
set(_FINDPACKAGE_GSL_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/GSL/include")
# Builds spdlog v1.4.2
ExternalProject_Add(spdlog
  PREFIX spdlog
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../spdlog"
  CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/spdlog
)
set(_FINDPACKAGE_SPDLOG_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/spdlog/lib/spdlog/cmake")
set(_FINDPACKAGE_SPDLOG_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/spdlog/include")

# Builds tsl_hat_trie 343e0dac54fc8491065e8a059a02db9a2b1248ab
ExternalProject_Add(tsl_hat_trie
  PREFIX tsl_hat_trie
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../hat-trie"
  CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/hat-trie
)
set(_FINDPACKAGE_TSL_HAT_TRIE_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/hat-trie/lib/cmake/tsl_hat_trie")
# Builds Boost.DI c5287ee710ad90f5286d0cc2b9e49b72d89267a6
ExternalProject_Add(Boost.DI
  PREFIX Boost.DI
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../Boost.DI"
  CMAKE_CACHE_ARGS
        -DBOOST_DI_OPT_BUILD_TESTS:BOOL=OFF
        -DBOOST_DI_OPT_BUILD_EXAMPLES:BOOL=OFF
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/Boost.DI
)
set(_FINDPACKAGE_BOOST_DI_CONFIG_DIR "${CMAKE_CURRENT_BINARY_DIR}/Boost.DI/lib/cmake/Boost.DI")


# BOOST library 
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost REQUIRED COMPONENTS date_time filesystem random) # header only libraries must not be added here

# Builds libp2p

ExternalProject_Add(libp2p
  PREFIX libp2p
  SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../libp2p"
  CMAKE_CACHE_ARGS
        ${_CMAKE_ARGS_OPENSSL_ROOT_DIR}
        -DCMAKE_USE_OPENSSL:BOOL=ON
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/libp2p
        -DTESTING:BOOL=OFF 
        -DEXPOSE_MOCKS:BOOL=ON
        -DHUNTER_ENABLED:BOOL=OFF
        -DBUILD_EXAMPLES:BOOL=OFF
        -DProtobuf_DIR:PATH=${_FINDPACKAGE_PROTOBUF_CONFIG_DIR}
        -DProtobuf_INCLUDE_DIR:PATH=${_FINDPACKAGE_PROTOBUF_INCLUDE_DIR}
        -DProtobuf_LIBRARIES:PATH=${_FINDPACKAGE_PROTOBUF_LIB_DIR}
        -Dspdlog_DIR:PATH=${_FINDPACKAGE_SPDLOG_CONFIG_DIR}
        -DBoost.DI_DIR:PATH=${_FINDPACKAGE_BOOST_DI_CONFIG_DIR}
        -DBOOST_ROOT:PATH="${BOOST_ROOT}"
        -DBOOST_INCLUDE_DIR:PATH="${BOOST_INCLUDE_DIR}"
        -DBOOST_LIBRARY_DIR:PATH="${BOOST_LIBRARY_DIR}"
        -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}\ -I\ ${BOOST_INCLUDE_DIR}\ -I\ ${_FINDPACKAGE_GSL_INCLUDE_DIR}\ -I\ ${_FINDPACKAGE_SPDLOG_INCLUDE_DIR}\ -I\ \\\"${OPENSSL_ROOT_DIR}/include\\\"\ -I\ ${_FINDPACKAGE_PROTOBUF_INCLUDE_DIR}

  DEPENDS protobuf spdlog tsl_hat_trie Boost.DI 
)
set(libp2p_DIR "${CMAKE_CURRENT_BINARY_DIR}/libp2p/lib/cmake/libp2p")
set(libp2p_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/libp2p/include")
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${_FINDPACKAGE_GSL_INCLUDE_DIR}
  deps/tinycbor/src
)

# find packages for common
message("libp2p_DIR=" ${libp2p_DIR})

find_package(OpenSSL REQUIRED)

set(spdlog_DIR "${_FINDPACKAGE_SPDLOG_CONFIG_DIR}")
find_package(spdlog CONFIG REQUIRED)

set(tsl_hat_trie_DIR "${_FINDPACKAGE_TSL_HAT_TRIE_CONFIG_DIR}")
find_package(tsl_hat_trie CONFIG REQUIRED)

find_package(libp2p CONFIG REQUIRED)

set(Protobuf_DIR "${_FINDPACKAGE_PROTOBUF_CONFIG_DIR}")
set(Protobuf_INCLUDE_DIR "${_FINDPACKAGE_PROTOBUF_INCLUDE_DIR}")
set(Protobuf_LIBRARIES "${_FINDPACKAGE_PROTOBUF_LIB_DIR}")
find_package(Protobuf CONFIG REQUIRED)

set(leveldb_DIR "${_FINDPACKAGE_LEVELDB_DIR}")
find_package(leveldb CONFIG REQUIRED)

option(TESTING "Build tests" ON)

add_subdirectory(deps)
add_subdirectory(primitives)
add_subdirectory(codec)
add_subdirectory(common)
add_subdirectory(crypto)
add_subdirectory(ipfs_lite)

if (TESTING)
  enable_testing()
  add_subdirectory(test)
endif ()
