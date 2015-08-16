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

/// Read all contents of a file into a vector of strings (per line)
std::vector<std::string> read_file_lines(std::string filename);

/// Replace a single occurrence of a string in another with a string
std::string replace_one(std::string &s, std::string mud, std::string gold);

/// Split a string into a vector based on a delimiter character
std::vector<std::string> split(const std::string &s, char delimiter);

/**
 * Specific to app
 */

/// Create URL to visit from local filename
std::string url_from_filename(std::string filename);
