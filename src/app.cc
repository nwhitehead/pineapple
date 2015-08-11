#include <iostream>
#include <string>
#include <vector>

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#if !wxUSE_WEBVIEW
#error "WebView not enabled"
#endif

#if !wxUSE_WEBVIEW_WEBKIT && !wxUSE_WEBVIEW_IE
#error "A wxWebView backend is required by this sample"
#endif

#include <wx/process.h>
#include <wx/stream.h>
#include <wx/txtstrm.h>
#include <wx/utils.h>
#include <wx/webview.h>

/// Environment variable to pass server path
constexpr char server_script_env[] = "PINEAPPLE_SERVER";

class MainApp: public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size);

    wxProcess *server;
    wxWebView *webview;

    void OnError(wxWebViewEvent &event);
    void OnHello(wxCommandEvent &event);
    void OnClose(wxCloseEvent &event);
    void OnExit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnSubprocessTerminate(wxCommandEvent &event);
private:
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_CLOSE(MyFrame::OnClose)
wxEND_EVENT_TABLE()

enum
{
    ID_Hello = 1
};

wxIMPLEMENT_APP(MainApp);

bool MainApp::OnInit()
{
    MyFrame *frame = new MyFrame("Hello world", wxPoint(50, 50), wxSize(400, 400));
    frame->Show();
    std::cout << "wxWidget versioninfo = " << wxGetLibraryVersionInfo().ToString() << std::endl;

    Connect(wxEVT_END_PROCESS, wxCommandEventHandler(MyFrame::OnSubprocessTerminate));

    wxString server_script;
    frame->server = nullptr;
    if (!wxGetEnv(server_script_env, &server_script)) {
        server_script = "venv/bin/python scripts/eridani-main serve";
    }
    frame->server = new wxProcess(frame);
    wxExecute(server_script,
        wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE | wxEXEC_MAKE_GROUP_LEADER,
        frame->server);

    std::cout << "Process execute started" << std::endl;

    return true;
}

MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
{
    std::cout << "Creating MyFrame" << std::endl;
    // Create sizer for panel.
    wxStaticBoxSizer* frame_sizer = new wxStaticBoxSizer(wxVERTICAL, this, "WebView");

    wxString url = "http://www.google.com";
    webview = wxWebView::New(this, wxID_ANY);
    frame_sizer->Add(webview, 1, wxEXPAND, 10);
//    webview = wxWebView::New(this, wxID_ANY);
    webview->LoadURL(url);
    webview->Show();
//    std::cout << "Title = " << webview->GetCurrentTitle() << std::endl;

    Connect(webview->GetId(), wxEVT_WEBVIEW_ERROR,
            wxWebViewEventHandler(MyFrame::OnError), NULL, this);

    SetSizerAndFit(frame_sizer);
    SetSize(wxDefaultCoord, wxDefaultCoord, 700, 700);
}

void MyFrame::OnError(wxWebViewEvent &event)
{
    std::cout << "ERROR" << std::endl;
}

void MyFrame::OnExit(wxCommandEvent &event)
{
    std::cout << "EXIT" << std::endl;
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent &event)
{
    wxMessageBox("This is wxWidgets",
        "About", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent &event)
{
    wxLogMessage("Hello world");
}

void MyFrame::OnSubprocessTerminate(wxCommandEvent &event)
{
    wxLogMessage("TERMINATE");
    event.Skip();
}

void MyFrame::OnClose(wxCloseEvent &event)
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
