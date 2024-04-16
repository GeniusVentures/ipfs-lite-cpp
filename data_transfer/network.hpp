
#ifndef CPP_IPFS_LITE_DATA_TRANSFER_NETWORK_HPP
#define CPP_IPFS_LITE_DATA_TRANSFER_NETWORK_HPP

#include <libp2p/connection/stream.hpp>
#include <libp2p/host/host.hpp>
#include <libp2p/peer/protocol.hpp>
#include "codec/cbor/cbor.hpp"
#include "data_transfer/impl/stream_message_sender.hpp"
#include "data_transfer/message.hpp"
#include "data_transfer/message_receiver.hpp"
#include "data_transfer/message_sender.hpp"

namespace sgns::data_transfer {

  using libp2p::peer::PeerId;

  /** The protocol identifier for graphsync messages */
  const libp2p::peer::Protocol kDataTransferLibp2pProtocol =
      "/fil/datatransfer/1.0.0";

  /**
   * DataTransferNetwork provides interface for network connectivity for
   * GraphSync
   */
  class DataTransferNetwork {
   public:
    virtual ~DataTransferNetwork() = default;

    /**
     * Registers the Receiver to handle messages received from the network
     */
    virtual IPFS::outcome::result<void> setDelegate(
        std::shared_ptr<MessageReceiver> receiver) = 0;

    /**
     * Establishes a connection to the given peer
     */
    virtual IPFS::outcome::result<void> connectTo(const PeerId &peer) = 0;

    virtual IPFS::outcome::result<std::shared_ptr<MessageSender>> newMessageSender(
        const PeerId &peer) = 0;
  };

}  // namespace sgns::data_transfer

#endif  // CPP_IPFS_LITE_DATA_TRANSFER_NETWORK_HPP
