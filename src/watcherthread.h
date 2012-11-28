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
