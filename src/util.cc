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

#include "util.hh"

#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "config.hh"

std::string read_all_file(std::string filename)
{
    std::ifstream ifs(filename);
    return std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
}

bool files_different(std::string filename, std::string other_filename)
{
    try {
        std::string contents(read_all_file(filename));
        std::string other_contents(read_all_file(other_filename));
        return contents != other_contents;
    } catch(std::exception &e) {
        return false;
    }
}


std::vector<std::string> read_file_lines(std::string filename)
{
    std::ifstream ifs(filename);
    std::vector<std::string> v;
    std::string s;
    while (std::getline(ifs, s)) {
        v.push_back(std::string(s));
    }
    return v;
}

void write_file(std::string filename, std::string contents)
{
    std::ofstream ofs(filename);
    ofs << contents;
}

void write_file_lines(std::string filename, std::vector<std::string> v)
{
    std::ofstream ofs;
    ofs.open(filename, std::fstream::out);
    for(auto it = v.begin(); it != v.end(); ++it) {
        ofs << *it << std::endl;
    }
}

std::string replace_one(std::string &s, std::string mud, std::string gold)
{
    if (s.find(mud)) {
        return s.replace(s.find(mud), mud.length(), gold);
    }
    return s;
}

std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> items;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        items.push_back(item);
    }
    return items;
}

std::string urlencode(std::string path)
{
    std::ostringstream result;
    result.fill('0');
    result << std::hex;
    for (auto c : path) {
        if (std::isalnum(c) || c == '/' || c == '-' || c == '_' || c == '.' || c == '~') {
            result << c;
        } else {
            result << '%' << std::setw(2) << static_cast<int>(c);
        }
    }
    return result.str();
}

bool has_ending (std::string const &s, std::string const &ending) {
    if (s.length() >= ending.length()) {
        return (0 == s.compare(s.length() - ending.length(),
            ending.length(), ending));
    } else {
        return false;
    }
}
