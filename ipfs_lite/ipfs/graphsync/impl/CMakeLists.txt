add_subdirectory(network/marshalling/protobuf)
add_library(graphsync
    network/marshalling/serialize.cpp
    network/marshalling/message_parser.cpp
    network/marshalling/message_builder.cpp
    network/marshalling/request_builder.cpp
    network/marshalling/response_builder.cpp
    )

target_include_directories(graphsync PRIVATE "${CMAKE_BINARY_DIR}/generated/ipfs_lite/ipfs/graphsync/impl/network/marshalling")

target_link_libraries(graphsync
    Boost::boost
    p2p::subscription
    cid
    buffer
    cbor
    filecoin_hasher
    logger
    graphsync_proto
    )