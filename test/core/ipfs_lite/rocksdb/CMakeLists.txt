addtest(rocksdb_fs_test
    rocksdb_fs_test.cpp
    )
target_link_libraries(rocksdb_fs_test
    rocksdb_wrapper
    base_fs_test
    )
set_target_properties(rocksdb_fs_test PROPERTIES LINK_FLAGS "/FORCE:MULTIPLE")
addtest(rocksdb_integration_test
    rocksdb_integration_test.cpp
    )
target_link_libraries(rocksdb_integration_test
    rocksdb_wrapper
    base_rocksdb_test
    Boost::filesystem
    )
set_target_properties(rocksdb_integration_test PROPERTIES LINK_FLAGS "/FORCE:MULTIPLE")