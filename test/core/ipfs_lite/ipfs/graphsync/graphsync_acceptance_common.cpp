#include "graphsync_acceptance_common.hpp"

#include <boost/di/extension/scopes/shared.hpp>

#include <libp2p/injector/host_injector.hpp>
#include <libp2p/protocol/common/asio/asio_scheduler.hpp>

#include "ipfs_lite/ipfs/graphsync/impl/graphsync_impl.hpp"
#include "ipfs_lite/ipld/impl/ipld_node_impl.hpp"

namespace sgns::ipfs_lite::ipfs::graphsync::test {

  void runEventLoop(const std::shared_ptr<boost::asio::io_context> &io,
                    size_t max_milliseconds) {
    boost::asio::signal_set signals(*io, SIGINT, SIGTERM);

    // io->run() can exit if we're not waiting for anything
    signals.async_wait(
        [&io](const boost::system::error_code &, int) { io->stop(); });

    if (max_milliseconds > 0) {
      io->run_for(std::chrono::milliseconds(max_milliseconds));
    } else {
      io->run();
    }
  }

  std::pair<std::shared_ptr<Graphsync>, std::shared_ptr<libp2p::Host>>
  createNodeObjects(std::shared_ptr<boost::asio::io_context> io) {

    // [boost::di::override] allows for creating multiple hosts for testing
    // purposes
    auto injector =
        libp2p::injector::makeHostInjector<boost::di::extension::shared_config>(
            boost::di::bind<boost::asio::io_context>.to(
                io)[boost::di::override]);

    std::pair<std::shared_ptr<Graphsync>, std::shared_ptr<libp2p::Host>>
        objects;
    objects.second = injector.template create<std::shared_ptr<libp2p::Host>>();
    auto scheduler = std::make_shared<libp2p::protocol::AsioScheduler>(
        io, libp2p::protocol::SchedulerConfig{});
    objects.first =
        std::make_shared<GraphsyncImpl>(objects.second, std::move(scheduler));
    return objects;
  }

  bool TestDataService::onDataBlock(CID cid, common::Buffer data) {
    bool expected = false;
    auto it = expected_.find(cid);
    if (it != expected_.end() && it->second == data) {
      expected = (received_.count(cid) == 0);
    }
    received_[cid] = std::move(data);
    return expected;
  }

  void TestDataService::insertNode(TestDataService::Storage &dst,
                                   const std::string &data_str) {
    using NodeImpl = sgns::ipfs_lite::ipld::IPLDNodeImpl;
    auto node = NodeImpl::createFromString(data_str);
    dst[node->getCID()] = node->getRawBytes();
  }

  IPFS::outcome::result<size_t> TestDataService::select(
      const CID &cid,
      gsl::span<const uint8_t> selector,
      std::function<bool(const CID &cid, const common::Buffer &data)> handler)
      const {
    auto it = data_.find(cid);
    if (it != data_.end()) {
      handler(it->first, it->second);
      return 1;
    }
    return 0;
  }

}  // namespace sgns::ipfs_lite::ipfs::graphsync::test
