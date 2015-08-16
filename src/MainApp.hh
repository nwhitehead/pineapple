#pragma once

#include <memory>
#include <string>
#include <vector>

#include <wx/app.h>
#include <wx/process.h>

#include "MainFrame.hh"
#include "RecentManager.hh"

class MainApp: public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();
    void OnQuit(wxCommandEvent &event);
    void OnSubprocessTerminate(wxProcessEvent &event);
    static void OnAbout(wxCommandEvent &event);

    wxProcess *server;
    MainFrame *frame;
    std::string blank_notebook;
    std::string load_page;
    RecentManager recently_used;
    std::vector<std::unique_ptr<wxWindow>> frames;
};

wxDECLARE_APP(MainApp);
