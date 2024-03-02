#pragma once

#include <QtCore/QAbstractListModel>
#include <QtCore/QStringList>


class StringQueueModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit StringQueueModel(QObject *parent = nullptr);
    virtual ~StringQueueModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public slots:
    void prepend(const QString &str);
    void removeLast();

private:
    QStringList _strings;

};
