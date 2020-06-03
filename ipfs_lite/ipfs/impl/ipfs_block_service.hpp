
#ifndef CPP_IPFS_LITE_IPFS_BLOCKSERVICE_IMPL_HPP
#define CPP_IPFS_LITE_IPFS_BLOCKSERVICE_IMPL_HPP

#include <memory>

#include "ipfs_lite/ipfs/datastore.hpp"

namespace sgns::ipfs_lite::ipfs {
  class IpfsBlockService : public IpfsDatastore {
   public:
    /**
     * @brief Construct IPFS storage
     * @param data_store - IPFS storage implementation
     */
    explicit IpfsBlockService(std::shared_ptr<IpfsDatastore> data_store);

    outcome::result<bool> contains(const CID &key) const override;

    outcome::result<void> set(const CID &key, Value value) override;

    outcome::result<Value> get(const CID &key) const override;

    outcome::result<void> remove(const CID &key) override;

   private:
    std::shared_ptr<IpfsDatastore> local_storage_; /**< Local data storage */
  };
}  // namespace sgns::ipfs_lite::ipfs

#endif
