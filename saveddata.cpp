#include "saveddata.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>

std::unordered_set<std::wstring> *SavedData::getLocations()
{
    return &locations;
}

std::unordered_map<std::wstring, std::unordered_set<std::wstring>> *SavedData::getKnownFiles()
{
    return &knownFiles;
}

bool SavedData::loadFromDisk()
{
    QFile jsonFile("ytmusic.json");
    
    if (!jsonFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open ytmusic.json file.");
        return false;
    }
    
    QJsonDocument doc;
    doc = QJsonDocument::fromJson(jsonFile.readAll());
    if (doc.isNull() || doc.isEmpty()) {
        jsonFile.close();
        return false;
    }
    
    const QJsonObject& root = doc.object();
    QJsonArray locationArray = root["locations"].toArray();
    for (auto it : locationArray) {
        QString qlocation = it.toObject()["location"].toString();
        std::wstring location = qlocation.toStdWString();
        this->locations.insert(location);
        QJsonArray knownFilesArray = it.toObject()["knownFiles"].toArray();
        auto& set = this->knownFiles[location];
        for (auto it2 : knownFilesArray) {
            QString qfile = it2.toString();
            std::wstring file = qfile.toStdWString();
            set.insert(file);
        }
    }
    
    this->scanTime = root["scanTime"].toInt(DEFAULT_SCAN_TIME);
    
    jsonFile.close();
    return true;
}

bool SavedData::saveToDisk()
{
    QFile jsonFile("ytmusic.json");
    
    if (!jsonFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open ytmusic.json file.");
        return false;
    }
    
    QJsonObject root;
    QJsonArray locationArray;
    int i=0;
    for (auto it : locations) {
        QJsonObject location;
        location["location"] = QString::fromStdWString(it);
        QJsonArray knownFilesArray;
        auto lookup = knownFiles.find(it);
        if (lookup != knownFiles.end()) {
            std::unordered_set<std::wstring>& files = lookup->second;
            for (auto it2 : files) {
                QJsonValue value = QString::fromStdWString(it2);
                knownFilesArray.append(value);
            }
        }
        location["knownFiles"] = knownFilesArray;
        locationArray.append(location);
        ++i;
    }
    root["locations"] = locationArray;
    root["scanTime"] = scanTime;
    QJsonDocument saveDoc(root);
    long long result = jsonFile.write(saveDoc.toJson());
    if (result == -1) {
        jsonFile.close();
        return false;
    }

    jsonFile.close();
    return true;
}

int SavedData::getScanTime() const
{
    return scanTime;
}

void SavedData::setScanTime(int value)
{
    scanTime = value;
}

SavedData::SavedData()
{
    
}
