#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModelIndex>

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
private:
    Ui::MainWindow *ui;
    RelaxEngine *engine;
    RTableModel *tableModel;
    RListModel *listModel;
    QDeclarativeView *view;
};

#endif // MAINWINDOW_H
