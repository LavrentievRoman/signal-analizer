#pragma once

#include <memory>
#include <optional>
#include <vector>

#include <QtCore/QAbstractTableModel>

#include "core/model/event.h"


class EventsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit EventsModel(QObject *parent = nullptr);
    virtual ~EventsModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void prependRow(Event &&row);

public slots:
    void onReset();

private:
    std::optional<QDateTime> _dataHelper(const QModelIndex &index) const;

private:
    std::vector<std::unique_ptr<Event>> _rows;

};
