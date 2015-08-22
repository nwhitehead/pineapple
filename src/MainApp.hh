#pragma once

#include <string>
#include <vector>

#include <wx/app.h>
#include <wx/process.h>

#include "config.hh"
#include "MainFrame.hh"
#include "RecentManager.hh"

class MainApp: public wxApp
{
public:
    MainApp() : recently_used((SetAppName(wxString(config::app_name)), RecentManager()))
    { }

    virtual bool OnInit();
    virtual int OnExit();
    virtual void MacOpenFile(const wxString &filename);
    void CloseAll();
    void OnQuit(wxCommandEvent &event);
    void OnSubprocessTerminate(wxProcessEvent &event);
    static void OnAbout(wxCommandEvent &event);

    wxProcess *server;
    MainFrame *frame;
    std::string blank_notebook;
    std::string load_page;
    RecentManager recently_used;
    std::vector<wxWindow *> frames;
    bool waiting_to_quit;
};

wxDECLARE_APP(MainApp);
