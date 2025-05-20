
#ifndef CPP_IPFS_LITE_IPFS_MERKLEDAG_LEAF_HPP
#define CPP_IPFS_LITE_IPFS_MERKLEDAG_LEAF_HPP

#include <functional>
#include <string_view>

#include "common/buffer.hpp"
#include "common/outcome.hpp"

namespace sgns::ipfs_lite::ipfs::merkledag {

/**
   * @enum Possible leaf errors
   */
enum class LeafError : int {
    LEAF_NOT_FOUND = 1,
    DUPLICATE_LEAF
};
}

OUTCOME_HPP_DECLARE_ERROR_2(sgns::ipfs_lite::ipfs::merkledag, LeafError)

namespace sgns::ipfs_lite::ipfs::merkledag {
class Leaf {
  public:
    /**
     * @brief Construct leaf
     * @param data - leaf content
     */
    explicit Leaf(common::Buffer data): content_(std::move(data)) {}

    /**
     * @brief Destructor
     */
    virtual ~Leaf() = default;

    /**
     * @brief Get leaf content
     * @return operation result
     */
    [[nodiscard]] const common::Buffer& content() const {
        return content_;
    }

    /**
     * @brief Get count of children leaves
     * @return Count of leaves
     */
    [[nodiscard]] size_t count() const {
        return children_.size();
    }

    /**
     * @brief Get children leaf
     * @param name - leaf name
     * @return operation result
     */
    [[nodiscard]] IPFS::outcome::result<std::reference_wrapper<const Leaf>> subLeaf(
        std::string_view name
    ) const {

        if (auto iter = children_.find(name); iter != children_.end()) {
            return iter->second;
        }
        return LeafError::LEAF_NOT_FOUND;
    }

    /**
     * @brief Get names of all sub leaves of the current leaf
     * @return operation result
     */
    [[nodiscard]] std::vector<std::string_view> getSubLeafNames() const {
        std::vector<std::string_view> names;
        for (const auto& child : children_) {
            names.push_back(child.first);
        }
        return names;
    }

    /**
     * @brief Insert children leaf
     * @param name - id of the leaf
     * @param children - leaf to insert
     * @return operation result
     */
    IPFS::outcome::result<void> insertSubLeaf(std::string name, Leaf children) {
        auto result = children_.emplace(std::move(name), std::move(children));
        if (result.second) {
            return IPFS::outcome::success();
        }
        return LeafError::DUPLICATE_LEAF;
    }

  private:
    common::Buffer content_;
    std::map<std::string, Leaf, std::less<>> children_;
};
}  // namespace sgns::ipfs_lite::ipfs::merkledag

#endif
