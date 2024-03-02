#include <cassert>

#include "core/model/signal-fragment.h"

#include "core/model/signal-buffer.h"

SignalBuffer::SignalBuffer(double timedeltaMSec, int chunkSize)
    : _chunkSize(chunkSize)
    , _chunks()
      // Chunk creation emit will be delayed until first append
    , _partialChunk(std::make_unique<SignalChunk>(chunkSize))
    , _lastAnalyzed(_chunks.cbegin())
    , m_chunkLifetimeMsecs(timedeltaMSec)
{
    assert(m_chunkLifetimeMsecs > 0.0);
}

int SignalBuffer::unanalyzedSize() const
{
    return std::distance(std::next(_lastAnalyzed), _chunks.cend())
           * _chunkSize + _partialChunk->size();
}

int SignalBuffer::size() const
{
    return _chunks.size() * _chunkSize + _partialChunk->size();
}

int SignalBuffer::chunksCount() const
{
    return _chunks.size() + 1;
}

int SignalBuffer::chunkSize() const
{
    return _chunkSize;
}

double SignalBuffer::chunkLifetime() const
{
    return m_chunkLifetimeMsecs;
}

void SignalBuffer::setChunkLifetime(double lifetimeMsecs)
{
    m_chunkLifetimeMsecs = lifetimeMsecs;
}

void SignalBuffer::moveAnalyzeCursor()
{
    _lastAnalyzed = std::next(_lastAnalyzed);
}

void SignalBuffer::append(const SignalFragment &fragment)
{
    // Fragment size may be less, equal or greater then _chunkSize.
    // _partialChunk always contains at least 1 value, except just after SignalBuffer init.
    assert(_chunks.empty() || !_partialChunk->isEmpty());
    assert(_partialChunk->size() < _chunkSize);
    assert(_partialChunk->hasNoLabel());

    if (_partialChunk->isEmpty()) { // Emit a signal that has been delayed since the constructor
        emit chunkCreated(_partialChunk.get());
    }

    for (const QPair<QDateTime, SignalValue> &signalValue : fragment) {
        double x = static_cast<double>(signalValue.first.toMSecsSinceEpoch());
        double y = signalValue.second;

        _partialChunk->append(x, y);

        if (_partialChunk->isFull()) {
            emit _partialChunk->sizeChanged(_partialChunk->size());
            emit chunkChanged(_partialChunk.get());
            emit chunkFull(_partialChunk.get());

            _moveFullChunk();
        }
    }

    emit _partialChunk->sizeChanged(_partialChunk->size());
    emit chunkChanged(_partialChunk.get());

    _removeOldChunks();
}

void SignalBuffer::_moveFullChunk()
{
    assert(_partialChunk->size() == _chunkSize);

    // Сopy last point to next chunk
    auto newChunk = std::make_unique<SignalChunk>(
                        _chunkSize,
                        _partialChunk->lastX(),
                        _partialChunk->lastY()
                    );
    _chunks.push_back(std::move(_partialChunk));
    _partialChunk = std::move(newChunk);

    emit chunkCreated(_partialChunk.get());
}

void SignalBuffer::_removeOldChunks()
{
    if (_partialChunk->isEmpty()) {
        assert(_chunks.empty());
        return;  // Nothing to remove
    }

    const double minTime = _partialChunk->lastX() - m_chunkLifetimeMsecs;

    for (CIter iter = _chunks.begin(); iter != _chunks.end() && (*iter)->lastX() < minTime; ) {
        emit chunkRemoved(iter->get());
        iter = _chunks.erase(iter);
    }
}
