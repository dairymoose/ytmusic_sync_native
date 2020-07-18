#ifndef MESSAGETHREAD_H
#define MESSAGETHREAD_H

#include <QThread>
#include <QLabel>
#include "saveddata.h"
#include <queue>
#include <QPlainTextEdit>
#include "textupdater.h"

class MessageThread : public QThread
{
public:
    MessageThread();
    void sendMessage(std::string &&msg);
    void sendFileData(unsigned char *bytes, uint32_t byteLen);
    void appendLabel(std::string &toAppend);
    void appendLabel(std::wstring &toAppend);
    void sendMetaMessage(int metaMessageTag, std::string &msg);
    QLabel *label = nullptr;
    QPlainTextEdit *edit = nullptr;
    TextUpdater *textUpdater = nullptr;
    
    void setSavedData(SavedData *value);
    
    void scanDirectory(std::wstring& dir, bool uploadNewFiles = false);
    
    void uploadFile(std::wstring wpath);
    void addToUploadQueue(std::wstring wpath);
    
    std::queue<std::string> replyQueue;
    
protected:
    virtual void run();
    
private:
    void scan();
    SavedData *sd = nullptr;
    std::queue<std::wstring> fileQueue;
    void processUploadQueue();
    
};

#endif // MESSAGETHREAD_H
