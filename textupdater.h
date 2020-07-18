#ifndef TEXTUPDATER_H
#define TEXTUPDATER_H

#include <QPlainTextEdit>
#include <queue>
#include <QTimer>

class TextUpdater
{
private:
    QPlainTextEdit *edit = nullptr;
    
public:
    TextUpdater();
    QPlainTextEdit *getEdit() const;
    void setEdit(QPlainTextEdit *value);
    
    void appendLabel(std::string& toAppend);
    void appendLabel(std::wstring& toAppend);
    
    std::queue<QString> toAppend;
    QTimer timer;
};

#endif // TEXTUPDATER_H
