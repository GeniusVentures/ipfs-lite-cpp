
add_library(cid
    cid.cpp
    json_codec.cpp
    )

target_link_libraries(cid
    Boost::boost
    blake2
    p2p::p2p_cid
    c-ares::cares
    soralog::soralog
    fmt::fmt
    cbor
)

add_library(comm_cid
    comm_cid.cpp
    comm_cid_errors.cpp
    )

target_link_libraries(comm_cid
    cid
    outcome
    )
