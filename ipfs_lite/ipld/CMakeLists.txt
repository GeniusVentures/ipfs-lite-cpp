add_subdirectory(impl/protobuf)

add_library(ipld_block INTERFACE)

target_link_libraries(ipld_block INTERFACE
    cid
    ipfs_lite_hasher
    )

add_library(ipld_link
    impl/ipld_link_impl.cpp
    )

target_link_libraries(ipld_link
    cid
    )

add_library(ipld_node
    impl/ipld_node_impl.cpp
    impl/ipld_node_encoder_pb.cpp
    impl/ipld_node_decoder_pb.cpp
    )

target_link_libraries(ipld_node
    ipld_node_protobuf
    ipld_link
    ipld_block
    Boost::boost
    cid
    )

add_library(ipld_walker
    walker.cpp
    )

target_link_libraries(ipld_walker
    ipld_node_protobuf
    cbor
    logger
    )
