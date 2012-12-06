/*
Copyright: 2012 LORDZOUGA <ozojiechikelu@gmail.com>
License: GPL-2+
 This package is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 .
 This package is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 .
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>

*/
#include "src/rlistmodel.h"

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
        if(!(QDir(path).exists())){
            return false;
        }
        if(RelaxEngine::getLiveMode()){
            RelaxEngine::getWatcher()->addPath(path);
        }
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

