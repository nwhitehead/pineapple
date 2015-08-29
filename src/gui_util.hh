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

#include <wx/bitmap.h>
#include <wx/filename.h>
#include <wx/webview.h>

/// Get filename of python
std::string python_fullpath();

/// Get filename of server
std::string server_fullpath();

/// Get filename for an app resource
std::string resource_filename(std::string filename);

/// Load an image file and process it into an appropriate toolbar icon
wxBitmap toolbar_icon(std::string filename);

/// Click a menu item by name (hidden in our webview)
void jupyter_click_cell(wxWebView *wv, std::string id);

/// Find a fresh new notebook filename
/// Returns whether filename was found
/// prefix: What to start name with
/// suffix: What to end with
/// fullname: fresh filename found (if found)
/// userdata: if true, use less visible userdata area
bool FindNewFileName(wxFileName &fullname, std::string prefix, std::string suffix,
                     int max_num_tries, bool userdata);
