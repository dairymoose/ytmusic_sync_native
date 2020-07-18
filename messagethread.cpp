#include "messagethread.h"

#include "base64.h"

#include <windows.h>
#include <iostream>
#include <sstream>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QApplication>
#include <QDirIterator>
#include <QTime>

#include <string>
#include <fstream>

#define METAMESSAGE_TAG_UPLOADSTART 1
#define METAMESSAGE_TAG_UPLOADEND 2


void MessageThread::appendLabel(std::string& toAppend) {
    QString qs = this->label->text();
    qs.append("[");
    qs.append(QTime().currentTime().toString());
    qs.append("] ");
    qs.append(toAppend.c_str());
    qs.append("\r\n");
    this->label->setText(qs);
}

void MessageThread::appendLabel(std::wstring& toAppend) {
    QString qs = this->label->text();
    qs.append("[");
    qs.append(QTime().currentTime().toString());
    qs.append("] ");
    qs.append(QString::fromStdWString(toAppend));
    qs.append("\r\n");
    this->label->setText(qs);
}

void MessageThread::sendMessage(std::string&& msg) {
    std::ostringstream ss;
    ss<<"{\"x\":\""<<msg<<"\"}";
    std::string output = ss.str();
    uint32_t len = output.length();
    std::cout.write((char *)&len, 4);
    std::cout<<output;
    std::cout.flush();
}

void MessageThread::sendMetaMessage(int metaMessageTag, std::string& msg) {
    std::ostringstream ss;
    ss<<"{\"m\":\""<<metaMessageTag<<(msg != "" ? "/" : "")<<msg<<"\"}";
    std::string output = ss.str();
    uint32_t len = output.length();
    std::cout.write((char *)&len, 4);
    std::cout<<output;
    std::cout.flush();
}

#define SEND_CHUNK_SIZE (uint32_t)(400*1024)
void MessageThread::sendFileData(unsigned char *bytes, uint32_t byteLen) {
    uint32_t bytesSent = 0;
    uint32_t bytesToSend = byteLen;
    while (bytesToSend > 0) {
        uint32_t len = std::min(bytesToSend, SEND_CHUNK_SIZE);
        
        std::ostringstream ss;
        ss<<"{\"f\":\"";
        ss.write((const char *)(bytes + bytesSent), len);
        ss<<"\"}";
        std::string output = ss.str();
        uint32_t output_len = output.length();
        
        std::cout.write((char *)&output_len, 4);
        std::cout<<output;
        std::cout.flush();
        
        bytesToSend -= len;
        bytesSent += len;
    }
    
}

#define BUFSIZE 100*1024
std::string getFileBytes(std::string& path) {
    std::ifstream fin;
    std::stringstream ss;
    fin.open(path, std::ios::binary);
    while (!fin.eof()) {
        char buf[BUFSIZE];
        fin.read(buf, BUFSIZE);
        ss.write(buf, fin.gcount());
    }
    fin.close();
    return ss.str();
}

void writeFileBytes(std::string path, unsigned char *bytes, unsigned int len) {
    std::ofstream fout;
    fout.open(path, std::ios::binary);
    fout.write((const char*)bytes, len);
    fout.close();
}

const uint32_t MOD_ADLER = 65521;

uint32_t adler32(unsigned char *data, size_t len) 
/* 
    where data is the location of the data in physical memory and 
    len is the length of the data in bytes 
*/
{
    uint32_t a = 1, b = 0;
    size_t index;
    
    // Process each byte of the data in order
    for (index = 0; index < len; ++index)
    {
        a = (a + data[index]) % MOD_ADLER;
        b = (b + a) % MOD_ADLER;
    }
    
    return (b << 16) | a;
}

void MessageThread::processUploadQueue() {
    bool doUpload = false;
    if (!fileQueue.empty()) {
        doUpload = true;
        std::stringstream txtss;
        txtss<<"Uploading "<<fileQueue.size()<<" items...";
        std::string txt = txtss.str();
        appendLabel(txt);
    }
    while (!fileQueue.empty()) {
        auto& path = fileQueue.front();
        fileQueue.pop();
        this->uploadFile(path);

        while (replyQueue.empty()) {
            Sleep(1);
        }
        replyQueue.pop();
    }
    if (doUpload) {
        std::string txtout = "Finished upload";
        appendLabel(txtout);
    }
}

