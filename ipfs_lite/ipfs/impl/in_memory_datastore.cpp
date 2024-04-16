
#include "ipfs_lite/ipfs/impl/in_memory_datastore.hpp"
namespace sgns::ipfs_lite::ipfs {
using sgns::ipfs_lite::ipfs::InMemoryDatastore;
using Value = sgns::ipfs_lite::ipfs::IpfsDatastore::Value;

    sgns::IPFS::outcome::result<bool> InMemoryDatastore::contains(const CID &key) const {
    return storage_.find(key) != storage_.end();
    }

    sgns::IPFS::outcome::result<void> InMemoryDatastore::set(const CID &key, Value value) {
    storage_[key] = value;
    return sgns::IPFS::outcome::success();
    }

    sgns::IPFS::outcome::result<Value> InMemoryDatastore::get(const CID &key) const {
    OUTCOME_TRY((auto &&, found), contains(key));
    if (!found) return IpfsDatastoreError::NOT_FOUND;
    return storage_.at(key);
    }

    sgns::IPFS::outcome::result<void> InMemoryDatastore::remove(const CID &key) {
    storage_.erase(key);
    return sgns::IPFS::outcome::success();
    }
}