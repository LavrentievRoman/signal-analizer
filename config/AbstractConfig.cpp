#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include "AbstractConfig.h"

AbstractConfig::AbstractConfig(QObject *parent)
    : QObject(parent)
    , m_synchedWithFile(false)
    , m_filePath()
{}

bool AbstractConfig::isSynchedWithFile() const
{
    return m_synchedWithFile;
}

void AbstractConfig::setFile(const QString &file)
{
    if (m_filePath != file) {
        m_filePath = file;
        m_synchedWithFile = false;
    }
}

bool AbstractConfig::save()
{
    // Config file may be changed by someone, so should save even when m_synchedWithFile is true

    if (m_filePath.isEmpty()) {
        emit error(tr("AbstractConfig::save: filePath is empty"));
        return false;
    }

    QFile file(m_filePath);
    if (!file.open(QFile::WriteOnly)) {
        emit error(tr("AbstractConfig::save: can't open file \"%1\"").arg(m_filePath));
        return false;
    }

    QJsonObject object;
    if (!toJsonObject(object)) {
        error(tr("AbstractConfig::save: error"));
        return false;
    }

    QJsonDocument document;
    document.setObject(object);
    file.write(document.toJson(QJsonDocument::JsonFormat::Indented));

    m_synchedWithFile = true;
    return true;
}

bool AbstractConfig::load()
{
    // Config file may be changed by someone, so should load even when m_synchedWithFile is true

    if (m_filePath.isEmpty()) {
        emit error(tr("AbstractConfig::load: filePath is empty"));
        return false;
    }

    QFile file(m_filePath);
    if (!file.open(QFile::ReadOnly)) {
        emit error(tr("AbstractConfig::load: can't open file \"%1\"").arg(m_filePath));
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit error(tr("AbstractConfig::load: parseError: %1").arg(parseError.errorString()));
        return false;
    }

    if (!fromJsonObject(document.object())) {
        error(tr("AbstractConfig::load: error"));
        return false;
    }

    m_synchedWithFile = true;
    return true;
}
