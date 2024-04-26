#ifndef IPFS_PUBSUB_KAD
#define IPFS_PUBSUB_KAD

#include <vector>
#include <string>
#include <memory>
#include "libp2p/protocol/kademlia/kademlia.hpp"

namespace sgns::ipfs_lite::ipfs::dht
{
    /**
	 * This class creates an DHT for finding peers with CIDs we want
	 * from IPFS node(s).
	 */
	class IpfsDHT
	{
	public:
		IpfsDHT(
			std::shared_ptr<libp2p::protocol::kademlia::Kademlia> kademlia,
			std::vector<std::string> bootstrapAddresses);

		void Start();

		bool FindProviders(
			const libp2p::multi::ContentIdentifier& cid,
			std::function<void(libp2p::outcome::result<std::vector<libp2p::peer::PeerInfo>> onProvidersFound)> onProvidersFound);

		void FindPeer(
			const libp2p::peer::PeerId& peerId,
			std::function<void(libp2p::outcome::result<libp2p::peer::PeerInfo>)> onPeerFound);
	private:
		std::vector<libp2p::peer::PeerInfo> GetBootstrapNodes() const;

		std::shared_ptr<libp2p::protocol::kademlia::Kademlia> kademlia_;
		std::vector<std::string> bootstrapAddresses_;
	};
}

#endif