add_library(cbor_stream
    cbor_buffering.cpp
    cbor_stream.cpp
    )
target_link_libraries(cbor_stream
    cbor
    p2p::p2p_event
)
