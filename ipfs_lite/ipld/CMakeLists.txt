add_subdirectory(impl/protobuf)

add_library(ipld_block INTERFACE)
target_link_libraries(ipld_block INTERFACE
    cid
    ipfs_lite_hasher
    )


