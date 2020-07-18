#ifndef FILELIST_H
#define FILELIST_H

#include <QDialog>
#include <QAbstractButton>
#include "saveddata.h"

namespace Ui {
class FileList;
}

class FileList : public QDialog
{
    Q_OBJECT
    
public:
    explicit FileList(QWidget *parent = nullptr);
    ~FileList();
    void setSavedData(SavedData *value);
    
    std::wstring *getLocation();
    void setLocation(const std::wstring &value);
    
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    
protected:
    virtual void showEvent(QShowEvent *event);
    
private:
    Ui::FileList *ui;
    
    SavedData *sd = nullptr;
    std::wstring location;
};

#endif // FILELIST_H
