#include <QDir>
#include <QtConcurrentMap>
#include <QString>
#include "relaxengine.h"

//static void copyFiles(CopyPair &aPair);

PathList RelaxEngine::paths = PathList();
bool RelaxEngine::isCopying = false;
bool RelaxEngine::pendingRefresh = false;
WatcherThread* RelaxEngine::watcher = new WatcherThread();
QFutureWatcher<void>* RelaxEngine::future = new QFutureWatcher<void>();

RelaxEngine::RelaxEngine(QObject *parent) :
    QObject(parent)
{
    if(connect(watcher, SIGNAL(directoryChanged(QString)), SLOT(refreshFolders(QString)),
            Qt::DirectConnection))
        qDebug() << "dammit";
    connect(watcher, SIGNAL(directoryChanged(QString)), SLOT(makeSure(QString)));
    connect(this, SIGNAL(copyFinished()), SLOT(recallRefresh()));

}

static void copyFiles(CopyPair &aPair)
{
    //this function copies file from the path in aPair.first to apair.second
    //qDebug() << "yay I am with: " << aPair;
    QString sourceFilePath = aPair.first;
    QString destFilePath = aPair.second;

    QFile handle(sourceFilePath);

    if(handle.copy(destFilePath)){//remove the source file if copy was successful
        //qDebug() << "copy successful";
        handle.remove();
    }
}

QFutureWatcher<void> *RelaxEngine::getFutureWatcher()
{
    return future;
}

void RelaxEngine::deleteStaticMembers()
{
    qDebug() << "deleting";
    delete watcher;
    delete future;
}

WatcherThread *RelaxEngine::getWatcher()
{
    return watcher;
}


void RelaxEngine::clearPaths()
{
    paths.baseFilePaths.clear();
    paths.listPairs.clear();
}

PathList RelaxEngine::getPaths()
{
    return paths;
}

QList<FilterPair>* RelaxEngine::getFilterPairs()
{
    return &paths.listPairs;
}

QStringList *RelaxEngine::getBaseFilePaths()
{
    return &paths.baseFilePaths;
}

void RelaxEngine::formatString(QStringList &list, RelaxEngine::Mode aMode)
{
    //qDebug() << "format String called";
    QString str;
        switch(aMode) {
        case RelaxEngine::Add:
            for(int j = 0; j < list.size(); j++){
                str = list.at(j);
                str.prepend("*.");
                list[j] = str;
            }
            break;

        case RelaxEngine::Remove:
            for(int i = 0; i < list.size(); i++){
                str = list.at(i);
                str.remove("*.");
                list[i] = str;
                //qDebug() << "Removing";
            }
            break;
        }
}

void RelaxEngine::emitAdd(QString path)
{
    qDebug() << "adding to paths " << path;
    watcher->addPath(path);
    qDebug() << watcher->directories();
}
void RelaxEngine::setBaseFilePaths(const QString &aPath)
{
    //set the path given by the user as the source file paths
    paths.baseFilePaths << aPath;
}

void RelaxEngine::setListPairs(const FilterPair &somePair)
{
    //add somePair ro the list FilterPair
    paths.listPairs.append(somePair);
    //FilterPair is a typedef for QPair<QString, QStringList>
}

void RelaxEngine::prepareFileCopy()
{
    /*this function iterates through the file paths gets the list of files in each list
     and copies them to their folders by changing the setNamefilters() of each instance.
    the copy function used is the QtConcurrent::map()*/

    if(paths.baseFilePaths.isEmpty() || paths.listPairs.isEmpty() ){
        qDebug() << "no files found";//premature calling of this function
        return;
    }
    qDebug() << paths.baseFilePaths;
    qDebug() << paths.listPairs;
    CopyList filesList;//list to hold pairs of file sources and destination

    if(isCopying)
        return;

    isCopying = true;

    for(int i = 0; i < paths.baseFilePaths.size(); i++){
        //loop to iterate through the source file paths

        for(int j = 0; j < paths.listPairs.size(); j++){
            //loop to iterate through the filters
            QDir baseDir(paths.baseFilePaths[i]);//create a directory handle

            baseDir.setFilter(QDir::Files);//set filters
            baseDir.setNameFilters(paths.listPairs[j].second);//set name filters

            qDebug() << "paths and filters used" << paths.listPairs[j];

            QStringList filesFound = baseDir.entryList();//get files
            QString file;

            if(!filesFound.isEmpty()){//if list is empty then no file was found corresponding to the filters

                foreach(file, filesFound){//for each file found append it to list
                    CopyPair filesToCopy;
                    QDir dest(paths.listPairs[j].first);

                    filesToCopy.first = baseDir.absoluteFilePath(file);
                    filesToCopy.second = dest.absoluteFilePath(file);

                    if(!(filesToCopy.first == filesToCopy.second))
                        filesList.append(filesToCopy);
                }
            }
        }
    }
    //create a future by calling the QtConcurrent::map() with the copyFiles function
    //so that each file in the in filesList gets mapped to the copyFiles function
    future->setFuture(QtConcurrent::map(filesList, copyFiles));
    future->waitForFinished();
    isCopying = false;

    if(!future->isRunning()){
      emit copyFinished();
    }
}

void RelaxEngine::refreshFolders(QString folder)
{
    Q_UNUSED(folder);

    if(!isCopying)
        prepareFileCopy();
    else
        pendingRefresh = true;
}

void RelaxEngine::recallRefresh()
{
    qDebug() << "recall refresh called";
    if(pendingRefresh){
        pendingRefresh = false;
        prepareFileCopy();
    }
}

void RelaxEngine::makeSure(QString path)
{
    qDebug() << "checking" << path;
}
