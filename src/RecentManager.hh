#pragma once

#include <string>
#include <vector>

class RecentManager {
public:
    RecentManager();
    ~RecentManager();
    std::string GetMostRecent();
    void Add(std::string filename);
private:
    void SyncWrite();
    std::string backing_file;
    std::vector<std::string> state;
};
