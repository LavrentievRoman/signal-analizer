#include <numeric>
#include <cassert>

#include "core/model/signal-model.h"

SignalModel::SignalModel(QObject *parent)
    : QObject(parent)
    , _buffer(std::make_unique<SignalBuffer>(1.0, 1))
{
    _connectBuffer();
}

SignalModel::~SignalModel()
{
}

int SignalModel::unanalyzedSize() const
{
    return _buffer->unanalyzedSize();
}

int SignalModel::size() const
{
    return _buffer->size();
}

int SignalModel::chunksCount() const
{
    return _buffer->chunksCount();
}

void SignalModel::moveAnalyzeCursor()
{
    _buffer->moveAnalyzeCursor();
}

void SignalModel::appendFragment(SignalFragment &fragment)
{
    _buffer->append(fragment);
}

void SignalModel::setChunkLifetime(double lifetimeMsec)
{
    _buffer->setChunkLifetime(lifetimeMsec);
}

void SignalModel::onReset()
{
    _reset(_buffer->chunkLifetime(), _buffer->chunkSize());
}

void SignalModel::setChunkSize(quint32 newSize)
{
    // TODO: [Evgeny] revise requirements and deside if data resetion needed here or it doesn't.
    // It is possible to rearrange buffer here instead of reinitialization.
    _reset(_buffer->chunkLifetime(), newSize);
}

void SignalModel::_reset(double chunkLifetime, int chunkSize)
{
    emit chunksReset();
    _buffer = std::make_unique<SignalBuffer>(chunkLifetime, chunkSize);
    _connectBuffer();
}

void SignalModel::_connectBuffer()
{
    connect(_buffer.get(), &SignalBuffer::chunkCreated, this, &SignalModel::chunkCreated);
    connect(_buffer.get(), &SignalBuffer::chunkChanged, this, &SignalModel::chunkChanged);
    connect(_buffer.get(), &SignalBuffer::chunkFull,    this, &SignalModel::chunkFull);
    connect(_buffer.get(), &SignalBuffer::chunkRemoved, this, &SignalModel::chunkRemoved,
            Qt::DirectConnection);
}
