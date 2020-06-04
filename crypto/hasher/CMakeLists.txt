add_library(ipfs_lite_hasher
    hasher.cpp
    )
target_link_libraries(ipfs_lite_hasher
    blake2
    )
