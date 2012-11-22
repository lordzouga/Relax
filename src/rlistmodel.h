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
