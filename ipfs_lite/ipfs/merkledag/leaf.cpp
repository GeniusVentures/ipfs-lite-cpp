#include "leaf.hpp"

OUTCOME_CPP_DEFINE_CATEGORY_3(sgns::ipfs_lite::ipfs::merkledag, LeafError, e) {
    using sgns::ipfs_lite::ipfs::merkledag::LeafError;
    switch (e) {
        case (LeafError::LEAF_NOT_FOUND):
            return "MerkleDAG leaf: children leaf not found";
        case (LeafError::DUPLICATE_LEAF):
            return "MerkleDAG leaf: duplicate leaf name";
    }
    return "MerkleDAG leaf: unknown error";
}
