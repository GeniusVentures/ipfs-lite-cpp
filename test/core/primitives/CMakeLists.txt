
add_subdirectory(cid)
addtest(big_int_test
big_int_test.cpp
)
target_link_libraries(big_int_test
Boost::boost
)