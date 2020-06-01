#ifndef CPP_IPFS_LITE__CODEC_CBOR_CBOR_HPP
#define CPP_IPFS_LITE__CODEC_CBOR_CBOR_HPP

#include "codec/cbor/cbor_decode_stream.hpp"
#include "codec/cbor/cbor_encode_stream.hpp"
#include "codec/cbor/cbor_resolve.hpp"
#include "common/buffer.hpp"

namespace sgns::codec::cbor {
  using common::Buffer;

  /**
   * @brief CBOR encoding to byte-vector
   * @tparam Type to be encoded
   * @param arg data to be encoded
   * @return encoded data
   */
  template <typename T>
  outcome::result<Buffer> encode(const T &arg) {
    try {
      CborEncodeStream encoder;
      encoder << arg;
      return Buffer{encoder.data()};
    } catch (std::system_error &e) {
      return outcome::failure(e.code());
    }
  }

  /**
   * @brief CBOR decoding from byte-vector
   * @tparam T - type of the value to decode
   * @param input - data to decode
   * @return operation result
   * @see cbor_errors.hpp for possible error cases
   */
  template <typename T>
  outcome::result<T> decode(gsl::span<const uint8_t> input) {
    try {
      T data{};
      CborDecodeStream decoder(input);
      decoder >> data;
      return data;
    } catch (std::system_error &e) {
      return outcome::failure(e.code());
    }
  }
}  // namespace sgns::codec::cbor

#endif  // CPP_IPFS_LITE__CODEC_CBOR_CBOR_HPP
