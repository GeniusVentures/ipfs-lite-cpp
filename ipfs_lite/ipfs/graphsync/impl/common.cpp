#include "common.hpp"

OUTCOME_CPP_DEFINE_CATEGORY_3( sgns::ipfs_lite::ipfs::graphsync, Error, e )
{
    using E = sgns::ipfs_lite::ipfs::graphsync::Error;
    switch ( e )
    {
        case E::MESSAGE_PARSE_ERROR:
            return "message parse error";
        case E::MESSAGE_SIZE_OUT_OF_BOUNDS:
            return "message size out of bounds";
        case E::MESSAGE_VALIDATION_FAILED:
            return "message validation failed";
        case E::MESSAGE_SERIALIZE_ERROR:
            return "message serialize error";
        case E::STREAM_NOT_READABLE:
            return "stream is not readable";
        case E::MESSAGE_READ_ERROR:
            return "message read error";
        case E::STREAM_NOT_WRITABLE:
            return "stream is not writable";
        case E::WRITE_QUEUE_OVERFLOW:
            return "write queue overflow";
        case E::MESSAGE_WRITE_ERROR:
            return "message write error";
        default:
            break;
    }
    return "unknown error";
}

namespace sgns::ipfs_lite::ipfs::graphsync
{

    common::Logger logger()
    {
        // Leaked on purpose (defense-in-depth, not a fix for an observed
        // crash): a function-local static shared_ptr is destroyed by
        // __cxa_atexit in LIFO registration order, so it can die before
        // other components' exit-time teardown paths log — and any throw
        // out of a noexcept destructor terminates the process. A
        // process-lifetime logger has no ordering hazard. (The 2026-09-19
        // macOS app-quit SIGABRT originally suspected here was traced to
        // the embedding app tearing its GlobalDB down while node threads
        // were still live, and is fixed there.)
        static common::Logger *graphsync_logger = new common::Logger( common::createLogger( "graphsync" ) );
        return *graphsync_logger;
    }

    bool isTerminal( ResponseStatusCode code )
    {
        return code < 10 || code >= 20;
    }

    bool isSuccess( ResponseStatusCode code )
    {
        return code >= 20 && code <= 21;
    }

    bool isError( ResponseStatusCode code )
    {
        return code < 10 || code >= 30;
    }

    std::string statusCodeToString( ResponseStatusCode code )
    {
        switch ( code )
        {
            // clang-format off
#define CHECK_CASE(X) case RS_##X: return #X;
      CHECK_CASE(NO_PEERS)
      CHECK_CASE(CANNOT_CONNECT)
      CHECK_CASE(TIMEOUT)
      CHECK_CASE(CONNECTION_ERROR)
      CHECK_CASE(INTERNAL_ERROR)
      CHECK_CASE(REJECTED_LOCALLY)
      CHECK_CASE(REQUEST_ACKNOWLEDGED)
      CHECK_CASE(ADDITIONAL_PEERS)
      CHECK_CASE(NOT_ENOUGH_GAS)
      CHECK_CASE(OTHER_PROTOCOL)
      CHECK_CASE(PARTIAL_RESPONSE)
      CHECK_CASE(FULL_CONTENT)
      CHECK_CASE(PARTIAL_CONTENT)
      CHECK_CASE(REJECTED)
      CHECK_CASE(TRY_AGAIN)
      CHECK_CASE(REQUEST_FAILED)
      CHECK_CASE(LEGAL_ISSUES)
      CHECK_CASE(NOT_FOUND)
#undef CHECK_CASE
                // clang-format on

            default:
                break;
        }
        return "UNKNOWN";
    }

}
