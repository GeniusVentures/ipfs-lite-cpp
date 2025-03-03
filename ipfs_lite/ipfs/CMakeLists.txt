add_library(ipfs_datastore_error
    impl/ipfs_datastore_error.cpp
)
target_link_libraries(ipfs_datastore_error
    PUBLIC Boost::system p2p::p2p_event
)

add_library(ipfs_datastore_in_memory
    impl/in_memory_datastore.cpp
)
target_link_libraries(ipfs_datastore_in_memory
    buffer
    cbor
    cid
    logger
    ipfs_datastore_error
)

add_library(ipfs_datastore_rocksdb
    impl/datastore_rocksdb.cpp
)
target_link_libraries(ipfs_datastore_rocksdb
    cbor
    rocksdb_wrapper
    ipfs_datastore_error
)

add_library(ipfs_blockservice
    impl/ipfs_block_service.cpp
)
target_link_libraries(ipfs_blockservice
    cbor
    buffer
    logger
)

add_subdirectory(merkledag)
add_subdirectory(graphsync)
