addtest(hexutil_test
    hexutil_test.cpp
    )
target_link_libraries(hexutil_test
    hexutil
    )

addtest(blob_test
    blob_test.cpp
    )
target_link_libraries(blob_test
    blob
    )

addtest(outcome_test
    outcome_test.cpp
    )
target_link_libraries(outcome_test
    outcome
    todo_error
    )
