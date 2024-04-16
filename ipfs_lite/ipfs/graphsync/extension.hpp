
#ifndef CPP_IPFS_LITE_GRAPHSYNC_EXTENSION_HPP
#define CPP_IPFS_LITE_GRAPHSYNC_EXTENSION_HPP

#include <set>
#include <string>

#include "primitives/cid/cid.hpp"

namespace sgns::ipfs_lite::ipfs::graphsync {

  constexpr std::string_view kResponseMetadataProtocol =
      "graphsync/response-metadata";
  constexpr std::string_view kDontSendCidsProtocol =
      "graphsync/do-not-send-cids";

  /**
   * ExtensionData is a name/data pair for a graphsync extension
   */
  struct Extension {
    std::string name;
    std::vector<uint8_t> data;

    inline bool operator==(const Extension &other) const {
      return name == other.name && data == other.data;
    }
  };

  /// Returns "graphsync/response-metadata":true extension
  Extension encodeMetadataRequest();

  /// Response metadata indicates whether the responder has certain CIDs
  using ResponseMetadata = std::vector<std::pair<CID, bool>>;

  /// Encodes response metadata pairs
  Extension encodeResponseMetadata(const ResponseMetadata &metadata);

  /// Decodes metadata pairs
  IPFS::outcome::result<ResponseMetadata> decodeResponseMetadata(
      const Extension &extension);

  /// Encodes CIDS for "graphsync/do-not-send-cids" extension
  Extension encodeDontSendCids(const std::vector<CID> &dont_send_cids);

  /// Decodes CID subset not to be included into response
  IPFS::outcome::result<std::set<CID>> decodeDontSendCids(const Extension &extension);

}  // namespace sgns::ipfs_lite::ipfs::graphsync

#endif  // CPP_IPFS_LITE_GRAPHSYNC_EXTENSION_HPP
