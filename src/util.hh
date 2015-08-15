#pragma once

#include <string>
#include <sstream>
#include <vector>

#include "config.h"

static std::string url_from_filename(std::string filename)
{
    std::string uri(filename);
    return std::string(config::base_url) + std::string(config::path_url) + uri;
}

/// Replace a single occurrence of a string in another with a string
std::string replace_one(std::string &s, std::string mud, std::string gold)
{
    return s.replace(s.find(mud), mud.length(), gold);
}

/// Split a string into a vector based on a delimiter character
static std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> items;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delimiter)) {
        items.push_back(item);
    }
    return items;
}

