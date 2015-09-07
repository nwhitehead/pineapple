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

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <wx/app.h>
#include <wx/process.h>

#include "config.hh"
#include "ExamplesFrame.hh"
#include "MainFrame.hh"
#include "PreferencesManager.hh"
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
    std::string UrlFromFilename(std::string filename);
    std::string ExportUrlFromFilename(std::string filename, std::string format);

    wxProcess *server;
    MainFrame *frame;
    ExamplesFrame *examples_frame;
    std::string blank_notebook;
    std::string load_page;
    RecentManager recently_used;
    PreferencesManager preferences;
    std::vector<wxWindow *> frames;
    bool waiting_to_quit;

    std::string hostname;
    uint16_t port_number;
    std::string host_port;
    std::string protocol;
    std::string base_url;
};

wxDECLARE_APP(MainApp);
