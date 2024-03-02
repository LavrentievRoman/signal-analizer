#pragma once

#include <list>
#include <memory>

#include <QtCore/QObject>

#include "core/model/signal-chunk.h"

class SignalFragment;


class SignalBuffer : public QObject
{
    Q_OBJECT

public:
    explicit SignalBuffer(double chunkLifetime, int chunkSize);
    ~SignalBuffer() override = default;

    // Returns count of unanalyzed signal points.
    int unanalyzedSize() const;

    // Returns total count of signal points.
    int size() const;

    // Returns total count of chunks
    int chunksCount() const;

    int chunkSize() const;

    double chunkLifetime() const;
    void setChunkLifetime(double lifetimeMsecs);

    void moveAnalyzeCursor();

    void append(const SignalFragment &fragment);

signals:
    void chunkCreated(const SignalChunk *);

    //TODO: [tvz] remove signals below if not needed
    void chunkChanged(const SignalChunk *);
    void chunkFull(SignalChunk *);
    void chunkRemoved(const SignalChunk *);

    void error(const QString &);

private:
    void _moveFullChunk();
    void _removeOldChunks();

private:
    // std::list must have a non-const value_type,
    // so use non-const SignalChunk values.
    typedef std::list<std::unique_ptr<SignalChunk>> LinkedList;
    typedef LinkedList::const_iterator CIter;

    // All _chunks elements has size equals to _chunkSize.
    // _chunkSize is defined in SignalBuffer constructor and constant for SignalBuffer instance.
    // Only way to change _chunkSize is to create new SignalBuffer instance.
    // _partialChunk is not null and points to one chunk.
    // _partialChunk always has size less then _chunkSize.
    // _partialChunk is always latest chunk.
    // _partialChunk can grow and can be moved to _chunks, when it's size reaches _chunkSize.
    // _chunks elements never changes.
    const int _chunkSize;
    LinkedList _chunks;
    std::unique_ptr<SignalChunk> _partialChunk;

    CIter _lastAnalyzed;
    double m_chunkLifetimeMsecs;
};
