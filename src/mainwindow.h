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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QFutureWatcher>

namespace Ui {
class MainWindow;
}

class RelaxEngine;
class RTableModel;
class RListModel;
class QDeclarativeView;
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    QString getPath();
    void populateView();
    void connectSignals();

private slots:
    void loadSettings();
    void saveSettings();

public:
    ~MainWindow();
    
public slots:
    void reset();
    void getSourcePath();
    void addSourcePath();
    void getDestPath();
    void addDestPath();
    void updateFolders();
    void activateListView(QModelIndex aIndex);
    void actiateTableView(QModelIndex aIndex);
    void deleteSourcePath();
    void deleteDestPath();
    void editFilterPair();
    void editSourcePath();
    void showAbout();
    void toggleLiveMode(bool checked);
private:
    Ui::MainWindow *ui;
    RelaxEngine *engine;
    RTableModel *tableModel;
    RListModel *listModel;
    QDeclarativeView *view;
    QFutureWatcher<void> *watcher;
    QStringList musicFilters;
    QStringList documentFilters;
    QStringList videoFilters;
    QStringList imageFilters;
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
