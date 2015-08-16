#pragma once

#include <string>
#include <vector>

class RecentManager {
public:
    /// RAII
    RecentManager();
    ~RecentManager();

    /// Get the most recently opened filename
    std::string Get();
    
    /// Add a new filename to consider
    void Add(std::string filename);

private:
    /// Write data structure to backing file
    void SyncWrite();

    std::string backing_file;
    std::vector<std::string> state;
};
