add_library(hexutil
    hexutil.hpp
    hexutil.cpp
    )
target_link_libraries(hexutil
    Boost::boost
    outcome
    )

add_library(outcome INTERFACE)
target_link_libraries(outcome INTERFACE
    Boost::boost
    p2p::p2p
    )