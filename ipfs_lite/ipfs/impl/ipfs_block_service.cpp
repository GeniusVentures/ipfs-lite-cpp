#include "ipfs_lite/ipfs/impl/ipfs_block_service.hpp"

namespace sgns::ipfs_lite::ipfs {
  IpfsBlockService::IpfsBlockService(std::shared_ptr<IpfsDatastore> data_store)
      : local_storage_{std::move(data_store)} {
    BOOST_ASSERT_MSG(local_storage_ != nullptr,
                     "IPFS block service: invalid local storage");
  }

  IPFS::outcome::result<bool> IpfsBlockService::contains(const CID &key) const {
    return local_storage_->contains(key);
  }

  IPFS::outcome::result<void> IpfsBlockService::set(const CID &key, Value value) {
    auto result = local_storage_->set(key, std::move(value));
    if (result.has_error()) return result.error();
    return IPFS::outcome::success();
  }

  IPFS::outcome::result<IpfsBlockService::Value> IpfsBlockService::get(
      const CID &key) const {
    OUTCOME_TRY((auto &&, data), local_storage_->get(key));
    return std::move(data);
  }

  IPFS::outcome::result<void> IpfsBlockService::remove(const CID &key) {
    return local_storage_->remove(key);
  }
}  // namespace sgns::ipfs_lite::ipfs
