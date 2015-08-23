#include "gui_util.hh"

#include <iostream>
#include <string>

#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/webview.h>

#include "config.hh"

/// Get filename for an app resource
std::string resource_filename(std::string filename)
{
    wxPathList pathList;
    pathList.Add(std::string(".")); // for dev builds
    wxString respath(wxStandardPaths::Get().GetResourcesDir());
    std::string std_respath(respath);
    if (std_respath.find(' ') != std::string::npos) {
        wxLogFatalError("This application cannot be run from a directory containing spaces");
    }
    pathList.Add(respath);
    wxString fname(pathList.FindValidPath(filename));
    if (!fname) {
        wxLogError("Resource not found: %s", filename);
        return std::string();
    }
    return std::string(fname);
}

/// Load an image file and process it into an appropriate toolbar icon
wxBitmap toolbar_icon(std::string filename)
{
    wxFileName wfname(filename);
    std::string fname(resource_filename(std::string(wfname.GetFullPath())));
    wxLogDebug("gui_util toolbar_icon Loading filename=[%s]", fname);
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
