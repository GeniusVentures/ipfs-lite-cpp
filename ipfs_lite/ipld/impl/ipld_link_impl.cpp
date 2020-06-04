
#include "ipfs_lite/ipld/impl/ipld_link_impl.hpp"

namespace sgns::ipfs_lite::ipld {
  IPLDLinkImpl::IPLDLinkImpl(CID id, std::string name, size_t size)
      : cid_{std::move(id)}, name_{std::move(name)}, size_{size} {}

  const std::string &IPLDLinkImpl::getName() const {
    return name_;
  }

  const CID &IPLDLinkImpl::getCID() const {
    return cid_;
  }

  size_t IPLDLinkImpl::getSize() const {
    return size_;
  }
}  // namespace sgns::ipfs_lite::ipld
