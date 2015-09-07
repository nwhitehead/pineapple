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

#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "config.hh"

/**
 * Generic utility functions
 */

/// Read all contents of file into a string
std::string read_all_file(std::string filename);

/// Check if two files are any different
bool files_different(std::string filename, std::string other_filename);

/// Read all contents of a file into a vector of strings (per line)
std::vector<std::string> read_file_lines(std::string filename);

/// Write string to file
void write_file(std::string filename, std::string contents);

/// Write vector of strings into a file
void write_file_lines(std::string filename, std::vector<std::string> v);

/// Replace a single occurrence of a string in another with a string
std::string replace_one(std::string &s, std::string mud, std::string gold);

/// Split a string into a vector based on a delimiter character
std::vector<std::string> split(const std::string &s, char delimiter);

/// Urlencode a string so it works inside url
std::string urlencode(std::string path);

/// Check if a string has given ending
bool has_ending (std::string const &s, std::string const &ending);


/**
 * Specific to app
 */

/// Create URL to visit from local filename
std::string url_from_filename(std::string filename);

/// Create URL for nbconvert from local filename
std::string export_url_from_filename(std::string filename, std::string format);
