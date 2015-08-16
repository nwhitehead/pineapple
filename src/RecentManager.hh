#pragma once

#include <string>
#include <vector>

#include <wx/filename.h>
#include <wx/stdpaths.h>

#include "config.hh"
#include "util.hh"
#include "MainApp.hh"

class RecentManager {
public:
    RecentManager() {
        std::string path(wxStandardPaths::Get().GetUserDataDir());
        std::string recentfile(wxFileName(path, config::recent_files_filename).GetFullPath());
        state = read_file_lines(recentfile);
    }
    ~RecentManager() {
    }
    std::string GetMostRecent() {
        return state[state.size() - 1];
    }
    void Add(std::string filename) {
        state.push_back(filename);
    }
private:
    void SyncWrite();
    std::vector<std::string> state;
};
