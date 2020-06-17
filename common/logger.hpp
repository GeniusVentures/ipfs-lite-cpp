
#ifndef CPP_IPFS_LITE_LOGGER_HPP
#define CPP_IPFS_LITE_LOGGER_HPP



#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

namespace sgns {
    namespace common {
        using Logger = std::shared_ptr<spdlog::logger>;

        /**
         * Provide logger object
         * @param tag - tagging name for identifying logger
         * @return logger object
         */
        Logger createLogger(const std::string &tag);
    }
}  // namespace sgns::common

#endif  // CPP_IPFS_LITE_LOGGER_HPP
