addtest(mnn_integration_test
    mnn_integration_test.cpp
    )
target_link_libraries(mnn_integration_test
    base_mnn_test
    )
set_target_properties(mnn_integration_test PROPERTIES LINK_FLAGS "/FORCE:MULTIPLE")