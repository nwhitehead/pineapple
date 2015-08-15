#pragma once

#include <string>

#include "config.h"

static std::string url_from_filename(std::string filename)
{
    std::string uri(filename);
    return std::string(config::base_url) + std::string(config::path_url) + uri;
}
