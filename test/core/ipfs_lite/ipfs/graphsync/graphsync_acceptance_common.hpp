
#include <boost/asio.hpp>

#include <libp2p/host/host.hpp>

#include "ipfs_lite/ipfs/graphsync/graphsync.hpp"

namespace sgns::ipfs_lite::ipfs::graphsync::test {

  /// runs event loop for max_milliseconds or until SIGINT or SIGTERM
  void runEventLoop(const std::shared_ptr<boost::asio::io_context> &io,
                    size_t max_milliseconds);

  // Creates per-node objects using libp2p hos injector
  std::pair<std::shared_ptr<Graphsync>, std::shared_ptr<libp2p::Host>>
  createNodeObjects(std::shared_ptr<boost::asio::io_context> io);

  inline std::ostream& operator << (std::ostream& os, const CID& cid) {
    os << cid.toString().value();
    return os;
  }

  // MerkleDAG bridge interface for test purposes
  class TestDataService : public MerkleDagBridge {
   public:
    using Storage = std::map<CID, common::Buffer>;

    TestDataService &addData(const std::string &s) {
      insertNode(data_, s);
      return *this;
    }

    TestDataService &addExpected(const std::string &s) {
      insertNode(expected_, s);
      return *this;
    }

    const Storage &getData() const {
      return data_;
    }

    const Storage &getExpected() const {
      return expected_;
    }

    const Storage &getReceived() const {
      return received_;
    }

    // places into data_, returns true if expected
    bool onDataBlock(CID cid, common::Buffer data);

   private:
    static void insertNode(Storage &dst, const std::string &data_str);

    IPFS::outcome::result<size_t> select(
        const CID &cid,
        gsl::span<const uint8_t> selector,
        std::function<bool(const CID &cid, const common::Buffer &data)> handler)
    const override;

    Storage data_;
    Storage expected_;
    Storage received_;
  };

}  // namespace sgns::ipfs_lite::ipfs::graphsync::test
