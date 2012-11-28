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
#include "src/rtablemodel.h"

#define COLUMN_SIZE 2

RTableModel::RTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    pairs = RelaxEngine::getFilterPairs();
}

int RTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return pairs->size();
}

int RTableModel::columnCount(const QModelIndex &parent = QModelIndex()) const
{
    Q_UNUSED(parent);
    return COLUMN_SIZE;
}

QVariant RTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    //QList<FilterPair> pairs = engine.getFilterPairs();

    if(index.row() >= pairs->size() || index.row() < 0)
        return QVariant();



    if(role == Qt::DisplayRole){

        FilterPair aPair = pairs->at(index.row());
        QStringList aList = aPair.second;
        RelaxEngine::formatString(aList, RelaxEngine::Remove);

        QString str = aList.join(",");

        if(index.column() == 0)
            return aPair.first;

        if(index.column() == 1)
            return str;

    }

    return QVariant();
}

QVariant RTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if(orientation == Qt::Horizontal){
        switch(section){
        case 0:
            return tr("Paths");

        case 1:
            return tr("Filters");
        }
    }

    return QVariant();

}

bool RTableModel::addFilterPair(const FilterPair &pair)
{
    if(!isExists(pair)){
        if(!(QDir(pair.first).exists())){
            return false;
        }
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        pairs->append(pair);
        endInsertRows();
        return true;
    }

    return false;

}

void RTableModel::removeFilterPair(const QModelIndex aIndex)
{
    if(!aIndex.isValid())
        return;

    if(aIndex.row() > pairs->size() || aIndex.row() < 0)
        return;

    beginRemoveRows(QModelIndex(), aIndex.row(), aIndex.row());
    pairs->removeAt(aIndex.row());
    endRemoveRows();
}

bool RTableModel::isExists(const FilterPair &pair)
{
    for(int i = 0; i < pairs->size(); i++){
         FilterPair aPair = pairs->at(i);
         for(int j = 0; j < pair.second.size(); j++){
             QString str = pair.second.at(j);

             if(aPair.second.contains(str))
                 return true;
         }
    }

    return false;
}
