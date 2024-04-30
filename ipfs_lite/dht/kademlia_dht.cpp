#include "kademlia_dht.hpp"

namespace sgns::ipfs_lite::ipfs::dht
{
    IpfsDHT::IpfsDHT(
        std::shared_ptr<libp2p::protocol::kademlia::Kademlia> kademlia,
        std::vector<std::string> bootstrapAddresses)
        : kademlia_(std::move(kademlia))
        , bootstrapAddresses_(bootstrapAddresses)
    {
    }

    void IpfsDHT::Start()
    {
        auto&& bootstrapNodes = GetBootstrapNodes();
        for (auto& bootstrap_node : bootstrapNodes) {
            kademlia_->addPeer(bootstrap_node, true);
        }
        
        kademlia_->start();
    }

    bool IpfsDHT::FindProviders(
        const libp2p::multi::ContentIdentifier& cid,
        std::function<void(libp2p::outcome::result<std::vector<libp2p::peer::PeerInfo>> onProvidersFound)> onProvidersFound)
    {
        std::cout << "find providers" << std::endl;
        auto kadCID = libp2p::protocol::kademlia::ContentId::fromWire(
            libp2p::multi::ContentIdentifierCodec::encode(cid).value());
        if (!kadCID)
        {
            std::cerr << "Wrong CID" << std::endl;
            return false;
            // TODO: pass an error to callback
            //onProvidersFound(ERROR);
        }
        else
        {
            std::cout << "actually find providers" << std::endl;
            kademlia_->bootstrap();
            [[maybe_unused]] auto res = kademlia_->findProviders(
                kadCID.value(), 0, onProvidersFound);
            return true;
        }
    }

    bool IpfsDHT::FindProviders(
        const libp2p::protocol::kademlia::ContentId& key,
        std::function<void(libp2p::outcome::result<std::vector<libp2p::peer::PeerInfo>> onProvidersFound)> onProvidersFound)
    {
        //std::cout << "actually find providers" << std::endl;
        kademlia_->bootstrap();
        [[maybe_unused]] auto res = kademlia_->findProviders(
            key, 0, onProvidersFound);
        return true;
        
    }

    std::vector<libp2p::peer::PeerInfo> IpfsDHT::GetBootstrapNodes() const
    {
        std::unordered_map<libp2p::peer::PeerId,
            std::vector<libp2p::multi::Multiaddress>>
            addresses_by_peer_id;

        for (auto& address : bootstrapAddresses_) {
            auto ma = libp2p::multi::Multiaddress::create(address).value();
            auto peer_id_base58 = ma.getPeerId().value();
            auto peer_id = libp2p::peer::PeerId::fromBase58(peer_id_base58).value();

            addresses_by_peer_id[std::move(peer_id)].emplace_back(std::move(ma));
        }

        std::vector<libp2p::peer::PeerInfo> v;
        v.reserve(addresses_by_peer_id.size());
        for (auto& i : addresses_by_peer_id) {
            v.emplace_back(libp2p::peer::PeerInfo{
                /*.id =*/ i.first, /*.addresses =*/ {std::move(i.second)} });
        }

        return v;
    }

    void IpfsDHT::FindPeer(
        const libp2p::peer::PeerId& peerId,
        std::function<void(libp2p::outcome::result<libp2p::peer::PeerInfo>)> onPeerFound)
    {
        [[maybe_unused]] auto res = kademlia_->findPeer(peerId, onPeerFound);
    }

    void IpfsDHT::ProvideCID(
        libp2p::protocol::kademlia::ContentId key,
        bool need_error
    )
    {
        kademlia_->provide(key,need_error);
        //if(provide.has_error())
        //{

        //}
    }
}