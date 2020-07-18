#include "textupdater.h"
#include <QTime>

QPlainTextEdit *TextUpdater::getEdit() const
{
    return edit;
}

void TextUpdater::setEdit(QPlainTextEdit *value)
{
    edit = value;
}

void TextUpdater::appendLabel(std::string& toAppend) {
    //QString qs = this->label->text();
    QString qs;
    qs.append("[");
    qs.append(QTime().currentTime().toString());
    qs.append("] ");
    qs.append(toAppend.c_str());
    //qs.append("\r\n");
    //this->edit->setText(qs);
    this->edit->appendPlainText(qs);
}

void TextUpdater::appendLabel(std::wstring& toAppend) {
    //QString qs = this->label->text();
    //QString qs = this->edit->text();
    QString qs;
    qs.append("[");
    qs.append(QTime().currentTime().toString());
    qs.append("] ");
    qs.append(QString::fromStdWString(toAppend));
    //qs.append("\r\n");
    //this->edit->setText(qs);
    this->edit->appendPlainText(qs);
}

TextUpdater::TextUpdater()
{
    timer.start(500);
}
