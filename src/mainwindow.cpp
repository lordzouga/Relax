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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QtConcurrentRun>
#include <QUrl>
#include <QCloseEvent>
#include <QSettings>
#include <QDeclarativeView>
#include <QDeclarativeItem>
#include <QGraphicsObject>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include "relaxengine.h"
#include "rtablemodel.h"
#include "rlistmodel.h"

#ifdef Q_OS_LINUX
#define USER_PATH           ((QString("/home/"))+(QString(getenv("USER"))))
#define REFRESH_PATH        (QString("/.relax-refresh"))
#define ADD_FOLDER_PATH     (QString("/.relax-add-folder"))
#define REFRESH_FOLDER_PATH (QString("/.refresh-current-folder"))
#define FIX_FILE            (QString("/.relax-fix-file"))
#endif

const char *unixFonts = "\nQTabBar::tab, QLabel, QLineEdit {font-weight: bold; font-size: 11px; color: #ffffff}";
static void checkIfExists(QStringList& files){
    QString file;

    foreach(file, files){
        QFile handle(file);

        if(!handle.exists())
        {
            handle.open(QIODevice::ReadWrite);
            handle.close();
        }
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    musicFilters << "*.mp3" << "*.m4a" << "*.ogg";
    documentFilters << "*.xls" << "*.doc" << "*.txt" << "*.pdf";
    videoFilters << "*.avi" << "*.mkv" << "*.mp4";
    imageFilters << "*.jpg" << "*.png" << "*.gif";

    engine = new RelaxEngine(this);
    tableModel = new RTableModel(this);
    listModel = new RListModel(this);

    watcher = engine->getFutureWatcher();


    ui->tabWidget->setTabText(0, "Set Origin Folders");
    ui->tabWidget->setTabText(1, "Set Choice Folders");

    ui->destTableView->setModel(tableModel);
    ui->sourceListView->setModel(listModel);

    ui->destTableView->setColumnWidth(0, 204);
    ui->destTableView->setColumnWidth(1, 156);

    ui->progressBar->hide();


    QDeclarativeEngine *aEngine = ui->declarativeView->engine();
    aEngine->rootContext()->setContextProperty("engine", engine);

    QGraphicsObject *object = ui->declarativeView->rootObject();
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem *>(ui->declarativeView->rootObject());

    connect(item, SIGNAL(startRefresh()), this, SLOT(updateFolders()));
    connect(item, SIGNAL(stopRefresh()), engine, SLOT(cancelCopy()));

    ui->declarativeView->show();
    qDebug() << object->children().size();

    ui->destTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->destTableView->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->updateButton->hide();
    ui->cancelButton->hide();


    QFile styleFile(":/style/style.qss");
    styleFile.open(QFile::ReadOnly);

    QString styleString = tr(styleFile.readAll());

    QStringList filePaths;
#ifdef Q_OS_LINUX
    styleString.append(unixFonts);


    filePaths.append(USER_PATH + REFRESH_PATH);
    filePaths.append(USER_PATH + ADD_FOLDER_PATH);
    filePaths.append(USER_PATH + REFRESH_FOLDER_PATH);
    filePaths.append(USER_PATH + FIX_FILE);

    checkIfExists(filePaths);
#endif

    qApp->setStyleSheet(styleString);
    ipcThread = new WatcherThread();
    ipcThread->addPaths(filePaths);

    connectSignals();

    /*check if its the first time the application is being run*/
    QSettings settings;
    QStringList setList = settings.allKeys();

    if(setList.isEmpty()){
        reset();//if settings is empty, fill model with default values
    }else{
     loadSettings();//load already saved settings
    }


    setMaximumSize(size());
    setMinimumSize(size());

    //ui->progressBar->hide();
    populateView();


}

QString MainWindow::getPath()
{
    return QFileDialog::getExistingDirectory(this, "Select Folder", QDir::rootPath());
}

void MainWindow::populateView()
{
    view = new QDeclarativeView();
    //view->setSource(QUrl::fromLocalFile("main.qml"));
    view->setMinimumSize(450, 420);
    view->setMaximumSize(450, 420);
    view->hide();
}

void MainWindow::connectSignals()
{
    connect(ui->browseSourcePathButton, SIGNAL(clicked()), this, SLOT(getSourcePath()));
    connect(ui->addSourcePathButton, SIGNAL(clicked()), this, SLOT(addSourcePath()));

    connect(ui->browseDestPathButton, SIGNAL(clicked()), this, SLOT(getDestPath()));
    connect(ui->addDestPathButton, SIGNAL(clicked()), this, SLOT(addDestPath()));

    /*connect(ui->updateButton, SIGNAL(clicked()), this, SLOT(updateFolders()));
    connect(engine, SIGNAL(copyStarted()), ui->updateButton, SLOT(hide()));
    connect(engine, SIGNAL(copyStarted()), ui->cancelButton, SLOT(show()));
    connect(engine, SIGNAL(finalFinish()), ui->cancelButton, SLOT(hide()));
    connect(engine, SIGNAL(finalFinish()), ui->updateButton, SLOT(show()));
    connect(ui->cancelButton, SIGNAL(clicked()), engine, SLOT(cancelCopy()));
*/
    connect(watcher, SIGNAL(started()), ui->progressBar, SLOT(show()));
    connect(watcher, SIGNAL(progressRangeChanged(int,int)), ui->progressBar, SLOT(setRange(int,int)));
    connect(watcher, SIGNAL(progressValueChanged(int)), ui->progressBar, SLOT(setValue(int)));
    connect(watcher, SIGNAL(finished()), ui->progressBar, SLOT(hide()));

    connect(ui->saveAction, SIGNAL(triggered()), SLOT(saveSettings()));
    connect(ui->exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(ui->aboutRelax, SIGNAL(triggered()), this, SLOT(showAbout()));
    connect(ui->actionUndo, SIGNAL(triggered()), SLOT(undo()));
    connect(ui->actionRedo, SIGNAL(triggered()), SLOT(redo()));

    connect(ui->destTableView, SIGNAL(clicked(QModelIndex)), this, SLOT(actiateTableView(QModelIndex)));
    connect(ui->sourceListView, SIGNAL(clicked(QModelIndex)), this, SLOT(activateListView(QModelIndex)));

    connect(ui->deleteSourcePathButton, SIGNAL(clicked()), this, SLOT(deleteSourcePath()));
    connect(ui->editSourcePathButton, SIGNAL(clicked()), this, SLOT(editSourcePath()));
    connect(ui->destPathDeleteButton, SIGNAL(clicked()), this, SLOT(deleteDestPath()));
    connect(ui->destPathEditButton, SIGNAL(clicked()), this, SLOT(editFilterPair()));

    connect(ui->radioButton, SIGNAL(toggled(bool)), this, SLOT(toggleLiveMode(bool)));

    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveSettings()));
    connect(ipcThread, SIGNAL(fileChanged(QString)), SLOT(replyComm(QString)));
    connect(engine, SIGNAL(checkUndo()), SLOT(setUndoState()));
}


