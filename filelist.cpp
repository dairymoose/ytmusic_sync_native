#include "filelist.h"
#include "ui_filelist.h"

FileList::FileList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileList)
{
    ui->setupUi(this);
    
    this->setWindowFlags(Qt::CustomizeWindowHint
                      | Qt::WindowSystemMenuHint
                      | Qt::WindowMaximizeButtonHint
                      | Qt::WindowCloseButtonHint);
}

FileList::~FileList()
{
    delete ui;
}

void FileList::showEvent(QShowEvent *event)
{
    this->ui->fileList->clear();
    if (this->sd != nullptr) {
        for (auto item : (*this->sd->getKnownFiles())[location]) {
            QString qitem = QString::fromStdWString(item);
            this->ui->fileList->addItem(qitem);
        }
    }
}

std::wstring *FileList::getLocation()
{
    return &location;
}

void FileList::setLocation(const std::wstring &value)
{
    location = value;
}

void FileList::on_buttonBox_clicked(QAbstractButton *button)
{
    location.clear();
    this->close();
}

void FileList::setSavedData(SavedData *savedData)
{
    this->sd = savedData;
}

