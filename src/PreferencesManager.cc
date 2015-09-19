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

#include "PreferencesManager.hh"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <map>

#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

#include "config.hh"
#include "util.hh"
#include "MainApp.hh"

PreferencesManager::PreferencesManager()
{
    wxString dir(wxStandardPaths::Get().GetUserDataDir());
    if (!wxDirExists(dir)) {
        // Create directory if it doesn't exist
        wxMkdir(dir);
    }
    backing_file = std::string(
        wxFileName(dir, config::preferences_filename).GetFullPath()
    );
}

bool PreferencesManager::SyncRead()
{
    std::lock_guard<std::recursive_mutex> lck(pref_mutex);
    std::vector<std::string> lines;
    try {
        lines = read_file_lines(backing_file);
    } catch(...) {
        return false;
    }
    for (auto line: lines) {
        std::vector<std::string> items = split(line, '=');
        Set(items[0], items[1]);
    }
    return true;
}

bool PreferencesManager::SyncWrite()
{
    std::lock_guard<std::recursive_mutex> lck(pref_mutex);
    std::vector<std::string> lines;
    for (auto it = state.begin(); it != state.end(); it++) {
        std::string line = it->first + std::string("=") + it->second;
        lines.push_back(line);
    }
    write_file_lines(backing_file, lines);
    return true;
}

PreferencesManager::~PreferencesManager()
{
    std::lock_guard<std::recursive_mutex> lck(pref_mutex);
    SyncWrite();
}

std::string PreferencesManager::Get(std::string key, std::string default_value)
{
    std::lock_guard<std::recursive_mutex> lck(pref_mutex);
    if (state.find(key) != state.end()) {
        return state[key];
    }
    Set(key, default_value);
    return default_value;
}

void PreferencesManager::Set(std::string key, std::string value)
{
    std::lock_guard<std::recursive_mutex> lck(pref_mutex);
    state[key] = value;
    SyncWrite();
}

bool PreferencesManager::GetBool(std::string key, bool default_value)
{
    std::lock_guard<std::recursive_mutex> lck(pref_mutex);
    if (state.find(key) != state.end()) {
        return (state[key] == std::string("yes")); 
    }
    SetBool(key, default_value);
    return default_value;
}

void PreferencesManager::SetBool(std::string key, bool value)
{
    std::lock_guard<std::recursive_mutex> lck(pref_mutex);
    state[key] = value ? std::string("yes") : std::string("no");
    SyncWrite();
}

int PreferencesManager::GetInt(std::string key, int default_value)
{
    std::lock_guard<std::recursive_mutex> lck(pref_mutex);
    if (state.find(key) != state.end()) {
        return std::atoi(state[key].c_str()); 
    }
    SetInt(key, default_value);
    return default_value;
}

void PreferencesManager::SetInt(std::string key, int value)
{
    std::lock_guard<std::recursive_mutex> lck(pref_mutex);
    state[key] = std::to_string(value);
    SyncWrite();
}
