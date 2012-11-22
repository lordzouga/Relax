#include "src/watcherthread.h"

WatcherThread::WatcherThread(QObject *parent) :
    QFileSystemWatcher(parent)
{
    aThread = new RThread();
    this->moveToThread(aThread);

    connect(aThread, SIGNAL(terminated()), SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed()), aThread, SLOT(quit()));

    aThread->start();
}

WatcherThread::~WatcherThread()
{
    if(aThread->isRunning())
        aThread->exit();

    aThread->deleteLater();
}
