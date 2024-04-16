
#ifndef CPP_IPFS_LITE_STORAGE_IPLD_WALKER_HPP
#define CPP_IPFS_LITE_STORAGE_IPLD_WALKER_HPP

#include <set>

#include "ipfs_lite/ipfs/datastore.hpp"

namespace sgns::ipfs_lite::ipld::walker {
  using codec::cbor::CborDecodeStream;
  using Ipld = ipfs::IpfsDatastore;

  // TODO(turuslan): implement selectors
  struct Selector {};

  struct Walker {
    Walker(Ipld &store) : store{store} {}

    IPFS::outcome::result<void> select(const CID &root, const Selector &selector);

    IPFS::outcome::result<void> recursiveAll(const CID &cid);

    void recursiveAll(CborDecodeStream &s);

    Ipld &store;
    std::set<CID> visited;
    std::vector<CID> cids;
  };
}  // namespace sgns::ipfs_lite::ipld::walker

#endif  // CPP_IPFS_LITE_STORAGE_IPLD_WALKER_HPP
