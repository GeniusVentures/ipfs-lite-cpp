
#ifndef CPP_IPFS_LITE_CLOCK_TIME_HPP
#define CPP_IPFS_LITE_CLOCK_TIME_HPP

#include <chrono>
#include <string>

#include "common/outcome.hpp"

namespace sgns::clock {
  enum class TimeFromStringError { INVALID_FORMAT = 1 };

  using UnixTime = std::chrono::seconds;
  using UnixTimeNano = std::chrono::nanoseconds;

  class Time {
   public:
    explicit Time(const UnixTimeNano &unix_time_nano);
    std::string time() const;
    UnixTime unixTime() const;
    UnixTimeNano unixTimeNano() const;
    bool operator<(const Time &other) const;
    bool operator==(const Time &other) const;
    static outcome::result<Time> fromString(const std::string &str);

   private:
    UnixTimeNano unix_time_nano_;
  };
}  // namespace sgns::clock

OUTCOME_HPP_DECLARE_ERROR_2(sgns::clock, TimeFromStringError);

#endif  // CPP_IPFS_LITE_CLOCK_TIME_HPP
