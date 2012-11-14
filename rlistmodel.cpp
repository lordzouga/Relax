#include "rlistmodel.h"

RListModel::RListModel(QObject *parent) :
    QAbstractListModel(parent)
{
    paths = RelaxEngine::getBaseFilePaths();
}

QVariant RListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(index.row() >= paths->size() || index.row() < 0)
        return QVariant();

    if(role == Qt::DisplayRole){
        return paths->at(index.row());
    }

    return QVariant();
}

int RListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    //return engine.getBaseFilePaths().size();
    return paths->size();
}

QVariant RListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Horizontal){

        switch (section) {
        case 0:
            return tr("Folder(s) to Watch");

        default:
            return QVariant();
    }
    }
    return QVariant();
}

bool RListModel::addFilePath(const QString &path)
{
    if(!paths->contains(path)){
        RelaxEngine::getWatcher()->addPath(path);
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        paths->append(path);
        endInsertRows();
        return true;
    }

    return false;
}

void RListModel::removeFilePath(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    if(index.row() > paths->size() || index.row() < 0)
        return;

    RelaxEngine::getWatcher()->removePath(paths->at(index.row()));

    beginRemoveRows(QModelIndex(), index.row(), index.row());
    paths->removeAt(index.row());
    endRemoveRows();
}

