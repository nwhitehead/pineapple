#pragma once

#include <string>

#include <wx/app.h>
#include <wx/process.h>

#include "MainFrame.hh"
#include "RecentManager.hh"

class MainApp: public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();
    void OnSubprocessTerminate(wxProcessEvent &event);
    static void OnAbout(wxCommandEvent &event);

    wxProcess *server;
    MainFrame *frame;
    std::string blank_notebook;
    std::string load_page;
    RecentManager recently_used;
};

wxDECLARE_APP(MainApp);
