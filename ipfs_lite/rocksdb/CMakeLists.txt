
add_library(rocksdb_wrapper
    rocksdb.cpp
    rocksdb_batch.cpp
    rocksdb_error.cpp
    rocksdb_cursor.cpp
    )
target_link_libraries(rocksdb_wrapper
    RocksDB::rocksdb
    buffer
    logger
    )
target_include_directories(rocksdb_wrapper PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)