
#include "ipfs_lite/ipfs/impl/in_memory_datastore.hpp"
namespace sgns::ipfs_lite::ipfs {
using sgns::ipfs_lite::ipfs::InMemoryDatastore;
using Value = sgns::ipfs_lite::ipfs::IpfsDatastore::Value;

    sgns::outcome::result<bool> InMemoryDatastore::contains(const CID &key) const {
    return storage_.find(key) != storage_.end();
    }

    sgns::outcome::result<void> InMemoryDatastore::set(const CID &key, Value value) {
    storage_[key] = value;
    return sgns::outcome::success();
    }

    sgns::outcome::result<Value> InMemoryDatastore::get(const CID &key) const {
    OUTCOME_TRY((auto &&, found), contains(key));
    if (!found) return IpfsDatastoreError::NOT_FOUND;
    return storage_.at(key);
    }

    sgns::outcome::result<void> InMemoryDatastore::remove(const CID &key) {
    storage_.erase(key);
    return sgns::outcome::success();
    }
}