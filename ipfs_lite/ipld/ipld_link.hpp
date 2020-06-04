#ifndef IPFS_LITE_IPLD_LINK_HPP
#define IPFS_LITE_IPLD_LINK_HPP

#include <string>
#include <vector>

#include "primitives/cid/cid.hpp"

namespace sgns::ipfs_lite::ipld {
  class IPLDLink {
   public:
    /**
     * @brief Destructor
     */
    virtual ~IPLDLink() = default;

    /**
     * @brief Get name of the link
     * @return Name, which should be unique per object
     */
    virtual const std::string &getName() const = 0;

    /**
     * @brief Get identifier of the target object
     * @return Content identifier
     */
    virtual const CID &getCID() const = 0;

    /**
     * @brief Get target object size
     * @return Cumulative size of the target object
     */
    virtual size_t getSize() const = 0;
  };
}  // namespace sgns::ipfs_lite::ipld

#endif
