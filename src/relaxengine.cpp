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
#include <QtConcurrentMap>
#include <QString>

#ifdef Q_OS_LINUX
#include "linops.h"
#endif

#include "src/relaxengine.h"

void copyFiles(CopyPair &aPair); /*copies the file contained in aPair.first to aPair.second and then removes
                                         the file in aPair.first if copy was successful*/

/**
  * static data member definitions
  */
PathList RelaxEngine::paths = PathList();
bool RelaxEngine::isCopying = false;
bool RelaxEngine::pendingRefresh = false;
bool RelaxEngine::liveMode = true;
bool RelaxEngine::scanNested = false;
WatcherThread* RelaxEngine::watcher = new WatcherThread();
QFutureWatcher<void>* RelaxEngine::future = new QFutureWatcher<void>();

/**
  * static auxilliary function prototypes
  */

/**
  * @abstract get_nested_files() returns a list of files in a folder an its sub-directories
  *
  * @param path contains the path to the folder to scan for files
  * @param found holds the list of files found
  *
  * @return void
  */
static void get_nested_files(QString path, QStringList &found);

/**
  * @abstract get_paths() recursively returns the files contained in a folder
  * contained in paths
  *
  * @param paths holds the list of path to scan
  */
static void get_paths(QStringList paths);

/**
  * @abstract has_children() checks if the folder referred to by path has sub-directories
  *
  * @param path: folder to check for sub-directories;
  *
  * @return bool: returns true if paths has children else returns false
  */
static bool has_children(const QString& path);
/**
  * @abstract store_paths() stores the paths contained in aList in static memory
  *
  * @param aList: list of paths to store
  *
  * @returns the saved list if an empty list is given to it as argument
  * else it returns a default constructed value
  */
static QStringList store_paths(const QStringList &aList);

/**
  * @abstract get_absolute_file_paths() returns the absolute file paths of the files
  * contained in str
  *
  * @param str contains the path to be scanned
  *
  * @returns a list of all the files found
  */
static QStringList get_absolute_file_paths(QString str);

/**
  * @abstract get_absolute_dir_paths() returns the absolute paths of the directories
  * contained in str
  *
  * @returns a QStringList containing the paths of the directories found
  */
static QStringList get_absolute_dir_paths(QString str);

/**
  * @abstract reverseList() takes listToReverse and returns a reversed version of it
  *
  * @param listToReverse contains the list to be reversed
  */
static void reverseList(CopyList &listToReverse);

/**
  * constructor definition
  */
RelaxEngine::RelaxEngine(QObject *parent) :
    QObject(parent)
{
    connect(watcher, SIGNAL(directoryChanged(QString)), SLOT(refreshFolders(QString)),
            Qt::DirectConnection);

    //connect(this, SIGNAL(copyFinished()), SLOT(recallRefresh()));

}

void copyFiles(CopyPair &aPair)
{
    //this function copies file from the path in aPair.first to apair.second
    QString sourceFilePath = aPair.first;
    QString destFilePath = aPair.second;

    QFile handle(sourceFilePath);

    if(handle.copy(destFilePath)){//remove the source file if copy was successful
        handle.remove();
    }
}

void RelaxEngine::deleteStaticMembers()
{
    qDebug() << "deleting";
    delete watcher;
    delete future;
}

void RelaxEngine::undoTransfer()
{
    if(isCopying)
        return;
    isCopying = true;
    emit copyStarted();

    CopyList listToUndo;
    listToUndo = sessionTransfer.pop();
    reverseList(listToUndo);

#ifdef Q_OS_LINUX
    future->setFuture(QtConcurrent::map(listToUndo, lin_move));
#else
    future->setFuture(QtConcurrent::map(listToUndo, copyFiles));
#endif

    redoStack.push(listToUndo);
    isCopying = false;
    emit finalFinish();
    emit checkUndo();
}

