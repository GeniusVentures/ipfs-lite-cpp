add_library(ipfs_kad_dht
	kademlia_dht.cpp
	)


target_link_libraries(ipfs_kad_dht
	p2p::p2p_kademlia
    )	