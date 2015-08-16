#pragma once

#include <string>

#include <wx/bitmap.h>
#include <wx/webview.h>

/// Load an image file and process it into an appropriate toolbar icon
wxBitmap toolbar_icon(std::string filename);

/// Click a menu item by name (hidden in our webview)
void jupyter_click_cell(wxWebView *wv, std::string id);
