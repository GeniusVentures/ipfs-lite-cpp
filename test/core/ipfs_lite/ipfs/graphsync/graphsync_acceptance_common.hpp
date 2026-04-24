#include <boost/asio.hpp>

#include <libp2p/host/host.hpp>

#include "ipfs_lite/ipfs/graphsync/graphsync.hpp"

namespace sgns::ipfs_lite::ipfs::graphsync::test
{

    /// runs event loop for max_milliseconds or until SIGINT or SIGTERM
    void runEventLoop( const std::shared_ptr<boost::asio::io_context> &io, size_t max_milliseconds );

    // Creates per-node objects using libp2p hos injector
    std::pair<std::shared_ptr<Graphsync>, std::shared_ptr<libp2p::Host>> createNodeObjects(
        std::shared_ptr<boost::asio::io_context> io );

    inline std::ostream &operator<<( std::ostream &os, const CID &cid )
    {
        os << cid.toString().value();
        return os;
    }
}
