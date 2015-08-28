#include "gui_util.hh"

#include <iostream>
#include <sstream>
#include <string>

#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/webview.h>

#include "config.hh"

/// Get filename of python
std::string python_fullpath()
{
    wxString result;
    if (wxGetEnv(config::python_path_env, &result)) {
        // User explicitly told us what to use for python
        return std::string(result);
    }
    // Look for python in resource location
    wxString respath(wxStandardPaths::Get().GetResourcesDir());
    wxFileName wfname(respath, "");
    wfname.AppendDir("python2.7");
    wfname.AppendDir("bin");
    wfname.SetFullName("python");
    return std::string(wfname.GetFullPath());
}

/// Get filename of server
std::string server_fullpath()
{
    wxString result;
    if (wxGetEnv(config::server_path_env, &result)) {
        // User explicitly told us what to use for server script
        return std::string(result);
    }
    wxString respath(wxStandardPaths::Get().GetResourcesDir());
    wxFileName wfname(respath, "pineapple-server");
    return std::string(wfname.GetFullPath());
}

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

bool FindNewFileName(wxFileName &fullname, std::string prefix,
                     std::string suffix, int max_num_tries, bool userdata)
{
    wxString datadir;
    if (userdata) {
        datadir = wxStandardPaths::Get().GetUserDataDir();
    } else {
        datadir = wxStandardPaths::Get().GetAppDocumentsDir();
    }
    int n = 1;
    do {
        std::stringstream ss;
        if (n > 1) {
            ss << prefix << n << suffix;
        } else {
            ss << prefix << suffix;
        }
        fullname = wxFileName(datadir, ss.str());
        wxLogDebug("MainFrame::FindNewFilName trying filename [%s]", fullname.GetFullPath());
        if (!fullname.IsOk()) break;
        if (fullname.IsOk() && !fullname.FileExists()) break;
        if (n > max_num_tries) break;
        n++;
    } while (1);
    if (fullname.IsOk() && !fullname.FileExists()) {
        return true;
    }
    return false;
} 
