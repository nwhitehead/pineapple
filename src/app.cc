#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#if !wxUSE_WEBVIEW
#error "WebView not enabled"
#endif

#include <wx/menu.h>
#include <wx/process.h>
#include <wx/stream.h>
#include <wx/txtstrm.h>
#include <wx/utils.h>
#include <wx/webview.h>

#include "config.h"

namespace config {

/// Environment variable to pass server path
constexpr char server_script_env[] = "PINEAPPLE_SERVER";
/// Default server script if none given
constexpr char server_script_default[] = "venv/bin/python scripts/eridani-main serve";
/// Default size of main window on startup
constexpr int initial_width = 900;
constexpr int initial_height = 700;
/// Initial url to open
constexpr char start_url[] = "http://localhost:8888/tree/demo/TestNotebook.ipynb";
/// Title prefix
constexpr char title[] = "Pineapple";

} /// namespace config

class MainApp: public wxApp
{
public:
    virtual bool OnInit();
};

class MainFrame: public wxFrame
{
public:
    MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size);

    wxProcess *server;
    wxWebView *webview;
    wxString url;
    wxMenuBar *menubar;
    wxMenu *menu_file;
    wxMenu *menu_help;

    void OnError(wxWebViewEvent &event);
    void OnHello(wxCommandEvent &event);
    void OnClose(wxCloseEvent &event);
    void OnQuit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnSubprocessTerminate(wxProcessEvent &event);
    void OnTitleChanged(wxWebViewEvent &event);
private:
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_CLOSE(MainFrame::OnClose)
    EVT_END_PROCESS(wxID_ANY, MainFrame::OnSubprocessTerminate)
    EVT_MENU(wxID_EXIT, MainFrame::OnQuit)
    EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
    EVT_WEBVIEW_ERROR(wxID_ANY, MainFrame::OnError)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MainApp);

bool MainApp::OnInit()
{
    MainFrame *frame = new MainFrame(config::title, wxPoint(50, 50), wxSize(400, 400));
    frame->Show();

    wxString server_script;
    frame->server = nullptr;
    if (!wxGetEnv(config::server_script_env, &server_script)) {
        server_script = config::server_script_default;
    }
    frame->server = new wxProcess(frame);
    wxExecute(server_script,
        wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE | wxEXEC_MAKE_GROUP_LEADER,
        frame->server);

    return true;
}

MainFrame::MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
{
    menubar = new wxMenuBar();
    menu_file = new wxMenu();
    menu_file->Append(wxID_EXIT, wxT("&Quit"));
    menubar->Append(menu_file, wxT("&File"));
    menu_help = new wxMenu();
    menu_help->Append(wxID_ABOUT, wxT("&About"));
    menubar->Append(menu_help, wxT("&Help"));
    SetMenuBar(menubar);

    // Create sizer for panel.
    wxBoxSizer* frame_sizer = new wxBoxSizer(wxVERTICAL);

    url = config::start_url;
    webview = wxWebView::New(this, wxID_ANY);
    frame_sizer->Add(webview, 1, wxEXPAND, 10);

    webview->LoadURL(url);
    webview->Show();

    SetSizerAndFit(frame_sizer);
    SetSize(wxDefaultCoord, wxDefaultCoord, config::initial_width, config::initial_height);
}

void MainFrame::OnError(wxWebViewEvent &event)
{
    std::cout << "ERROR" << std::endl;
//    webview->Hide();
    webview->LoadURL(url);
}

void MainFrame::OnQuit(wxCommandEvent &event)
{
    std::cout << "QUIT" << std::endl;
    Close(true);
}

void MainFrame::OnAbout(wxCommandEvent &event)
{
    std::stringstream ss;
    ss << config::version_full << "\n\n" << wxGetLibraryVersionInfo().ToString() << std::endl;

    wxMessageBox(ss.str(), "About", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnHello(wxCommandEvent &event)
{
    wxLogMessage("Hello world");
}

void MainFrame::OnSubprocessTerminate(wxProcessEvent &event)
{
    wxLogMessage("TERMINATE");
    event.Skip();
}

void MainFrame::OnClose(wxCloseEvent &event)
{
    std::cout << "CLOSE" << std::endl;
    if (server) {
        server->Kill(server->GetPid(), wxSIGTERM, wxKILL_CHILDREN);
        delete server;
    }
    if (webview) {
        webview->Destroy();
    }
    Destroy();
}
