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
#include <QFutureWatcher>
#include <QDir>
#include <QStack>
#include "global_defs.h"
#include "watcherthread.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct PathList{

    /*a simple data structure to hold the paths needed by the software*/
    QStringList baseFilePaths;//this should hold the paths to the folders the user wants monitored

    QList<FilterPair> listPairs;/*this should hold a list of QPairs (see typedef above) that consists of a
                                string anda stringlist the string should hold the path of the folder
                                 with the filters to indicate the type of files stored in them*/
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void copyFiles(CopyPair &aPair);/*copies the file contained in aPair.first to aPair.second and then removes
                                         the file in aPair.first if copy was successful*/
/**
  * @abstract process_dirs() checks if the paths contained in dir_list is to be copied. it then copies
  * the path into file_list
  *
  * @param dir_list contains the list of file transfer candidates
  * @param file_list contains the list files to be transferred
  * @param fil_pairs contains filters and their corresponding paths
  */
void process_files(QStringList dir_list, CopyList& file_list,
                  const QList<FilterPair> &fil_pairs);

/**
  * @note this class by virtue should be instantiated only once throughout the
  * the application. it contains the static data member PathList::paths that holds
  * the data to be used throughout the application. it contains a utility function
  * formatstring() that is used for string manipulations
  */
class RelaxEngine : public QObject
{
    Q_OBJECT
    /** Qt Enum declaration*/
    Q_ENUMS(Mode)
public:

    enum Mode{Remove, Add};

    /** constructor*/
    explicit RelaxEngine(QObject *parent = 0);
    //~RelaxEngine();
    /** public functions*/

    /**
      * assigns aPath to paths.baseFilePaths*/
    void setBaseFilePaths(const QString &aPath){ paths.baseFilePaths << aPath; }

    /**
      * assigns somePair to paths.listPairs
      */
    void setListPairs(const FilterPair &somePair){ paths.listPairs.append(somePair); }

    /** @note getFutureWatcher() returns a pointer to the QFutureWatcher used for monitoring
      * the progress of the file copy. it exits onlt once throughout
      * the software
      */
    QFutureWatcher<void>* getFutureWatcher(){return future;}

   /** deleteStaticMembers() deletes the static members that are pointers. it should be called only
     * once throughout the application. preferrably during the application's
     * destruction
     */
    void deleteStaticMembers();

    /**
      * clearPaths() clears all the data present in the "paths" member variable. did not use it
      * in this version. as most of the data manipulation is done by the  model
      * */
    void clearPaths(){paths.baseFilePaths.clear(); paths.listPairs.clear();}

    /**
      * @note getPaths() returns the PathList data member. not used because the data manipulation is
      * done by the model
      */
    PathList getPaths(){return paths;}

    /**
      * @abstract undoTransfer() is used to undo a previous file transfer session
      * by popping a copyList object from session transfer and using it as the file
      * transfer object
      */
    void undoTransfer();

    /**
      * @abstract redoTransfer() is used to redo a previous transfer session
      */
    void redoTransfer();
    /**
      * @note clearWatchPaths() clears watcher of paths to watch
      */
    void clearWatchPaths(){watcher->removePaths(paths.baseFilePaths);}

    /**
      * @abstract canUndo() returns true if a session can be undone.. required by the GUI to activate
      * and disable the undo button
      */
    bool canUndo() const;

    /**
      * @abstract canRedo() returns true if a session can be redone. required by the GUI to activate
      * or disable the redo button
      */
    bool canRedo() const;
    /**
      * @note restoreWatchPaths() restore watcher paths
      */
    void restoreWatchPaths(){watcher->addPaths(paths.baseFilePaths);}

    /** Public static member functions*/
public:

    /**
      * @note getWatcher() returns the data member watcher
      */
    static WatcherThread* getWatcher(){return watcher;}

    /**
      * @abstract getFilterPairs() returns paths.listpair .
      *
      * @abstract not used. prefer data manipulation at the model
      */
    static QList<FilterPair> *getFilterPairs(){ return &paths.listPairs; }