void MessageThread::scanDirectory(std::wstring &dir, bool uploadNewFiles)
{
    if (this->sd != nullptr) {
        
        sd->getLocations()->insert(dir);
        std::wstring text = L"Added location: ";
        text += dir;
        if (!uploadNewFiles)
            appendLabel(text);
        auto it = sd->getKnownFiles()->find(dir);
        if (it == sd->getKnownFiles()->end()) {
            (*sd->getKnownFiles())[dir] = std::unordered_set<std::wstring>();
        }
        auto& fileSet = (*sd->getKnownFiles())[dir];
        
        QString qdir = QString::fromStdWString(dir);
        QDirIterator dirIt(qdir, QDirIterator::Subdirectories);
        std::string userText1 = "Registering files from directory...";
        if (!uploadNewFiles)
            appendLabel(userText1);
        int counter = 0;
        while (dirIt.hasNext()) {
            QString current = dirIt.next();
            QFileInfo info = dirIt.fileInfo();
            if (info.isFile() && 
                    (info.fileName().endsWith(".wav") || 
                     info.fileName().endsWith(".mp3") || 
                     info.fileName().endsWith(".flac") || 
                     info.fileName().endsWith(".m4a") || 
                     info.fileName().endsWith(".opus") || 
                     info.fileName().endsWith(".ogg") || 
                     info.fileName().endsWith(".oga"))) {
                std::wstring path = info.filePath().toStdWString();
                if (uploadNewFiles) {
                    if (fileSet.find(path) == fileSet.end()) {
                        std::wstring userText3 = L"New file: ";
                        userText3 += path;
                        appendLabel(userText3);
                        this->addToUploadQueue(path);
                    }
                }
                fileSet.insert(path);
                ++counter;
            }
            QApplication::processEvents();
        }
        std::ostringstream ss2;
        ss2<<"Finished adding files, count = "<<counter;
        std::string userText2 = ss2.str();
        if (!uploadNewFiles)
            appendLabel(userText2);
        sd->saveToDisk();
    }
}

void MessageThread::scan() {
    if (this->sd != nullptr) {
        this->processUploadQueue();
        for (auto it : *sd->getLocations()) {
            std::wstring& location = it;
            this->scanDirectory(location, true);
        }
    }
}

void MessageThread::setSavedData(SavedData *value)
{
    sd = value;
}

void MessageThread::uploadFile(std::wstring wpath) {
    QFile file(QString::fromStdWString(wpath));
    if (!file.open(QIODevice::ReadOnly)) {
        std::string err = "Error opening file";
        appendLabel(err);
    }
    std::string path = file.fileName().toStdString();
    std::string bytes = getFileBytes(path);
    file.close();
    size_t out_len;
    unsigned char *encoded = base64_encode((const unsigned char *)bytes.c_str(), bytes.length(), &out_len);
    
    
    QFileInfo fileInfo(file);
    QString filename(fileInfo.fileName());
    std::string title = filename.toStdString();
    sendMetaMessage(METAMESSAGE_TAG_UPLOADSTART, title);
    
    sendFileData(encoded, out_len);
    std::ostringstream len_ss;
    len_ss<<out_len;
    std::string len_ss_str = len_ss.str();
    sendMetaMessage(METAMESSAGE_TAG_UPLOADEND, len_ss_str);
    //writeFileBytes("test.wav", encoded, out_len);
    free(encoded);
}

void MessageThread::addToUploadQueue(std::wstring wpath)
{
    fileQueue.push(wpath);
}

void MessageThread::run() {
    std::string s1 = "Start thread";
    appendLabel(s1);
    sendMessage("start");
    for (;;) {
        this->scan();
        if (this->sd != nullptr) {
            Sleep(this->sd->getScanTime()*1000);
        } else {
            Sleep(DEFAULT_SCAN_TIME);
        }
    }
    std::string s2 = "Finished thread";
    appendLabel(s2);
}

MessageThread::MessageThread()
{
    
}

