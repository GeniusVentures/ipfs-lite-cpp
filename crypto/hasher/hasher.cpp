#include "hasher.hpp"

#include <libp2p/crypto/sha/sha256.hpp>
#include "crypto/blake2/blake2b160.hpp"

namespace sgns::crypto {

  std::map<IPFSHasher::HashType, IPFSHasher::HashMethod> &IPFSHasher::GetMethods()
  {
    static std::map<IPFSHasher::HashType, IPFSHasher::HashMethod> methods_{
        {HashType::sha256, IPFSHasher::sha2_256},
        {HashType::blake2b_256, IPFSHasher::blake2b_256}};
    return methods_;
  }

  IPFSHasher::Multihash IPFSHasher::calculate(HashType hash_type,
                                      gsl::span<const uint8_t> buffer) {
    HashMethod hash_method = GetMethods().at(hash_type);
    return std::invoke(hash_method, buffer);
  }

  IPFSHasher::Multihash IPFSHasher::sha2_256(gsl::span<const uint8_t> buffer) {
    auto digest = libp2p::crypto::sha256(buffer);
    BOOST_ASSERT_MSG(digest,
        "sgns::crypto::IPFSHasher - failed to generate sha2-256 hash value");

    auto multi_hash = Multihash::create(HashType::sha256, digest.value());
    BOOST_ASSERT_MSG(multi_hash.has_value(),
                     "sgns::crypto::IPFSHasher - failed to create sha2-256 hash");
    return multi_hash.value();
  }

  IPFSHasher::Multihash IPFSHasher::blake2b_256(gsl::span<const uint8_t> buffer) {
    auto digest = crypto::blake2b::blake2b_256(buffer);
    auto multi_hash = Multihash::create(HashType::blake2b_256, digest);
    BOOST_ASSERT_MSG(multi_hash.has_value(),
                     "sgns::crypto::IPFSHasher - failed to create blake2b_256 hash");
    return multi_hash.value();
  }
}  // namespace sgns::crypto
