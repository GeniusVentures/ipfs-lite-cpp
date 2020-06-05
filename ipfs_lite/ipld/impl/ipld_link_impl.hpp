#ifndef IPFS_LITE_IPLD_LINK_IMPL_HPP
#define IPFS_LITE_IPLD_LINK_IMPL_HPP

#include <string>
#include <utility>
#include <vector>

#include "ipfs_lite/ipld/ipld_link.hpp"

namespace sgns::ipfs_lite::ipld {
  class IPLDLinkImpl : public IPLDLink {
   public:
    /**
     * @brief Construct Link
     * @param id - CID of the target object
     * @param name - name of the target object
     * @param size - total size of the target object
     */
    IPLDLinkImpl(CID id, std::string name, size_t size);

    IPLDLinkImpl() = default;

    const std::string &getName() const override;

    const CID &getCID() const override;

    size_t getSize() const override;

   private:
    CID cid_;
    std::string name_;
    size_t size_{};
  };
}  // namespace sgns::ipfs_lite::ipld

#endif
