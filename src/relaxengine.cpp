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
/**
  * @note include for link() and unlink() sytem calls
  */
#include <unistd.h>
#endif

#include "src/relaxengine.h"

void copyFiles(CopyPair &aPair); /*copies the file contained in aPair.first to aPair.second and then removes
                                         the file in aPair.first if copy was successful*/

#ifdef Q_OS_LINUX
/**
  * @def prototype for the linux version of the copy function
  * it uses system calls for file transfer
  *
  * @param pair holds the absolute paths to the file be transferred.
  * source = pair.first
  * dest = pair.second
  *
  * @note if the file exists on a different drive it uses Qt genric file copy
  */
void lin_move(CopyPair &pair);

/**
  * @def prototype for the move_file() that makes the link() and unlink()
  * system call on linux
  *
  * @param source: holds the path of the file to be moved
  * @param dest: holds a valid path where the file is to be copied
  *
  * @note dest should not be referring to a valid file
  */
void move_file(const char* source, const char* dest);
#endif //Q_OS_LINUX

/**
  * static data defnitions
  */
PathList RelaxEngine::paths = PathList();
bool RelaxEngine::isCopying = false;
bool RelaxEngine::pendingRefresh = false;
bool RelaxEngine::liveMode = true;

WatcherThread* RelaxEngine::watcher = new WatcherThread();
QFutureWatcher<void>* RelaxEngine::future = new QFutureWatcher<void>();

RelaxEngine::RelaxEngine(QObject *parent) :
    QObject(parent)
{
    connect(watcher, SIGNAL(directoryChanged(QString)), SLOT(refreshFolders(QString)),
            Qt::DirectConnection);

    connect(watcher, SIGNAL(directoryChanged(QString)), SLOT(makeSure(QString)));
    connect(this, SIGNAL(copyFinished()), SLOT(recallRefresh()));

}

void copyFiles(CopyPair &aPair)
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

#ifdef Q_OS_LINUX
/** definition of lin_move()*/
void lin_move(CopyPair &pair){
    /**
      * @pre pair contains strings to valid file paths
      * source = pair.first, dest = pair.second
      *
      * @post the file path contained in pair.second now refers to a
      * valid file
      */

    QString old_path = pair.first;
    QString new_path = pair.second;

    std::string s = old_path.toStdString();
    std::string o = new_path.toStdString();

    move_file(s.c_str(), o.c_str());
}

void move_file(const char *source, const char *dest){
    /**
      * @pre source points to a valid file name. dest doesn't exist
      *
      * @post source does not exist. dest points to a valid file name
      */
    link(source, dest);
    unlink(source);
}

#endif//Q_OS_LINUX

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

bool RelaxEngine::getLiveMode()
{
    return liveMode;
}

bool RelaxEngine::setLiveMode(bool live)
{
    liveMode = live;
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

    emit copyStarted();

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

      emit copyFinished();
}

void RelaxEngine::clearWatchPaths()
{
    watcher->removePaths(paths.baseFilePaths);
}

void RelaxEngine::restoreWatchPaths()
{
    watcher->addPaths(paths.baseFilePaths);
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
    }else{
        emit finalFinish();
    }
}

void RelaxEngine::makeSure(QString path)
{
    qDebug() << "checking" << path;
}

void RelaxEngine::cancelCopy(){
    future->cancel();
}

void RelaxEngine::jusChecking()
{
    qDebug() << "I emmited stuff";
}

void RelaxEngine::checkFinish()
{
    qDebug() << "final finish called";
}