void MainWindow::loadSettings()
{
    QSettings settings;

    QString sourcePaths("sourcePaths");
    QString destPathsFilters("destPathsFilters");

    if(!settings.contains("appCount"))
        settings.setValue("appCount", 1);

        int size = settings.beginReadArray(sourcePaths);

        for(int i = 0; i < size; ++i){
            settings.setArrayIndex(i);
            QString sourcePath = settings.value("sourcePath").toString();
            qDebug() << "load settings" << sourcePath;
            listModel->addFilePath(sourcePath);
        }

        settings.endArray();


        int length = settings.beginReadArray(destPathsFilters);

        for(int i = 0; i < length; ++i){
            settings.setArrayIndex(i);
            FilterPair pair;
            pair.first = settings.value("destPath").toString();
            pair.second = settings.value("filter").toString().split(",");
            tableModel->addFilterPair(pair);
        }

        settings.endArray();
}

void MainWindow::saveSettings()
{
    QSettings settings;

    QString sourcePaths("sourcePaths");
    QString destPathsFilters("destPathsFilters");

    PathList lists(engine->getPaths());
    qDebug() << "checking settings";

    if(lists.baseFilePaths.isEmpty()&&lists.listPairs.isEmpty()){
        settings.clear();
        return;
    }

    settings.clear();

    if(!lists.baseFilePaths.isEmpty()){
        settings.beginWriteArray(sourcePaths);

        for(int i = 0; i < lists.baseFilePaths.size(); ++i){
            settings.setArrayIndex(i);
            settings.setValue("sourcePath", lists.baseFilePaths.at(i));
            qDebug() << lists.baseFilePaths.at(i);
            qDebug() << "variant" << settings.value("sourcePath").toString();
        }

        settings.endArray();
    }


    if(!lists.listPairs.isEmpty()){
        settings.beginWriteArray(destPathsFilters);

        for(int i = 0; i < lists.listPairs.size(); ++i){
            settings.setArrayIndex(i);
            settings.setValue("destPath", lists.listPairs.at(i).first);
            QString filter = lists.listPairs.at(i).second.join(",");
            settings.setValue("filter", filter);
        }

        settings.endArray();
    }

    settings.sync();

}


void MainWindow::reset()
{
   engine->clearPaths();

    QString username;

    QString musicPath;
    QString docPath;
    QString imagePath;
    QString videoPath;

#ifdef Q_OS_WIN
    username = qgetenv("USERNAME");

    musicPath = "C:\\Users\\" + username + "\\Music";
    docPath = "C:\\Users\\" + username + "\\Documents";
    imagePath = "C:\\Users\\" + username + "\\Pictures";
    videoPath = "C:\\Users\\" + username + "\\videos";
#endif //Q_OS_WIN

#ifdef Q_OS_UNIX
    username = qgetenv("USER");
    musicPath = "/home/" + username + "/Music";
    docPath = "/home/" + username + "/Documents";
    imagePath = "/home/" + username + "/Pictures";
    videoPath = "/home/" + username + "/Videos";
#endif //Q_OS_UNIX

    tableModel->addFilterPair(FilterPair(musicPath, musicFilters));
    tableModel->addFilterPair(FilterPair(docPath, documentFilters));
    tableModel->addFilterPair(FilterPair(imagePath, imageFilters));
    tableModel->addFilterPair(FilterPair(videoPath, videoFilters));
}

