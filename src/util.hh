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

/**
 * Specific to app
 */

/// Create URL to visit from local filename
std::string url_from_filename(std::string filename);

/// Create URL for nbconvert from local filename
std::string export_url_from_filename(std::string filename, std::string format);
