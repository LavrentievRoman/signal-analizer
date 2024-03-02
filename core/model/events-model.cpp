#include <array>

#include "core/model/events-model.h"

EventsModel::EventsModel(QObject *parent)
    : QAbstractTableModel(parent)
    , _rows()
{
}

EventsModel::~EventsModel()
{
}

int EventsModel::rowCount(const QModelIndex &parent) const
{
    return _rows.size();
}

int EventsModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant EventsModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        std::optional<QDateTime> dataOpt = _dataHelper(index);
        QString dataStr = dataOpt ? dataOpt.value().toString("hh:mm:ss") : "Error";
        return QVariant(dataStr);
    }

    if (role == Qt::ToolTipRole) {
        std::optional<QDateTime> dataOpt = _dataHelper(index);
        QString dataStr = dataOpt ? dataOpt.value().toString("dd.MM.yyyy hh:mm:ss") : "Error";
        return QVariant(dataStr);
    }

    return QVariant();
}

std::optional<QDateTime> EventsModel::_dataHelper(const QModelIndex &index) const
{
    // TODO: [Evgeny] looks like it never returns empty option
    return _rows.at(_rows.size() - index.row() - 1)->at(index.column());
}

QVariant EventsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal
            && section >= 0 && section < 3) {
        static const std::array<QVariant, 3> columnNames {
            tr("Время\nгенерации\nсобытия"),
            tr("Начало\nфрагмента"),
            tr("Конец\nфрагмента")
        };

        return columnNames[section];
    }

    return QVariant();
}

Qt::ItemFlags EventsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::ItemFlags();
    }

    return Qt::ItemIsEnabled;
}

void EventsModel::prependRow(Event &&row)
{
    beginInsertRows(QModelIndex(), 0, 0);
    _rows.emplace_back(std::make_unique<Event>(std::move(row)));
    endInsertRows();
}

void EventsModel::onReset()
{
    beginResetModel();
    _rows.clear();
    endResetModel();
}