MainWindow::~MainWindow()
{
    delete ui;
    engine->deleteStaticMembers();
    engine->deleteLater();
    tableModel->deleteLater();
    listModel->deleteLater();
    delete view;
}


void MainWindow::getSourcePath()
{
    QString source = getPath();
    if(!source.isEmpty() || !source.isNull()){
        ui->sourcePathEdit->setText(source);
    }
}

void MainWindow::addSourcePath()
{
    QString str = ui->sourcePathEdit->text();
    if(str.isEmpty()|| !QDir(str).exists())
        return;

    listModel->addFilePath(str);
    ui->sourcePathEdit->clear();
}

void MainWindow::getDestPath()
{
    QString dest = getPath();
    if(!dest.isEmpty() || !dest.isNull())
        ui->destPathEdit->setText(dest);
}

void MainWindow::addDestPath()
{
    QString dest = ui->destPathEdit->text();
    QString filters  = ui->nameFilterEdit->text();

    qDebug() << "destination" << dest;
    qDebug() << "filters" << filters;

    if(!filters.isEmpty() && !dest.isEmpty()){
         QString str = filters.remove(" ");
        FilterPair pair;
        QStringList filterList = str.split(",", QString::SkipEmptyParts);
        RelaxEngine::formatString(filterList, RelaxEngine::Add);

        pair.first = dest;
        pair.second = filterList;

        qDebug() << "adding pairs" << pair;
        if(tableModel->addFilterPair(pair)){
            ui->destPathEdit->clear();
            ui->nameFilterEdit->clear();
        }

    }
}

void MainWindow::updateFolders()
{
    QtConcurrent::run(engine, &RelaxEngine::startTransfer);
}

void MainWindow::activateListView(QModelIndex aIndex)
{
    Q_UNUSED(aIndex);

    ui->deleteSourcePathButton->setEnabled(true);
    ui->editSourcePathButton->setEnabled(true);
}

void MainWindow::actiateTableView(QModelIndex aIndex)
{
    Q_UNUSED(aIndex)

    ui->destPathDeleteButton->setEnabled(true);
    ui->destPathEditButton->setEnabled(true);
}

void MainWindow::deleteSourcePath()
{
    QModelIndex aIndex = ui->sourceListView->currentIndex();

    listModel->removeFilePath(aIndex);
}

void MainWindow::setUndoState()
{
    if(engine->canUndo()){
        ui->actionUndo->setEnabled(true);
        qDebug() << "can undo";
    }
    else
        ui->actionUndo->setEnabled(false);

    qDebug() << "it has happened";

    if(engine->canRedo()){
        ui->actionRedo->setEnabled(true);
    }
    else
        ui->actionRedo->setEnabled(false);
}

void MainWindow::undo()
{
    QtConcurrent::run(engine, &RelaxEngine::undoTransfer);
}

void MainWindow::redo()
{
    QtConcurrent::run(engine, &RelaxEngine::redoTransfer);
}

void MainWindow::deleteDestPath()
{
    QModelIndex aIndex = ui->destTableView->currentIndex();
    tableModel->removeFilterPair(aIndex);
}

void MainWindow::editFilterPair()
{
    QItemSelectionModel* selModel = ui->destTableView->selectionModel();

    QModelIndexList indexList = selModel->selectedIndexes();
    QModelIndex index;

    foreach (index, indexList) {
        if(index.column() == 0)
            ui->destPathEdit->setText(index.data().toString());

        if(index.column() == 1)
            ui->nameFilterEdit->setText(index.data().toString());
    }
    deleteDestPath();
}

void MainWindow::editSourcePath()
{
    QModelIndex aIndex = ui->sourceListView->currentIndex();

    ui->sourcePathEdit->setText(aIndex.data().toString());

    deleteSourcePath();
}

void MainWindow::showAbout()
{
    view->setSource(QUrl("qrc:/main.qml"));
    view->show();
}

void MainWindow::toggleLiveMode(bool checked)
{
    if(checked){
        RelaxEngine::setLiveMode(true);
        engine->restoreWatchPaths();
    }
    else{
        RelaxEngine::setLiveMode(false);
        engine->clearWatchPaths();
    }
}

void MainWindow::replyComm(QString file)
{
    QFile handle(file);

    if(handle.open(QIODevice::ReadOnly)){
        QString path(handle.readAll());
        if(!path.isEmpty()){

            if(file == USER_PATH + ADD_FOLDER_PATH)
                listModel->addFilePath(path);
            if(file == USER_PATH + REFRESH_PATH)
                QtConcurrent::run(engine, &RelaxEngine::startTransfer);
            if(file == USER_PATH + REFRESH_FOLDER_PATH)
                QtConcurrent::run(engine, &RelaxEngine::fixCurrentDir, path);
        }
    }


}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}
