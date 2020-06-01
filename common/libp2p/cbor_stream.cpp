
#include "common/libp2p/cbor_stream.hpp"

namespace sgns::common::libp2p {
  CborStream::CborStream(std::shared_ptr<Stream> stream)
      : stream_{std::move(stream)} {}

  std::shared_ptr<CborStream::Stream> CborStream::stream() const {
    return stream_;
  }

  void CborStream::readRaw(ReadCallbackFunc cb) {
    buffering_.reset();
    buffer_.erase(buffer_.begin(), buffer_.begin() + size_);
    size_ = 0;
    consume(buffer_, std::move(cb));
  }

  void CborStream::writeRaw(gsl::span<const uint8_t> input,
                            WriteCallbackFunc cb) {
    stream_->write(input, input.size(), std::move(cb));
  }

  void CborStream::readMore(ReadCallbackFunc cb) {
    if (buffering_.done()) {
      return cb(gsl::make_span(buffer_).subspan(0, size_));
    }
    buffer_.resize(size_ + kReserveBytes);
    stream_->readSome(
        gsl::make_span(buffer_).subspan(size_),
        kReserveBytes,
        [cb{std::move(cb)}, self{shared_from_this()}](auto count) {
          if (!count) {
            return cb(count.error());
          }
          self->buffer_.resize(self->size_ + count.value());
          self->consume(gsl::make_span(self->buffer_).subspan(self->size_),
                        std::move(cb));
        });
  }

  void CborStream::consume(gsl::span<uint8_t> input, ReadCallbackFunc cb) {
    auto consumed = buffering_.consume(input);
    if (!consumed) {
      return cb(consumed.error());
    }
    size_ += consumed.value();
    readMore(std::move(cb));
  }
}  // namespace sgns::common::libp2p
