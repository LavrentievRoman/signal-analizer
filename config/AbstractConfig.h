#pragma once

#include <memory>

#include <QObject>

class AbstractConfig : public QObject
{
    Q_OBJECT
public:
    explicit AbstractConfig(QObject *parent = nullptr);
    ~AbstractConfig() override = default;

    bool isSynchedWithFile() const;
    void setFile(const QString &file);

signals:
    void error(const QString &message);

public slots:
    virtual bool save();
    virtual bool load();

protected:
    virtual bool fromJsonObject(const QJsonObject &object) = 0;
    virtual bool toJsonObject(QJsonObject &object) const = 0;

protected:
    bool m_synchedWithFile = false;

private:
    QString m_filePath;
};

