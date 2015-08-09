#include <iostream>
#include <string>
#include <vector>

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/process.h>
#include <wx/stream.h>
#include <wx/txtstrm.h>

constexpr char server_script_location[] = "../../scripts/eridani-main serve";

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size);

    wxProcess *server;

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

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame("Hello world", wxPoint(50, 50), wxSize(400, 400));
    frame->Show();
    std::cout << "wxWidget versioninfo = " << wxGetLibraryVersionInfo().ToString() << std::endl;

    Connect(wxEVT_END_PROCESS, wxCommandEventHandler(MyFrame::OnSubprocessTerminate));

    frame->server = new wxProcess(frame);
    wxExecute(server_script_location,
        wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE | wxEXEC_MAKE_GROUP_LEADER,
        frame->server);

/*
    if (process) {
        process->Redirect();
        std::cout << "Redirected" << std::endl;
        wxString log;
        wxInputStream *msg = process->GetInputStream();
        wxTextInputStream tStream(*msg);
        while(msg && !(msg->Eof())) {
            log = tStream.ReadLine();
            std::cout << "REDIRECT: " << log << std::endl;
        }
    }
*/
    return true;
}

MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size)
    : wxFrame(nullptr, wxID_ANY, title, pos, size)
{

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
    }
    Destroy();
}
