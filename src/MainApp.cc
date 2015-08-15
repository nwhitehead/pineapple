
#include "MainApp.hh"

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>

#include <signal.h>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/app.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/menu.h>
#include <wx/process.h>
#include <wx/stdpaths.h>
#include <wx/stream.h>
#include <wx/txtstrm.h>
#include <wx/utils.h>
#include <wx/webview.h>

#include "MainFrame.hh"

wxBEGIN_EVENT_TABLE(MainApp, wxApp)
    EVT_END_PROCESS(wxID_ANY, MainApp::OnSubprocessTerminate)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MainApp);

static void signal_handler(int signum)
{
    exit(wxGetApp().OnExit());
}

bool MainApp::OnInit()
{
    // Load blank notebook so we can do "New"
    std::ifstream ifs(config::blank_notebook_filename);
    blank_notebook = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

    wxInitAllImageHandlers();
    
    frame = MainFrame::CreateNew(true);
    if (frame == nullptr) return false;

    wxString server_script;
    server = nullptr;
    if (!wxGetEnv(config::server_script_env, &server_script)) {
        server_script = config::server_script_default;
    }
    server = new wxProcess(frame);
    wxExecute(server_script,
        wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE | wxEXEC_MAKE_GROUP_LEADER,
        server);
    // Set handler to kill process if we die
    signal(SIGINT, signal_handler);

    return true;
}

int MainApp::OnExit()
{
    std::cout << "APP OnExit()" << std::endl;
    if (server) {
        std::cout << "CLOSING SERVER" << std::endl;
        server->Kill(server->GetPid(), wxSIGTERM, wxKILL_CHILDREN);
    }
    return(0);
}

void MainApp::OnSubprocessTerminate(wxProcessEvent &event)
{
    std::cout << "SUBPROCESS TERMINATED" << std::endl;
}
