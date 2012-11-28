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
#ifndef RTABLEMODEL_H
#define RTABLEMODEL_H

#include <QAbstractTableModel>
#include "relaxengine.h"

class RTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit RTableModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool addFilterPair(const FilterPair &pair);
    void removeFilterPair(const QModelIndex aIndex);
    bool isExists(const FilterPair &pair);


    
signals:
    
public slots:

private:
    RelaxEngine engine;
    QList<FilterPair>* pairs;
    
};

#endif // RTABLEMODEL_H
