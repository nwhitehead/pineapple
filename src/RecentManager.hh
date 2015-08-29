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
