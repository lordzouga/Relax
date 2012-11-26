#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QtConcurrentRun>
#include <QUrl>
#include <QSettings>
#include <QDeclarativeView>
#include <QDeclarativeItem>
#include <QGraphicsObject>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include "relaxengine.h"
#include "rtablemodel.h"
#include "rlistmodel.h"

const char *unixFonts = "\nQTabBar::tab, QLabel, QLineEdit {font-weight: bold; font-size: 11px; color: #ffffff}";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    musicFilters << "*.mp3" << "*.m4a" << "*.ogg";
    documentFilters << "*.xls" << "*.doc" << "*.txt" << "*.pdf" << "*.epub";
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

    ui->declarativeView->show();
    QDeclarativeEngine *aEngine = ui->declarativeView->engine();
    aEngine->rootContext()->setContextProperty("engine", engine);

    QGraphicsObject *object = ui->declarativeView->rootObject();
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem *>(ui->declarativeView->rootObject());

    connect(item, SIGNAL(startRefresh()), this, SLOT(updateFolders()));
    connect(item, SIGNAL(stopRefresh()), engine, SLOT(cancelCopy()));

    qDebug() << object->children().size();

    ui->destTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->destTableView->setSelectionMode(QAbstractItemView::SingleSelection);

    ui->updateButton->hide();

    QFile styleFile(":/style/style.qss");
    styleFile.open(QFile::ReadOnly);

    QString styleString = tr(styleFile.readAll());

#ifndef Q_WS_WIN
    styleString.append(unixFonts);
#endif

    qApp->setStyleSheet(styleString);

    connectSignals();
    qDebug() << qgetenv("USER");
    loadSettings();

    setMaximumSize(size());
    setMinimumSize(size());

    //ui->progressBar->hide();
    populateView();
    reset();


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

    connect(ui->updateButton, SIGNAL(clicked()), this, SLOT(updateFolders()));

    connect(watcher, SIGNAL(started()), ui->progressBar, SLOT(show()));
    connect(watcher, SIGNAL(progressRangeChanged(int,int)), ui->progressBar, SLOT(setRange(int,int)));
    connect(watcher, SIGNAL(progressValueChanged(int)), ui->progressBar, SLOT(setValue(int)));
    connect(watcher, SIGNAL(finished()), ui->progressBar, SLOT(hide()));

    connect(ui->saveAction, SIGNAL(triggered()), SLOT(saveSettings()));
    connect(ui->exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(ui->aboutRelax, SIGNAL(triggered()), this, SLOT(showAbout()));

    connect(ui->destTableView, SIGNAL(clicked(QModelIndex)), this, SLOT(actiateTableView(QModelIndex)));
    connect(ui->sourceListView, SIGNAL(clicked(QModelIndex)), this, SLOT(activateListView(QModelIndex)));

    connect(ui->deleteSourcePathButton, SIGNAL(clicked()), this, SLOT(deleteSourcePath()));
    connect(ui->editSourcePathButton, SIGNAL(clicked()), this, SLOT(editSourcePath()));
    connect(ui->destPathDeleteButton, SIGNAL(clicked()), this, SLOT(deleteDestPath()));
    connect(ui->destPathEditButton, SIGNAL(clicked()), this, SLOT(editFilterPair()));

    connect(ui->radioButton, SIGNAL(toggled(bool)), this, SLOT(toggleLiveMode(bool)));

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
#endif //Q_WS_WIN

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
    QString str;
    QtConcurrent::run(engine, &RelaxEngine::refreshFolders, str);
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
        engine->restoreWatchPaths();
    }
    else{
        engine->clearWatchPaths();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}
