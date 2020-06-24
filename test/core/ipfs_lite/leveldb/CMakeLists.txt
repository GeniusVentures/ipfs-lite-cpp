addtest(leveldb_fs_test
    leveldb_fs_test.cpp
    )
target_link_libraries(leveldb_fs_test
    leveldb_wrapper
    base_fs_test
    )
set_target_properties(leveldb_fs_test PROPERTIES LINK_FLAGS "/FORCE:MULTIPLE")
addtest(leveldb_integration_test
    leveldb_integration_test.cpp
    )
target_link_libraries(leveldb_integration_test
    leveldb_wrapper
    base_leveldb_test
    Boost::filesystem
    )
set_target_properties(leveldb_integration_test PROPERTIES LINK_FLAGS "/FORCE:MULTIPLE")