    /**
      * @abstract getBaseFIlePaths returns paths.baseFilePaths.
      *
      * @note not used, data manipulation done at the model
      */
    static QStringList* getBaseFilePaths(){ return &paths.baseFilePaths; }

    /**
      * @abstract formatString() a static non related function that
      * is used to edit path filters.
      *
      * @param list: a list of strings to be modified
      * @param aMode: the mode of the manipulation to be done
      */
    static void formatString(QStringList& list,
                             Mode aMode);

    /**
      * @abstract getliveMode() returns the state of liveMode data members
      */
    static bool getLiveMode(){ return liveMode;}

    /**
      * @abstract setLiveMode() sets the value of liveMode data member
      *
      * @param live: value to be set
      */
    static void setLiveMode(bool live){ liveMode = live;}
/**
  * private members
  */
private:

    /** @abstract prepareFileCopy() should prepare the files given by the user for copying by filterin
      * each path with each given filter, appending the found files to a QList of Qpairs
      * then copying the files by applying copyFiles() on each item on the list  using
      * QtConcurrent::map()
      * */
    void prepareFileCopy();

    /**
      * @abstract prepareFiles() scans the directories contained in pathsToScan and puts
      * the files found in filesList
      *
      * @param fileList: files to be copied
      * @param pathsToScan: list of paths to be checked for files
      * @param pair: paths and their corresponding filters
      *
      * @return void
      */

    void prepareFiles(CopyList &filesList, const QStringList& pathsToScan,
                      const QList<FilterPair> &pair);

    /**
      * @abstract shallowWalk() gets the files contained in path denoted by walkWay
      * and returns it in found
      *
      * @param walkWay: path to search
      * @param found contains a list of files found
      *
      * @return void
      */
    void shallowWalk(QString walkWay, QStringList& found);

    /**
      * @abstract saveToStack() saves the List copied in the last file transfer to
      * member variable sessionTransfer
      *
      * @param aList: list to be saved
      */
    void saveToStack(const CopyList& aList){sessionTransfer.push(aList);}

    /**
      * @abstract fixFolder() scans pathToFolder and moves all the files contained in it
      * to their appropriate folders according to paths.listPairs
      *
      * @param pathToFolder: path to scan for files
      */
    void fixFolder(const QString pathToFolder);

    /** signals **/
signals:
    /**
      * copyFinished() signal is emitted everytime a new set of files have been copied*/
    void copyFinished();

    /** copyStarted() indicates when a new copy session has started*/
    void copyStarted();

    /** indicates when the final there is absolutely no new file to copy*/
    void finalFinish();
    
    /** public slots **/
public slots:

    /** this slot is called every time a folder is modified and also
      * called whenever a user wants a folder refresh. it calls
      * Relaxengine::prepareFilecopy(). if it is already running, it sets
      * RelaxEngine::pendingRefresh to true*/
    void refreshFolders(QString folder);

    /**
      * @abstract cancelCopy() cancels an ongoing copy session
      */
    void cancelCopy();

    /** private slots **/
private slots:

    /**
      * @abstract recallRefresh() slot is called at the end of every call to RelaxEngine::prepareFilecopy()
      * it is used to check if any path has been modified during a previous call to
      * RelaxEngine::prepareFileCopy()
      * */
    //void recallRefresh();
    
    /** private data members **/
private:
    /** member to hold all list of paths and is also the data source for the whole app **/
    static PathList paths;

    /** future: the overall future watcher for progress reports. for more info
     * on future watcher. see qt docs for more on QFutureWatcher
     **/
    static QFutureWatcher<void> *future;
    static WatcherThread* watcher;
    static bool liveMode;
    static bool isCopying;
    static bool pendingRefresh;
    static bool scanNested;

    QStack<CopyList> sessionTransfer;
    QStack<CopyList> redoStack;
};

#endif // RELAXENGINE_H