void RelaxEngine::redoTransfer()
{
    if(isCopying)
        return;
    isCopying = true;
    CopyList listToRedo = redoStack.pop();
    reverseList(listToRedo);

#ifdef Q_OS_LINUX
    future->setFuture(QtConcurrent::map(listToRedo, lin_move));
#else
    future->setFuture(QtConcurrent::map(listToRedo, copyFiles));
#endif

    sessionTransfer.push(listToRedo);
    isCopying = false;
    emit finalFinish();
    emit checkUndo();
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


void RelaxEngine::prepareFileCopy()
{

    if(paths.baseFilePaths.isEmpty() || paths.listPairs.isEmpty() ){
        qDebug() << "no files found";//premature calling of this function
        return;
    }
    CopyList filesList;//list to hold pairs of file sources and destination

    if(isCopying)
        return;
    isCopying = true;

    emit copyStarted();

#ifdef Q_OS_LINUX
    if(scanNested)
        trav_dirs(filesList, paths.baseFilePaths, paths.listPairs);
    else
        prepareFiles(filesList, paths.baseFilePaths, paths.listPairs);

    future->setFuture(QtConcurrent::map(filesList, lin_move));
#else
    prepareFiles(filesList, paths.baseFilePaths, paths.listPairs);
    future->setFuture(QtConcurrent::map(filesList, copyFiles));
#endif
    future->waitForFinished();
    isCopying = false;

    sessionTransfer.push(filesList);

    emit finalFinish();
    emit checkUndo();
}

void RelaxEngine::prepareFiles(CopyList &filesList, const QStringList &pathsToScan,
                               const QList<FilterPair> &pair)
{
    QString a_path;
    QStringList founds;
    foreach(a_path, pathsToScan){
        if(scanNested){
            get_nested_files(a_path, founds);
        }else{
            shallowWalk(a_path, founds);
        }
    }
    process_files(founds, filesList, pair);
}

void RelaxEngine::shallowWalk(QString walkWay, QStringList &found)
{
    found.append(get_absolute_file_paths(walkWay));
}

void RelaxEngine::fixFolder(const QString pathToFolder)
{

    if(isCopying)
        return;
    isCopying = true;

    emit copyStarted();
    CopyList filesToCopy;
    QStringList pathToScan;

    pathToScan.append(pathToFolder);
#ifdef Q_OS_LINUX
    if(scanNested)
        trav_dirs(filesToCopy, pathToScan, paths.listPairs);
    else
        prepareFiles(filesToCopy, pathToScan, paths.listPairs);

    future->setFuture(QtConcurrent::map(filesToCopy, lin_move));
#else
    prepareFiles(filesToCopy, pathToScan, paths.listPairs);
    future->setFuture(QtConcurrent::map(filesToCopy, copyFiles));
#endif

    future->waitForFinished();
    isCopying = false;

    sessionTransfer.push(filesToCopy);

    emit finalFinish();
    emit checkUndo();
}

bool RelaxEngine::canUndo() const
{
    if(sessionTransfer.isEmpty())
        return false;
    else
        return true;

    return false;
}

bool RelaxEngine::canRedo() const
{
    if(redoStack.isEmpty())
        return false;
    else
        return true;

    return false;
}

void RelaxEngine::refreshFolders(QString folder)
{

    if(!isCopying)
        fixFolder(folder);
}

/*void RelaxEngine::recallRefresh()
{
    qDebug() << "recall refresh called";
    if(pendingRefresh){
        pendingRefresh = false;
        prepareFileCopy();
    }else{
        emit finalFinish();
    }
}*/

void RelaxEngine::cancelCopy(){
    future->cancel();
}

void get_nested_files(QString path, QStringList& found){

    QStringList a_list;
    a_list.append(path);

    get_paths(a_list);
    found.append(store_paths(QStringList()));
}

void get_paths(QStringList paths){
    QString str;

    foreach(str, paths){
        if(has_children(str)){
            get_paths(get_absolute_dir_paths(str));
            store_paths(get_absolute_file_paths(str));
        }
    }
}

QStringList get_absolute_file_paths(QString str)
{
    QDir a_dir(str);
    a_dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable
                    | QDir::Writable);
    QStringList found_files = a_dir.entryList();
    QStringList ret_val;
    QString s;
    foreach(s, found_files){
        ret_val.append(a_dir.absoluteFilePath(s));
    }

    return ret_val;
}

bool has_children(const QString &path)
{
    QDir a_dir(path);
    a_dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable |
                    QDir::Writable);
    QStringList a_list = a_dir.entryList();

    if(a_list.isEmpty()){
        return false;
    }else{
        return true;
    }

    return false;
}

QStringList store_paths(const QStringList &aList){

    static QStringList store;
    if(aList.isEmpty()){
        QStringList temp = store;
        store.empty();

        return temp;
    }else{
        store.append(aList);
        return QStringList();
    }

    return QStringList();
}

QStringList get_absolute_dir_paths(QString str){
    QDir a_dir(str);
    a_dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable |
                    QDir::Writable);
    QString s;
    QStringList ret_val;
    QStringList found_dirs = a_dir.entryList();
    foreach(s, found_dirs){
        QString path;
        path.append(str);
        path.append("/");
        path.append(s);
        ret_val.append(path);
    }

    return ret_val;
}
void process_files(QStringList dir_list, CopyList &file_list,
                  const QList<FilterPair> &fil_pairs){
    /**
      * @pre dir_list contains list of filepaths to be processed, file_list contains
      * already processed files and fil_pairs contains filters and their directories
      */

    QString str;

    foreach(str, dir_list){
        FilterPair a_pair;

        foreach(a_pair, fil_pairs){

            QStringList filters = a_pair.second;
            QString a_str;
            foreach(a_str, filters){
                remove_ast(a_str);

                if(str.endsWith(a_str)){
                    CopyPair p;
                    QString f_name;

                    get_file_name(str, f_name);

                    p.first = str;
                    p.second = a_pair.first;
                    p.second.append("/");
                    p.second.append(f_name);

                    file_list.append(p);
                }
            }
        }
    }
}

void reverseList(CopyList &listToReverse){

    /**
      * @pre listToReverse contains an unreversed list
      *
      * @post listToReverse now contains a reversed list
      */
    CopyPair aPair;

    for(int i = 0; i < listToReverse.size(); i++){
        QString temp;

        temp = listToReverse[i].first;
        listToReverse[i].first = listToReverse[i].second;

        listToReverse[i].second = temp;
    }
}
