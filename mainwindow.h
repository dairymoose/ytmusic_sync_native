#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "messagethread.h"
#include "viewlocationsdialog.h"
#include <set>
#include <unordered_set>
#include "saveddata.h"
#include <QSystemTrayIcon>
#include "inputthread.h"
#include <QPlainTextEdit>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    void on_actionE_xit_triggered();
    void on_action_Add_Location_triggered();
    void on_action_View_Locations_triggered();
    void onTrayActivate( QSystemTrayIcon::ActivationReason reason );
    void onTrayExit();
    void onTrayShowHide();
    
protected:
    void changeEvent(QEvent *event);
    void closeEvent (QCloseEvent *event);
    
private:
    Ui::MainWindow *ui;
    MessageThread *mt = nullptr;
    InputThread *inputThread = nullptr;
    ViewLocationsDialog *viewLocationsDialog = nullptr;
    SavedData *sd = nullptr;
    void create_tray_icon();
    QSystemTrayIcon *m_tray_icon = nullptr;
    
    void appendLabel(std::string& toAppend);
};

#endif // MAINWINDOW_H
