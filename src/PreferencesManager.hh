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

#pragma once

#include <string>
#include <map>
#include <mutex>

class PreferencesManager {
public:
    /// RAII
    PreferencesManager();
    ~PreferencesManager();

    std::string Get(std::string key, std::string default_value);
    void Set(std::string key, std::string value);

    bool GetBool(std::string key, bool default_value);
    void SetBool(std::string key, bool value);

    int GetInt(std::string key, int default_value);
    void SetInt(std::string key, int value);

    /// Read data structure from backing file
    bool SyncRead();

    /// Write data structure to backing file
    bool SyncWrite();

private:
    std::recursive_mutex pref_mutex;
    std::string backing_file;
    std::map<std::string, std::string> state;
};
