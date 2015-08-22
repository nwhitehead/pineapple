#include "gui_util.hh"

#include <iostream>
#include <string>

#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/stdpaths.h>
#include <wx/webview.h>

#include "config.hh"

/// Get filename for an app resource
std::string resource_filename(std::string filename)
{
    wxPathList pathList;
    pathList.Add(std::string(".")); // for dev builds
    pathList.Add(wxStandardPaths::Get().GetResourcesDir());
    wxString fname(pathList.FindValidPath(filename));
    if (!fname) {
        std::cout << "NOT FOUND " << filename << std::endl;
        return std::string();
    }
    return std::string(fname);
}

/// Load an image file and process it into an appropriate toolbar icon
wxBitmap toolbar_icon(std::string filename)
{
    wxFileName wfname(filename);
    std::string fname(resource_filename(std::string(wfname.GetFullPath())));
    std::cout << "LOADING " << fname << std::endl;
    if (fname.empty()) {
        return wxArtProvider::GetBitmap(wxART_QUESTION);
    }
    return wxBitmap(wxImage(fname).Rescale(config::toolbar_width, config::toolbar_height, wxIMAGE_QUALITY_BICUBIC));
}

/// Click a menu item by name (hidden in our webview)
void jupyter_click_cell(wxWebView *wv, std::string id)
{
    std::string cmd = "Jupyter.menubar.element.find('#" + id + "').click();";
    wv->RunScript(cmd);
}
