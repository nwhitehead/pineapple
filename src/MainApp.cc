// Copyright 2015 Nathan Whitehead
// Distributed under the terms of the GNU General Public License

// This file is part of Pineapple.
//
// Pineapple is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Pineapple is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Pineapple.  If not, see <http://www.gnu.org/licenses/>.

#include "MainApp.hh"

#include <cstdint>
#include <chrono>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <string>

#include <wx/app.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/process.h>
#include <wx/stdpaths.h>
#include <wx/webview.h>

#include "config.hh"
#include "gui_util.hh"
#include "util.hh"
#include "ExamplesFrame.hh"
#include "MainFrame.hh"
#include "PreferencesManager.hh"

wxIMPLEMENT_APP(MainApp);

static void signal_handler(int /* signum */)
{
    // Do nothing except exit cleanly on Ctrl-C signal
    exit(wxGetApp().OnExit());
}

bool MainApp::OnInit()
{
    examples_frame = nullptr;
    waiting_to_quit = false;

    // Load preferences
    preferences.SyncRead();

    // Load blank notebook so we can do "New"
    blank_notebook = read_all_file(resource_filename(
        config::blank_notebook_filename));

    // Initialize image handlers so we can load toolbar bitmaps
    wxInitAllImageHandlers();

    // On Mac, allow no frames
#if defined(__APPLE__)
    SetExitOnFrameDelete(false);
    wxMenuBar *menubar = new wxMenuBar();
    wxMenu *menu = new wxMenu();
    menu->Append(wxID_ABOUT, "About");
    menu->Append(wxID_NEW, "New\tCtrl-N");
    menu->Append(wxID_OPEN, "Open\tCtrl-O");
    menu->AppendSeparator();
    menu->Append(MainFrame::wxID_OPEN_EXAMPLE, "Examples...");
    menu->AppendSeparator();
    menu->Append(wxID_EXIT, "Quit\tCtrl-Q");
    menubar->Append(menu, "File");

    wxMenuBar::MacSetCommonMenuBar(menubar);

    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnNew, wxID_NEW);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnOpen, wxID_OPEN);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainApp::OnAbout, wxID_ABOUT);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainApp::OnQuit, this, wxID_EXIT);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnOpenExample, MainFrame::wxID_OPEN_EXAMPLE);

#endif

    /// Setup server connection info
    // Get random offset for port
    int64_t ms = 
        std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now().time_since_epoch()).count();

    hostname = "localhost";
    port_number = static_cast<uint16_t>(config::min_port +
        (static_cast<int>(ms) % (config::max_port - config::min_port)));
    host_port = std::string(hostname) + std::string(":") + std::to_string(port_number);
    protocol = "http";
    base_url = protocol + std::string("://") + host_port;

    int loaded = 0;
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            wxFileName fname(arg);
            fname.MakeAbsolute();
            if (fname.FileExists()) {
                std::string filename(fname.GetFullPath());
                wxLogDebug("MainApp::OnInit Loading file from command line [%s]", filename);
                if (MainFrame::SafeSpawn(filename, true)) {
                    loaded++;
                }
            } else {
                wxLogError("Could not open file: %s", arg);
            }
        }
        if (loaded == 0) return false;
    } else {
        // See if we can load most recently used file
        std::string filename(recently_used.Get());

        if (filename.empty()) {
            ExamplesFrame::SpawnExample(std::string("Welcome"), resource_filename("Welcome.ipynb"));
        } else {
            // Open most recently used
            MainFrame::SafeSpawn(filename, true);
        }
    }

    server = nullptr;
    Bind(wxEVT_END_PROCESS, &MainApp::OnSubprocessTerminate, this, wxID_ANY);

    /// Unset environment variables that might affect Python
    wxUnsetEnv("PYTHONSTARTUP");
    wxUnsetEnv("PYTHONPATH");
    wxUnsetEnv("PYTHONHOME");

    /// Setup arguments to start server
    std::string python_path(python_fullpath());
    std::string script_path(server_fullpath());
    std::string portnum_string(std::to_string(port_number));
    wxLogDebug("MainApp::OnInit Python [%s] Script [%s]", python_path, script_path);
    // Need to construct "char **argv" for wxExecute, kind of convoluted in C++
    // Passing in a single string "path/python path/server portnum" fails if paths have spaces
    std::vector<char> raw_python_path(python_path.begin(), python_path.end());
    raw_python_path.push_back('\0');
    std::vector<char> raw_script_path(script_path.begin(), script_path.end());
    raw_script_path.push_back('\0');
    std::vector<char> raw_portnum(portnum_string.begin(), portnum_string.end());
    raw_portnum.push_back('\0');
    char *argv[] = { &raw_python_path[0], &raw_script_path[0], &raw_portnum[0], nullptr };

    /// Start server
    server = new wxProcess(frame);
    long res;
    if ((res = wxExecute(argv,
        wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE | wxEXEC_MAKE_GROUP_LEADER,
        server))) {
        // Nonzero result means we started the subprocess successfully
        wxLogDebug("MainApp::OnInit Started subprocess");
    } else {
        wxLogError("Internal Error", "Could not start python, exiting");
        wxLogDebug("MainApp::OnInit Could not start subprocess");
        return false;
    }

    // Set handler to kill process if we die
    signal(SIGINT, signal_handler);

    return true;
}

void MainApp::MacOpenFile(const wxString &filename)
{
    std::string fname(filename);
    wxGetApp().recently_used.Add(fname);
    MainFrame::SafeSpawn(fname, true);
}

int MainApp::OnExit()
{
    wxLogDebug("MainApp::OnExit");
    if (server) {
        wxLogDebug("MainApp::OnExit Killing server process");
        server->Kill(static_cast<int>(server->GetPid()), wxSIGTERM, wxKILL_CHILDREN);
    }
    return(0);
}

void MainApp::CloseAll()
{
    for (auto it : frames) {
        if (it) {
            it->Close();
        }
    }
}

void MainApp::OnSubprocessTerminate(wxProcessEvent &/* event */)
{
    wxLogDebug("MainApp::OnSubprocessTerminate");
}

void MainApp::OnAbout(wxCommandEvent &/* event */)
{
    std::stringstream ss;
    ss << config::version_full << "\n";
    ss << "(" << config::version_date << ")\n\n";
    ss << "Copyright (c) 2015 Nathan Whitehead\n\n";
    ss << wxGetLibraryVersionInfo().ToString() << "\n";
    wxMessageBox(ss.str(), "About", wxOK | wxICON_INFORMATION);
}

void MainApp::OnQuit(wxCommandEvent &/* event */)
{
    // Close all frames asynchronously
    // As they close, they remove themselves from list
    // and notice waiting_to_quit
    waiting_to_quit = true;
    if (frames.size() > 0) {
        CloseAll();
    } else {
        ExitMainLoop();
    }
}

std::string MainApp::UrlFromFilename(std::string filename)
{
    std::string uri(urlencode(filename));
    return base_url + std::string(config::path_url) + uri;
}

std::string MainApp::ExportUrlFromFilename(std::string filename, std::string format)
{
    std::string uri(filename);
    return std::string(config::nbconvert_url) + std::string("/") + format + uri;
}
