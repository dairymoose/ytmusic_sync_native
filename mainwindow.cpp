#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <QMessageBox>
#include <QFileDialog>
#include <QDirIterator>
#include <sstream>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QTime>

void MainWindow::appendLabel(std::string& toAppend) {
    QString qs = this->ui->outputLabel->text();
    qs.append("[");
    qs.append(QTime().currentTime().toString());
    qs.append("] ");
    qs.append(toAppend.c_str());
    qs.append("\r\n");
    this->ui->outputLabel->setText(qs);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    if (sd == nullptr) {
        sd = new SavedData();
    }

    sd->loadFromDisk();
    
    mt = new MessageThread();
    mt->label = this->ui->outputLabel;
    mt->setSavedData(sd);
    mt->start();
    
    inputThread = new InputThread();
    inputThread->setMessageThread(mt);
    inputThread->start();
    
    create_tray_icon();
}

void MainWindow::changeEvent(QEvent *event) 
{
    event->accept();
    if( event->type() == QEvent::WindowStateChange )
    {
        if( windowState() == Qt::WindowMinimized )
        {
            this->hide();
        }
    }
}

void MainWindow::closeEvent (QCloseEvent *event)
{
    this->hide();
    event->ignore();
}

void MainWindow::create_tray_icon()
 {
    m_tray_icon = new QSystemTrayIcon(QIcon(":/icon.png"), this);

    connect( m_tray_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onTrayActivate(QSystemTrayIcon::ActivationReason)) );

    QAction *quit_action = new QAction( "Exit", m_tray_icon );
    connect( quit_action, SIGNAL(triggered()), this, SLOT(onTrayExit()) );

    QAction *hide_action = new QAction( "Show/Hide", m_tray_icon );
    connect( hide_action, SIGNAL(triggered()), this, SLOT(onTrayShowHide()) );

    QMenu *tray_icon_menu = new QMenu;
    tray_icon_menu->addAction( hide_action );
    tray_icon_menu->addAction( quit_action );

    m_tray_icon->setContextMenu( tray_icon_menu );

    m_tray_icon->show();
  }


void MainWindow::onTrayExit() {
    QApplication::exit(0);
}

void MainWindow::onTrayShowHide() {
    if( isVisible() )
    {
        hide();
    }
    else
    {
        show();
        raise();
        setFocus();
    }
}

void MainWindow::onTrayActivate( QSystemTrayIcon::ActivationReason reason )
{
    if( reason )
    {
        if( reason != QSystemTrayIcon::DoubleClick )
        return;
    }

    if( isVisible() )
    {
        hide();
    }
    else
    {
        show();
        raise();
        setFocus();
    }
}

MainWindow::~MainWindow()
{
    if (sd != nullptr) {
        sd->saveToDisk();
    }
    if (m_tray_icon != nullptr) {
        m_tray_icon->hide();
        delete m_tray_icon;
    }
    
    delete ui;
}

void MainWindow::on_actionE_xit_triggered()
{
    std::string text = "Exiting";
    appendLabel(text);
    QApplication::exit(0);
}

void MainWindow::on_action_Add_Location_triggered()
{
    QString qdir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                "/home",
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
    if (!qdir.isNull()) {
        std::wstring dir = qdir.toStdWString();
        mt->scanDirectory(dir);
    }
}

void MainWindow::on_action_View_Locations_triggered()
{
    if (viewLocationsDialog == nullptr) {
        viewLocationsDialog = new ViewLocationsDialog(this);
        viewLocationsDialog->setSavedData(sd);
    }
    viewLocationsDialog->exec();
}
