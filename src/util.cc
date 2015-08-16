#include "util.hh"

#include <fstream>
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
}

std::string replace_one(std::string &s, std::string mud, std::string gold)
{
    return s.replace(s.find(mud), mud.length(), gold);
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
