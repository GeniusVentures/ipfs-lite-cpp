add_library(base_fs_test
    base_fs_test.hpp
    base_fs_test.cpp
    )

target_link_libraries(base_fs_test
    GTest::gtest
    Boost::filesystem
    Boost::boost
    logger
    )

add_library(base_leveldb_test
    base_leveldb_test.hpp
    base_leveldb_test.cpp
    )

target_link_libraries(base_leveldb_test
    base_fs_test
    Boost::filesystem
    Boost::boost
    leveldb_wrapper
    logger
    )



