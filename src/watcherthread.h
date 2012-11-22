#ifndef WATCHERTHREAD_H
#define WATCHERTHREAD_H

#include <QFileSystemWatcher>
#include <QThread>

class RThread : public QThread
{
    Q_OBJECT
public:
    RThread(QObject *parent = 0): QThread(parent){
    }
protected:
    void run(){ exec(); }
};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

class WatcherThread : public QFileSystemWatcher
{
    Q_OBJECT
public:
    explicit WatcherThread(QObject *parent = 0);
    ~WatcherThread();
signals:
    
public slots:
private:
    RThread *aThread;
    
};

#endif // WATCHERTHREAD_H
