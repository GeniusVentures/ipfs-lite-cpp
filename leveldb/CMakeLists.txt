

add_library(leveldb
    leveldb.cpp
    )
target_link_libraries(leveldb
    leveldb::leveldb
    )
