
#ifndef CPP_IPFS_LITE_IPFS_IMPL_IN_MEMORY_DATASTORE_HPP
#define CPP_IPFS_LITE_IPFS_IMPL_IN_MEMORY_DATASTORE_HPP

#include <map>

#include "ipfs_lite/ipfs/datastore.hpp"

namespace sgns::ipfs_lite::ipfs {

  class InMemoryDatastore : public IpfsDatastore {
   public:
    ~InMemoryDatastore() override = default;

    /** @copydoc IpfsDatastore::contains() */
    outcome::result<bool> contains(const CID &key) const override;

    /** @copydoc IpfsDatastore::set() */
    outcome::result<void> set(const CID &key, Value value) override;

    /** @copydoc IpfsDatastore::get() */
    outcome::result<Value> get(const CID &key) const override;

    /** @copydoc IpfsDatastore::remove() */
    outcome::result<void> remove(const CID &key) override;

   private:
    std::map<CID, Value> storage_;
  };

}  // namespace sgns::ipfs_lite::ipfs

#endif  // CPP_IPFS_LITE_IPFS_IMPL_IN_MEMORY_DATASTORE_HPP
