#include "message_queue.hpp"

#include <libp2p/connection/stream.hpp>

#include <cassert>

namespace sgns::ipfs_lite::ipfs::graphsync
{

    MessageQueue::MessageQueue( StreamPtr stream, FeedbackFn feedback ) : feedback_( std::move( feedback ) )
    {
        assert( feedback_ );
        assert( stream );
        assert( !stream->isClosedForWrite() );

        state_.stream = std::move( stream );
        state_.active = true;
    }

    const MessageQueue::State &MessageQueue::getState() const
    {
        return state_;
    }

    void MessageQueue::enqueue( SharedData data )
    {
        if ( !data || data->empty() || !state_.active )
        {
            // ignore if inactive and do nothing with empty messages for consistency
            return;
        }

        if ( state_.writing_bytes > 0 )
        {
            // waiting for async write completion, enqueue
            state_.pending_bytes += data->size();
            pending_buffers_.emplace_back( std::move( data ) );
        }
        else
        {
            // begin async write
            beginWrite( std::move( data ) );
        }
    }

    void MessageQueue::clear()
    {
        state_.pending_bytes = 0;
        pending_buffers_.clear();
    }

    void MessageQueue::close()
    {
        clear();
        state_.active        = false;
        state_.writing_bytes = 0;
        state_.stream.reset();
    }

    void MessageQueue::dequeue()
    {
        assert( state_.active );

        if ( !pending_buffers_.empty() )
        {
            auto buffer = std::move( pending_buffers_.front() );
            pending_buffers_.pop_front();
            state_.pending_bytes -= buffer->size();
            beginWrite( std::move( buffer ) );
        }
    }

    void MessageQueue::beginWrite( SharedData buffer )
    {
        if ( !buffer )
        {
            logger()->error( "beginWrite: null buffer provided" );
            feedback_( state_.stream, Error::MESSAGE_WRITE_ERROR );
            return;
        }

        if ( !state_.stream )
        {
            logger()->error( "beginWrite: stream is null - peer likely disconnected" );
            feedback_( nullptr, Error::MESSAGE_WRITE_ERROR );
            return;
        }

        if ( state_.stream->isClosedForWrite() )
        {
            logger()->error( "beginWrite: stream is closed for write - peer likely disconnected" );
            feedback_( state_.stream, Error::MESSAGE_WRITE_ERROR );
            return;
        }

        state_.writing_bytes = buffer->size();

        logger()->trace( "beginWrite: writing to a stream with {} bytes", buffer->size() );

        // clang-format off
    state_.stream->write(
        *buffer,
        buffer->size(),
        [wptr{weak_from_this()}, buffer]
            (IPFS::outcome::result<size_t> result)
        {
          auto self = wptr.lock();
          if (self) {
            self->onMessageWritten(result);
          }
        }
    );
        // clang-format on
    }

    void MessageQueue::onMessageWritten( IPFS::outcome::result<size_t> res )
    {
        if ( !state_.active )
        {
            // dont need the result anymore
            return;
        }
        logger()->trace( "onMessageWritten: Writing data with {} bytes", state_.writing_bytes );

        auto n               = state_.writing_bytes;
        state_.writing_bytes = 0;

        if ( !res )
        {
            feedback_( state_.stream, res.error() );
            return;
        }

        if ( n != res.value() )
        {
            feedback_( state_.stream, Error::MESSAGE_WRITE_ERROR );
            return;
        }

        state_.total_bytes_written += n;

        dequeue();

        feedback_( state_.stream, IPFS::outcome::success() );
    }

}
