#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

#include "config.hh"
#include "util.hh"
#include "MainApp.hh"

class RecentManager {
public:
    RecentManager() {
        wxString dir(wxStandardPaths::Get().GetUserDataDir());
        if (!wxDirExists(dir)) {
            // Create if it doesn't exist
            wxMkdir(dir);
        }
        backing_file = std::string(wxFileName(dir, config::recent_files_filename).GetFullPath());
        std::cout << "RECENT FILES filename is " << backing_file << std::endl;
        state = read_file_lines(backing_file);
    }
    ~RecentManager() {
//        std::cout << "RECENT FILES writing [" << backing_file << "]" << std::endl;
//        write_file_lines(backing_file, state);
//        std::cout << "RECENT FILES DONE writing [" << backing_file << "]" << std::endl;
    }
    std::string GetMostRecent() {
        return state[state.size() - 1];
    }
    void Add(std::string filename) {
        state.push_back(filename);
    }
private:
    void SyncWrite();
    std::string backing_file;
    std::vector<std::string> state;
};
