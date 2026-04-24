#include "leaf.hpp"

namespace sgns::ipfs_lite::ipfs::merkledag
{
    Leaf::Leaf( common::Buffer data ) : content_{ std::move( data ) } {}

    IPFS::outcome::result<std::reference_wrapper<const Leaf>> Leaf::subLeaf( std::string_view name ) 
    {
        if ( auto iter = children_.find( name ); iter != children_.end() )
        {
            return iter->second;
        }
        return LeafError::LEAF_NOT_FOUND;
    }

    IPFS::outcome::result<void> Leaf::insertSubLeaf( std::string name, Leaf children )
    {
        auto result = children_.emplace( std::move( name ), std::move( children ) );
        if ( result.second )
        {
            return IPFS::outcome::success();
        }
        return LeafError::DUPLICATE_LEAF;
    }
}

OUTCOME_CPP_DEFINE_CATEGORY_3( sgns::ipfs_lite::ipfs::merkledag, LeafError, e )
{
    using sgns::ipfs_lite::ipfs::merkledag::LeafError;
    switch ( e )
    {
        case ( LeafError::LEAF_NOT_FOUND ):
            return "MerkleDAG leaf: children leaf not found";
        case ( LeafError::DUPLICATE_LEAF ):
            return "MerkleDAG leaf: duplicate leaf name";
    }
    return "MerkleDAG leaf: unknown error";
}
