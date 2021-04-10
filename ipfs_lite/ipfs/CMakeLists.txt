add_library(ipfs_datastore_in_memory
    impl/in_memory_datastore.cpp
    impl/ipfs_datastore_error.cpp
    )

target_link_libraries(ipfs_datastore_in_memory
    buffer
    cbor
    cid
    )

#set_target_properties(ipfs_datastore_in_memory PROPERTIES PUBLIC_HEADER "some.h;another.h")

add_library(ipfs_datastore_rocksdb
    impl/datastore_rocksdb.cpp
    impl/ipfs_datastore_error.cpp
    )
target_link_libraries(ipfs_datastore_rocksdb
    buffer
    cbor
    cid
    rocksdb_wrapper
    )

add_library(ipfs_blockservice
    impl/ipfs_block_service.cpp
    )
target_link_libraries(ipfs_blockservice
    buffer
    )

add_subdirectory(merkledag)
add_subdirectory(graphsync)
