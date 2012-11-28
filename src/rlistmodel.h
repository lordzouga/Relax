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
#ifndef RLISTMODEL_H
#define RLISTMODEL_H

#include <QAbstractListModel>
#include "relaxengine.h"

/*a Model class that manages the source paths provided by the user and provides them to the views
 * */
class RListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit RListModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    bool addFilePath(const QString &path);
    void removeFilePath(const QModelIndex &index);

    
signals:
    
public slots:
    
private:
    //RelaxEngine engine;
    QStringList* paths;
};

#endif // RLISTMODEL_H
