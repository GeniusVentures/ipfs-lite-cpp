

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
link_directories(${BOOST_LIBRARY_DIR})
add_subdirectory(core)
add_subdirectory(testutil)