// Copyright 2015 Nathan Whitehead
// Distributed under the terms of the GNU General Public License

// This file is part of Pineapple.
//
// Pineapple is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Pineapple is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Pineapple.  If not, see <http://www.gnu.org/licenses/>.

#include "RecentManager.hh"

#include <algorithm>
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
    // Take out any entries that are of files we cannot open
    state.erase(
        std::remove_if( state.begin(), state.end(), 
            [](std::string x) -> bool {
                return !wxFileExists(x);
            }
        ),
        state.end()
    );
    // Remove elements from front to make size small enough
    while (state.size() > config::recent_number) {
        state.erase(state.begin());
    }
}

RecentManager::~RecentManager()
{
    SyncWrite();
}

std::string RecentManager::Get()
{
    if (state.size() > 0) {
        return state[state.size() - 1];
    }
    return std::string();
}

void RecentManager::Add(std::string filename)
{
    state.push_back(filename);
}

void RecentManager::SyncWrite()
{
    write_file_lines(backing_file, state);
}
