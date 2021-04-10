
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
