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
#ifndef RELAXENGINE_H
#define RELAXENGINE_H

#include <QObject>
#include <QPair>
#include <QStringList>
#include <QFutureWatcher>
#include <QDir>
#include "watcherthread.h"

typedef QPair<QString, QStringList> FilterPair;
typedef QList<QPair<QString, QString> > CopyList;
typedef QPair<QString, QString> CopyPair;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct PathList{

    /*a simple data structure to hold the paths needed by the software*/
    QStringList baseFilePaths;//this should hold the paths to the folders the user wants monitored

    QList<FilterPair> listPairs;/*this should hold a list of QPairs (see typedef above) that consists of a
                                string anda stringlist the string should hold the path of the folder
                                 with the filters to indicate the type of files stored in them*/
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RelaxEngine : public QObject
{                                 /*this class by virtue should be instantiated only once throughout the
                                    application. it contains the static data member "PathList::paths"
                                    that holds the data to be used throughout the application. it contains
                                    the utility function formatString() that is used for string manipulatio
                                    ns outside the class.*/
    Q_OBJECT
    Q_ENUMS(Mode)
public:

    enum Mode{Remove, Add};

    explicit RelaxEngine(QObject *parent = 0);/*constructor*/
    //~RelaxEngine();
    void setBaseFilePaths(const QString &aPath);/*assigns aPath to paths.baseFilePaths*/

    void setListPairs(const FilterPair &somePair);/*assigns somePair to paths.listPairs*/


    QFutureWatcher<void>* getFutureWatcher();/*returns a pointer to the QFutureWatcher used for monitoring
                                               the progress of the file copy. it exits onlt once throughout
                                               the software*/

    void deleteStaticMembers();/*this deletes the static members that are pointers. it should be called only
                                 once throughout the application. preferrably during the application's
                                 destruction*/

    void clearPaths();/*this clears all the data present in the "paths" member variable. did not use it
                        in this version. as most of the data manipulation is done by the  model*/


    PathList getPaths();/*this returns the PathList data member. not used because the data manipulation is
                         done by the model*/

    void clearWatchPaths();
    void restoreWatchPaths();



    /*Public static member functions*/
    static WatcherThread* getWatcher();/*returns the data member WatcherThread watcher*/

    static QList<FilterPair> *getFilterPairs();/*returns paths.listpair. not used. prefer data manipulation
                                                 at the model*/

    static QStringList* getBaseFilePaths();/*returns paths.baseFilepaths. not used. data manipulation done
                                             at the model*/

    static void formatString(QStringList& list,
                             Mode aMode);/*a static non-class related function that is used to edit
                                           path filters. it modifies all the strings contained in "list"
                                           according to mode*/
    static bool getLiveMode();
    static bool setLiveMode(bool live);

private:

    void prepareFileCopy();/*this function should prepare the files given by the user for copying by filterin
                           each path with each given filter, appending the found files to a QList of Qpairs
                           then copying the files by applying copyFiles() on each item on the list  using
                           QtConcurrent::map()*/

signals:
    void copyFinished();/*this signal is emitted everytime a new file has been copied*/
    void copyStarted();/*indicates when a new copy session has started*/
    void finalFinish();/*indicates when the final there is absolutely no new file to copy*/
    
public slots:
    void refreshFolders(QString folder);/*this slot is called every time a folder is modified and also
                                          called whenever a user wants a folder refresh. it calls
                                          Relaxengine::prepareFilecopy(). if it is already running, it sets
                                          RelaxEngine::pendingRefresh to true*/

    void cancelCopy();
    void jusChecking();
    void checkFinish();
private slots:
    void recallRefresh();/*this slot is called at the end of every call to RelaxEngine::prepareFilecopy()
                           it is used to check if any path has been modified during a previous call to
                           RelaxEngine::prepareFileCopy()*/

    void makeSure(QString path);
    
private:
    static PathList paths;/*member to hold all list of paths and is also the data source for the whole app*/

    static QFutureWatcher<void> *future;/*the overall future watcher for progress reports. for more info
                                          on future watcher. see qt docs for more on QFutureWatcher*/

    static WatcherThread* watcher;
    static bool liveMode;
    static bool isCopying;
    static bool pendingRefresh;
};

#endif // RELAXENGINE_H
