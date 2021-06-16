
addtest(graphsync_acceptance_test
    graphsync_acceptance_test.cpp
    graphsync_acceptance_common.cpp
    )
target_link_libraries(graphsync_acceptance_test
    graphsync
    ipfs_merkledag_service
    p2p::asio_scheduler
    Boost::date_time
    Boost::regex
    ${WIN_CRYPT_LIBRARY}
    )
set_target_properties(graphsync_acceptance_test PROPERTIES LINK_FLAGS "/FORCE:MULTIPLE")

addtest(graphsync_extension_test
    extensions_test.cpp
    )
target_link_libraries(graphsync_extension_test
    graphsync
    )
set_target_properties(graphsync_extension_test PROPERTIES LINK_FLAGS "/FORCE:MULTIPLE")