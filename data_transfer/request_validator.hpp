
#ifndef CPP_IPFS_LITE_REQUEST_VALIDATOR_HPP
#define CPP_IPFS_LITE_REQUEST_VALIDATOR_HPP

#include <libp2p/peer/peer_id.hpp>
#include "common/outcome.hpp"
#include "primitives/cid/cid.hpp"
#include "ipfs_lite/ipld/ipld_node.hpp"

namespace sgns::data_transfer {

  using libp2p::peer::PeerId;
  using ipfs_lite::ipld::IPLDNode;

  /**
   * RequestValidator is an interface implemented by the client of the data
   * transfer module to validate requests
   */
  class RequestValidator {
   public:
    virtual ~RequestValidator() = default;

    /**
     * ValidatePush validates a push request received from the peer that will
     * send data
     */
    virtual outcome::result<void> validatePush(
        const PeerId &sender,
        std::vector<uint8_t> voucher,
        CID base_cid,
        std::shared_ptr<IPLDNode> selector) = 0;
    /**
     * ValidatePull validates a pull request received from the peer thatF will
     * receive data
     */
    virtual outcome::result<void> validatePull(
        const PeerId &receiver,
        std::vector<uint8_t> voucher,
        CID base_cid,
        std::shared_ptr<IPLDNode> selector) = 0;
  };

}  // namespace sgns::data_transfer

#endif  // CPP_IPFS_LITE_REQUEST_VALIDATOR_HPP
