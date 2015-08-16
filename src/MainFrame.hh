#pragma once

#include <signal.h>
#include <string>

#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/process.h>
#include <wx/toolbar.h>
#include <wx/webview.h>

#include "callback.hh"

class MainFrame: public wxFrame
{
    enum {
        wxID_SAVE_HTML = 10000,
        wxID_SAVE_AS, wxID_PROPERTIES,
        wxID_INSERT, wxID_DELETE, wxID_UNDELETE,
        wxID_SPLIT, wxID_MERGE,
        wxID_MOVE_UP, wxID_MOVE_DOWN,
        wxID_RUN, wxID_RUN_NEXT, wxID_RUN_ALL, wxID_RUN_ALL_ABOVE, wxID_RUN_ALL_BELOW,
        wxID_CELL_CODE, wxID_CELL_MARKDOWN, wxID_CELL_RAW,
        wxID_KERNEL_INTERRUPT, wxID_KERNEL_RESTART, wxID_KERNEL_RECONNECT,
        wxID_HELP_KEYBOARD, wxID_HELP_NOTEBOOK, wxID_HELP_MARKDOWN,
        wxID_KERNEL_BUSY,
    };

public:

    MainFrame(std::string url0, std::string filename, const wxString &title, const wxPoint &pos, const wxSize &size, bool indirect_load);

    static MainFrame *Spawn(std::string url, std::string filename, bool indirect_load=false);
    static MainFrame *CreateNew(bool indirect_load=false);

    wxProcess *server;
    wxWebView *webview;
    wxMenuBar *menubar;
    wxToolBar *toolbar;
    std::string url;
    std::string local_filename;
    CallbackHandler handler;

    void OnClose(wxCloseEvent &event);
    void OnSubprocessTerminate(wxProcessEvent &event);
    void OnTitleChanged(wxWebViewEvent &event);

    void OnNew(wxCommandEvent &event);
    void OnOpen(wxCommandEvent &event);
    void OnSaveAs(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnProperties(wxCommandEvent &event);
    void OnMenuClose(wxCommandEvent &event);

    void SetupMenu();
    void SetupToolbar();
    void SetupWebView();
    void SetupLayout(const wxPoint &pos, const wxSize &size);
    void SetupBindings();
    void LoadDocument(bool indirect_load=true);

    void eval_javascript(std::string expression, Callback::t success, Callback::t failure=Callback::debug);
};
