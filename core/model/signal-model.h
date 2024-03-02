#pragma once

#include <memory>

#include <QtCore/QObject>

#include "core/model/signal-buffer.h"
#include "core/model/signal-fragment.h"

class SignalChunk;


class SignalModel : public QObject
{
    Q_OBJECT

public:
    explicit SignalModel(QObject *parent = nullptr);
    ~SignalModel() override;

    int unanalyzedSize() const;
    int size() const;
    int chunksCount() const;

    void moveAnalyzeCursor();

signals:
    void chunkCreated(const SignalChunk *);

    //TODO: [tvz] remove signals below if not needed
    void chunkChanged(const SignalChunk *);
    void chunkFull(SignalChunk *);
    void chunkRemoved(const SignalChunk *);

    void chunksReset();

public slots:
    void appendFragment(SignalFragment &);
    void setChunkLifetime(double lifetimeMsec);
    void onReset();
    void setChunkSize(quint32 newSize);

private:
    void _reset(double chunkLifetime, int chunkSize);
    void _connectBuffer();

private:
    std::unique_ptr<SignalBuffer> _buffer;
};
