#pragma once

#include <string>

#include <wx/bitmap.h>
#include <wx/filename.h>
#include <wx/webview.h>

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
