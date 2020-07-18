#ifndef VIEWLOCATIONSDIALOG_H
#define VIEWLOCATIONSDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <unordered_set>
#include "saveddata.h"
#include "filelist.h"

namespace Ui {
class ViewLocationsDialog;
}

class ViewLocationsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit ViewLocationsDialog(QWidget *parent = nullptr);
    ~ViewLocationsDialog();
    
    void setSavedData(SavedData *value);
    
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    void prepareMenu( const QPoint & pos );
    
protected:
    virtual void showEvent(QShowEvent *event);
    
private:
    Ui::ViewLocationsDialog *ui;
    
    void present();
    
    SavedData *sd = nullptr;
    FileList *fileList = nullptr;
};

#endif // VIEWLOCATIONSDIALOG_H
