#include <QtCore/QDateTime>

#include "core/model/string-queue-model.h"

StringQueueModel::StringQueueModel(QObject *parent)
    : QAbstractListModel(parent)
    , _strings()
{
}

StringQueueModel::~StringQueueModel()
{
}

int StringQueueModel::rowCount(const QModelIndex &parent) const
{
    return _strings.size();
}

QVariant StringQueueModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        return _strings.at(_strings.size() - index.row() - 1);
    }

    return QVariant();
}

void StringQueueModel::prepend(const QString &str)
{
    int last = _strings.size() - 1;

    beginInsertRows(QModelIndex(), last, last);
    _strings.append(
        QDateTime::currentDateTime().toString("hh:mm:ss ") + str);
    endInsertRows();
}

void StringQueueModel::removeLast()
{
    if (_strings.isEmpty()) {
        return;
    }

    beginRemoveRows(QModelIndex(), 0, 0);
    _strings.removeFirst();
    endRemoveRows();
}
