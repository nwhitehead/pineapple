
#include "MainApp.hh"

#include <fstream>
#include <iostream>
#include <signal.h>
#include <string>

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/app.h>
#include <wx/process.h>
#include <wx/webview.h>

#include "config.hh"
#include "util.hh"
#include "MainFrame.hh"

wxIMPLEMENT_APP(MainApp);

static void signal_handler(int /* signum */)
{
    exit(wxGetApp().OnExit());
}

bool MainApp::OnInit()
{
    // Load blank notebook so we can do "New"
    std::ifstream ifs(config::blank_notebook_filename);
    blank_notebook = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

    // Initialize image handlers so we can load toolbar bitmaps
    wxInitAllImageHandlers();

    std::string filename(recently_used.Get());

    if (filename.empty()) {
        // If no recently used, create fresh new file
        frame = MainFrame::CreateNew(true);
    } else {
        // Open most recently used
        frame = MainFrame::Spawn(url_from_filename(filename), filename, true);
    }
    if (frame == nullptr) return false;

    wxString server_script;
    server = nullptr;
    if (!wxGetEnv(config::server_script_env, &server_script)) {
        server_script = config::server_script_default;
    }

    Bind(wxEVT_END_PROCESS, &MainApp::OnSubprocessTerminate, this, wxID_ANY);

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
    if (server) {
        server->Kill(static_cast<int>(server->GetPid()), wxSIGTERM, wxKILL_CHILDREN);
    }
    return(0);
}

void MainApp::OnSubprocessTerminate(wxProcessEvent &/* event */)
{
    std::cout << "SUBPROCESS TERMINATED" << std::endl;
}
