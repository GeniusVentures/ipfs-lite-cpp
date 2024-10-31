#ifndef IPFS_PUBSUB_KAD
#define IPFS_PUBSUB_KAD

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include "libp2p/protocol/kademlia/kademlia.hpp"
#include "libp2p/multi/content_identifier_codec.hpp"
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>

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
			std::vector<std::string> bootstrapAddresses,
			std::shared_ptr<boost::asio::io_context> io_context);

		void Start();

		void bootstrap();

		bool FindProviders(
			const libp2p::multi::ContentIdentifier& cid,
			std::function<void(libp2p::outcome::result<std::vector<libp2p::peer::PeerInfo>> onProvidersFound)> onProvidersFound);

		bool FindProviders(
			const libp2p::protocol::kademlia::ContentId& key,
			std::function<void(libp2p::outcome::result<std::vector<libp2p::peer::PeerInfo>> onProvidersFound)> onProvidersFound);

		void FindPeer(
			const libp2p::peer::PeerId& peerId,
			std::function<void(libp2p::outcome::result<libp2p::peer::PeerInfo>)> onPeerFound);

		void ProvideCID(
			libp2p::protocol::kademlia::ContentId key,
			bool need_err,
			bool force = false
		);
	private:
		void ScheduleProvideCID(libp2p::protocol::kademlia::ContentId key, bool need_err);
		std::vector<libp2p::peer::PeerInfo> GetBootstrapNodes() const;

		std::shared_ptr<libp2p::protocol::kademlia::Kademlia> kademlia_;
		std::vector<std::string> bootstrapAddresses_;
		std::shared_ptr<boost::asio::io_context> io_context_;
		boost::asio::steady_timer timer_;
	};
}

#endif