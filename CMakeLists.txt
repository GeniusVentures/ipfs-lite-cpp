cmake_minimum_required(VERSION 3.22)
set(
    CMAKE_TOOLCHAIN_FILE
    "${CMAKE_SOURCE_DIR}/cmake/toolchain/cxx17.cmake"
    CACHE
    FILEPATH
    "Default toolchain"
)

# Project
project(ipfs-lite-cpp LANGUAGES C CXX)

include(cmake/functions.cmake)
include(cmake/print.cmake)

# BOOST library
find_package(Boost REQUIRED COMPONENTS date_time filesystem random regex) # header only libraries must not be added here

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${_FINDPACKAGE_GSL_INCLUDE_DIR}
    deps/tinycbor/src
)

# find packages for common
find_package(GTest CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(tsl_hat_trie CONFIG REQUIRED)
find_package(libp2p CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(soralog CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(RocksDB CONFIG REQUIRED)

option(TESTING "Build tests" ON)

add_subdirectory(deps)
add_subdirectory(primitives)
add_subdirectory(clock)
add_subdirectory(codec)
add_subdirectory(common)
add_subdirectory(crypto)
add_subdirectory(data_transfer)
add_subdirectory(ipfs_lite)

install_hfile(ipfs-lite)

if(TESTING)
    SET(WIN_CRYPT_LIBRARY)

    if(NOT UNIX)
        SET(WIN_CRYPT_LIBRARY crypt32)
    endif()

    enable_testing()
    add_subdirectory(test)
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS rocksdb_integration_test)
endif()

set(CONFIG_NAME "ipfs-lite-cppConfig")
set(CONFIG_DESTINATION_DIR "${CMAKE_INSTALL_PREFIX}/cmake/${PROJECT_NAME}")

install(TARGETS ipfs_blockservice
    ipfs_datastore_rocksdb
    ipfs_datastore_in_memory
    ipfs_datastore_error
    ipfs_merkledag_service
    graphsync
    buffer rocksdb_wrapper cid ipld_node cbor ipfs_lite_hasher logger
    hexutil
    blake2
    ipld_link
    outcome
    tinycbor
    ipld_block
    blob
    graphsync_proto
    ipld_node_protobuf
    ipfs_kad_dht
    EXPORT ${CONFIG_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_PROTO_HEADER DESTINATION "${CMAKE_INSTALL_PREFIX}/include/protobuf"
    FRAMEWORK DESTINATION ${CMAKE_INSTALL_PREFIX}
)
message("--install --- ${CMAKE_INSTALL_PREFIX}/include/protobuf")

# install header files
install_hfile(codec)
install_hfile(common)
install_hfile(crypto)
install_hfile(data_transfer)
install_hfile(deps)
install_hfile(platform)
install_hfile(primitives)
install_hfile(ipfs_lite)
install_hfile(clock)

# install protobuf header files
message("-gen---${CMAKE_BINARY_DIR}/generated")
install(
    DIRECTORY ${CMAKE_BINARY_DIR}/generated/ipfs_lite
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include
    FILES_MATCHING # install only matched files
    PATTERN "*.pb.h" # select header files hpp or h file
)

install(
    EXPORT ${CONFIG_NAME}
    NAMESPACE ipfs-lite-cpp::
    DESTINATION ${CONFIG_DESTINATION_DIR}
)
