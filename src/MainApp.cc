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
#include "MainFrame.hh"

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

    int loaded = 0;
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            wxFileName fname(arg);
            fname.MakeAbsolute();
            if (fname.FileExists()) {
                std::string filename(fname.GetFullPath());
                wxLogDebug("MainApp::OnInit Loading file from command line [%s]", filename);
                MainFrame::Spawn(url_from_filename(filename), filename, true);
                loaded++;
            } else {
                wxLogError("Could not open file: %s", arg);
            }
        }
        if (loaded == 0) return false;
    } else {
        // See if we can load most recently used file
        std::string filename(recently_used.Get());

        if (filename.empty()) {
            // If no recently used, create fresh new file
            MainFrame::CreateNew(true);
        } else {
            // Open most recently used
            MainFrame::Spawn(url_from_filename(filename), filename, true);
        }
    }

    server = nullptr;
    Bind(wxEVT_END_PROCESS, &MainApp::OnSubprocessTerminate, this, wxID_ANY);

    std::string python_path(python_fullpath());
    std::string script_path(server_fullpath());
    wxLogDebug("MainApp::OnInit Python [%s] Script [%s]", python_path, script_path);
    // Need to construct "char **argv" for wxExecute, kind of convoluted in C++
    // Passing in a single string "path/python path/server" fails if paths have spaces
    std::vector<char> raw_python_path(python_path.begin(), python_path.end());
    raw_python_path.push_back('\0');
    std::vector<char> raw_script_path(script_path.begin(), script_path.end());
    raw_script_path.push_back('\0');
    char *argv[] = { &raw_python_path[0], &raw_script_path[0], nullptr };
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
    MainFrame::Spawn(url_from_filename(fname), fname, true);
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
    ss << config::version_full << "\n\n";
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
