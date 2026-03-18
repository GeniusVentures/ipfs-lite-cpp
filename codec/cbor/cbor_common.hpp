#pragma once

#include <cstdint>

#include <boost/optional.hpp>

#include "codec/cbor/cbor_errors.hpp"
#include "common/outcome.hpp"
#include "primitives/cid/cid.hpp"

namespace sgns::codec::cbor
{
    constexpr uint64_t kCidTag = 42;
}
