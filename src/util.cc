#include "util.hh"

#include <fstream>
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

std::string url_from_filename(std::string filename)
{
    std::string uri(filename);
    return std::string(config::base_url) + std::string(config::path_url) + uri;
}

std::string export_url_from_filename(std::string filename, std::string format)
{
    std::string uri(filename);
    return std::string(config::nbconvert_url) + std::string("/") + format + uri;
}
