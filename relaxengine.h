#ifndef RELAXENGINE_H
#define RELAXENGINE_H

#include <QObject>
#include <QPair>
#include <QStringList>
#include <QFutureWatcher>
#include <QFileSystemWatcher>
#include "watcherthread.h"

typedef QPair<QString, QStringList> FilterPair;
typedef QList<QPair<QString, QString> > CopyList;
typedef QPair<QString, QString> CopyPair;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct PathList{

    /*a simple data structure to hold the paths needed by the software*/
    QStringList baseFilePaths;//this should hold the paths to the folders the user wants monitored

    QList<FilterPair> listPairs;//this should hold a list of QPairs (see typedef above) that consists of a string and a stringlist
                                //the string should hold the path of the folder with the filters to indicate
                                //the type of files stored in them
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RelaxEngine : public QObject
{
    Q_OBJECT
    Q_ENUMS(Mode)
public:

    enum Mode{Remove, Add};

    explicit RelaxEngine(QObject *parent = 0);
    //~RelaxEngine();
    void setBaseFilePaths(const QString &aPath);//assigns aPath to paths.baseFilePaths

    void setListPairs(const FilterPair &somePair);//assigns somePair to paths.listPairs

    void prepareFileCopy();//this function should prepare the files given by the user for copying by filtering
                           //each path with each given filter, appending the found files to a QList of Qpairs
                           //then copying the files by applying copyFiles() on each item on the list  using
                           //QtConcurrent::map()

    //void copyFiles(CopyPair &aPair);//copies the file contained in aPair.first to aPair.second and then removes
                                    //the file in aPair.first if copy was successful
    QFutureWatcher<void>* getFutureWatcher();
    void deleteStaticMembers();
    static WatcherThread* getWatcher();
    void clearPaths();
    PathList getPaths();
    static QList<FilterPair> *getFilterPairs();
    static QStringList* getBaseFilePaths();
    static void formatString(QStringList& list, Mode aMode);
    static WatcherThread* watcher;
    RThread *aThread;

private:
    void emitAdd(QString path);

signals:
    void addToWatcher(QString path);
    void copyFinished();
    
private slots:
    void refreshFolders(QString folder);
    void recallRefresh();
    void makeSure(QString path);
    
private:
    static PathList paths;
    static QFutureWatcher<void> *future;
    static bool isCopying;
    static bool pendingRefresh;
};

#endif // RELAXENGINE_H
