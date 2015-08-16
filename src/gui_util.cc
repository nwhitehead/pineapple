#include "gui_util.hh"

#include <string>

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/webview.h>

#include "config.hh"

/// Load an image file and process it into an appropriate toolbar icon
wxBitmap toolbar_icon(std::string filename)
{
    return wxBitmap(wxImage(filename).Rescale(config::toolbar_width, config::toolbar_height, wxIMAGE_QUALITY_BICUBIC));
}

/// Click a menu item by name (hidden in our webview)
void jupyter_click_cell(wxWebView *wv, std::string id)
{
    std::string cmd = "Jupyter.menubar.element.find('#" + id + "').click();";
    wv->RunScript(cmd);
}
