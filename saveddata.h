#ifndef SAVEDDATA_H
#define SAVEDDATA_H

#include <unordered_set>
#include <unordered_map>
#include <string>
#define DEFAULT_SCAN_TIME 60

class SavedData
{
    std::unordered_set<std::wstring> locations;
    std::unordered_map<std::wstring, std::unordered_set<std::wstring>> knownFiles;
    int scanTime = DEFAULT_SCAN_TIME;
    
public:
    SavedData();
    std::unordered_set<std::wstring> *getLocations();
    std::unordered_map<std::wstring, std::unordered_set<std::wstring>> *getKnownFiles();
    bool saveToDisk();
    bool loadFromDisk();
    int getScanTime() const;
    void setScanTime(int value);
};

#endif // SAVEDDATA_H
