#include "viewlocationsdialog.h"
#include "ui_viewlocationsdialog.h"

#include <QString>
#include <QMessageBox>
#include <QMenu>
#include <QAction>

#include <QScrollArea>

ViewLocationsDialog::ViewLocationsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewLocationsDialog)
{
    ui->setupUi(this);
    
    ui->fileTree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->fileTree,&QTreeWidget::customContextMenuRequested,this,&ViewLocationsDialog::prepareMenu);
    
    this->setWindowFlags(this->windowFlags()
                         | Qt::WindowMaximizeButtonHint
                         | Qt::WindowCloseButtonHint);
}

void ViewLocationsDialog::prepareMenu( const QPoint & pos ) {
    QTreeWidget *tree = ui->fileTree;
    
    QTreeWidgetItem *item = tree->itemAt( pos );
    
    if (item != nullptr) {
        QAction *viewFilesAction = new QAction("&View Files");
        QAction *removeAction = new QAction("&Remove");
        
        QMenu menu(this);
        menu.addAction(viewFilesAction);
        menu.addAction(removeAction);
        
        QPoint pt(pos);
        QAction * selected = menu.exec( tree->mapToGlobal(pos) );
        QString qlocation = item->text(0);
        std::wstring location = qlocation.toStdWString();
        if (selected != nullptr) {
            if (selected == viewFilesAction) {
                if (fileList == nullptr) {
                    fileList = new FileList();
                    fileList->setSavedData(sd);
                }
                fileList->setLocation(location);
                fileList->exec();
            } else if (selected == removeAction) {
                QMessageBox::StandardButton reply;
                  reply = QMessageBox::question(this, "Confirmation", "Are you sure you want to remove this directory?",
                                                QMessageBox::Yes|QMessageBox::No);
                  if (reply == QMessageBox::Yes) {
                      sd->getLocations()->erase(location);
                      sd->getKnownFiles()->erase(location);
                      delete item;
                      
                      sd->saveToDisk();
                  } else {
                    ;
                  }
            }
        }
    }
}

void ViewLocationsDialog::present() {
    if (sd->getLocations() != nullptr) {
        this->ui->fileTree->clear();
        for (auto it : *sd->getLocations()) {
            QTreeWidgetItem *item = new QTreeWidgetItem(this->ui->fileTree);
            item->setText(0, QString::fromStdWString(it));
            this->ui->fileTree->addTopLevelItem(item);
        }
    }
}

ViewLocationsDialog::~ViewLocationsDialog()
{
    delete ui;
}

void ViewLocationsDialog::setSavedData(SavedData *savedData)
{
    this->sd = savedData;
}

void ViewLocationsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    this->close();
}

void ViewLocationsDialog::showEvent(QShowEvent *event)
{
    this->present();
}
