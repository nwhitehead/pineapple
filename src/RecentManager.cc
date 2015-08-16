#include "RecentManager.hh"

#include <iostream>
#include <string>
#include <vector>

#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

#include "config.hh"
#include "util.hh"
#include "MainApp.hh"

RecentManager::RecentManager()
{
    wxString dir(wxStandardPaths::Get().GetUserDataDir());
    if (!wxDirExists(dir)) {
        // Create directory if it doesn't exist
        wxMkdir(dir);
    }
    backing_file = std::string(
        wxFileName(dir, config::recent_files_filename).GetFullPath()
    );
    state = read_file_lines(backing_file);
}

RecentManager::~RecentManager()
{
    SyncWrite();
}

std::string RecentManager::GetMostRecent()
{
    return state[state.size() - 1];
}

void RecentManager::Add(std::string filename)
{
    state.push_back(filename);
}

void RecentManager::SyncWrite()
{
    write_file_lines(backing_file, state);
}
