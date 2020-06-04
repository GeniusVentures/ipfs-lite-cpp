
add_library(leveldb_wrapper
    leveldb.cpp
    leveldb_batch.cpp
    leveldb_error.cpp
    leveldb_cursor.cpp
    )
target_link_libraries(leveldb_wrapper
    leveldb::leveldb
    buffer
    logger
    )
