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
