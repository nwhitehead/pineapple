
#include "MainFrame.hh"

#include <algorithm>
#include <functional>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>

#include <wx/bitmap.h>
#include <wx/filedlg.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/process.h>
#include <wx/sizer.h>
#include <wx/stdpaths.h>
#include <wx/stream.h>
#include <wx/toolbar.h>
#include <wx/txtstrm.h>
#include <wx/url.h>
#include <wx/utils.h>
#include <wx/webview.h>

#include "callback.hh"
#include "config.hh"
#include "gui_util.hh"
#include "util.hh"
#include "MainApp.hh"


/**
 * MainFrame constructor and associated helpers
 */

MainFrame::MainFrame(std::string url0, std::string filename,
    const wxString &title, const wxPoint &pos, const wxSize &size,
    bool indirect_load)
        : wxFrame(nullptr, wxID_ANY, title, pos, size),
          url(url0), local_filename(filename), jupyter_ready(false)
{
    SetupMenu();
    SetupToolbar();
    SetupWebView();
    SetupLayout(pos, size);
    SetupBindings();
    LoadDocument(indirect_load);
    Show();
}

void MainFrame::SetupMenu()
{
    menubar = new wxMenuBar();
    wxMenu *menu_file = new wxMenu();
    menu_file->Append(wxID_NEW, "New\tCtrl-N");
    menu_file->AppendSeparator();
    menu_file->Append(wxID_OPEN, "Open\tCtrl-O");
    menu_file->AppendSeparator();
    menu_file->Append(wxID_SAVE, "Save\tCtrl-S");
    menu_file->Append(wxID_SAVEAS, "Save As\tShift-Ctrl-S");
    menu_file->AppendSeparator();

    {
        wxMenu *menu_export = new wxMenu();
        menu_export->Append(wxID_EXPORT_PYTHON, "Python");
        menu_export->Append(wxID_EXPORT_MARKDOWN, "Markdown");
        menu_file->AppendSubMenu(menu_export, "Export as");
    }

    menu_file->Append(wxID_TRUST, "Trust notebook");
    menu_file->AppendSeparator();
    menu_file->Append(wxID_PROPERTIES, "Properties...");
    menu_file->AppendSeparator();
    menu_file->Append(wxID_PRINT, "Print...");
    menu_file->AppendSeparator();
    menu_file->Append(wxID_CLOSE, "Close\tCtrl-W");
    // Close All only makes sense on mac
#if defined(__APPLE__)
    menu_file->Append(wxID_CLOSE_ALL, "Close All\tShift-Ctrl-W");
#endif
    menu_file->AppendSeparator();
    menu_file->Append(wxID_EXIT, "Quit\tCtrl-Q");
    menubar->Append(menu_file, "File");

    wxMenu *menu_edit = new wxMenu();
    menu_edit->Append(wxID_CUT, "Cut cell\tCtrl-X");
    menu_edit->Append(wxID_COPY, "Copy cell\tCtrl-C");
    menu_edit->Append(wxID_PASTE, "Paste cell below\tCtrl-V");
    menu_edit->AppendSeparator();
    menu_edit->Append(wxID_INSERT, "Insert cell below\tCtrl-=");
    menu_edit->AppendSeparator();
    menu_edit->Append(wxID_DELETE, "Delete cell\tCtrl-Shift-D");
    menu_edit->Append(wxID_UNDELETE, "Undelete cell\tCtrl-Shift-U");
    menu_edit->AppendSeparator();
    menu_edit->Append(wxID_SPLIT, "Split cell\tCtrl--");
    menu_edit->Append(wxID_MERGE, "Merge cell below\tCtrl-Shift--");
    menu_edit->AppendSeparator();
    menu_edit->Append(wxID_MOVE_UP, "Move cell up\tCtrl-Shift-O");
    menu_edit->Append(wxID_MOVE_DOWN, "Move cell down\tCtrl-Shift-L");
    menubar->Append(menu_edit, "Edit");

    wxMenu *menu_cell = new wxMenu();
    menu_cell->Append(wxID_RUN, "Run\tCtrl-Enter");
    menu_cell->AppendSeparator();
    menu_cell->Append(wxID_RUN_ALL, "Run all\tCtrl-Shift-Enter");
    menu_cell->Append(wxID_RUN_ALL_ABOVE, "Run all above");
    menu_cell->Append(wxID_RUN_ALL_BELOW, "Run all below");
    menu_cell->AppendSeparator();
    {
        wxMenu *menu_type = new wxMenu();
        menu_type->Append(wxID_CELL_CODE, "Code\tCtrl-Y");
        menu_type->Append(wxID_CELL_MARKDOWN, "Markdown\tCtrl-M");
        menu_type->Append(wxID_CELL_RAW, "Raw\tCtrl-R");
        menu_cell->AppendSubMenu(menu_type, "Cell type");
    }
    menubar->Append(menu_cell, "Cell");

    wxMenu *menu_kernel = new wxMenu();
    menu_kernel->Append(wxID_KERNEL_INTERRUPT, "Interrupt\tCtrl-I");
    menu_kernel->AppendSeparator();
    menu_kernel->Append(wxID_KERNEL_RESTART, "Restart\tCtrl-0");
    menu_kernel->AppendSeparator();
    menu_kernel->Append(wxID_KERNEL_RECONNECT, "Reconnect");
    menubar->Append(menu_kernel, "Kernel");

    wxMenu *menu_help = new wxMenu();
    menu_help->Append(wxID_HELP_KEYBOARD, "Keyboard shortcuts");
    menu_help->AppendSeparator();
    menu_help->Append(wxID_HELP_NOTEBOOK, "Notebook");
    menu_help->Append(wxID_HELP_MARKDOWN, "Markdown");
    menu_help->AppendSeparator();
    menu_help->Append(wxID_ABOUT, "About");
    menu_help->AppendSeparator();
    menubar->Append(menu_help, "Help");
    
    SetMenuBar(menubar);
}

void MainFrame::SetupToolbar()
{
    toolbar = CreateToolBar(config::toolbar_style);

    toolbar->AddTool(wxID_SAVE, "Save", toolbar_icon("Save-50.png"), "Save");

    toolbar->AddSeparator();

    toolbar->AddTool(wxID_INSERT, "Insert", toolbar_icon("Plus-50.png"), "Insert below");
    toolbar->AddTool(wxID_DELETE, "Delete", toolbar_icon("Delete-50.png"), "Delete cell");

    toolbar->AddSeparator();

    toolbar->AddTool(wxID_CUT, "Cut", toolbar_icon("Cut-50.png"), "Cut cell");
    toolbar->AddTool(wxID_COPY, "Copy", toolbar_icon("Copy-50.png"), "Copy cell");
    toolbar->AddTool(wxID_PASTE, "Paste", toolbar_icon("Paste-50.png"), "Paste cell");

    toolbar->AddSeparator();

    toolbar->AddTool(wxID_MOVE_UP, "Move up", toolbar_icon("Up-50.png"), "Move cell up");
    toolbar->AddTool(wxID_MOVE_DOWN, "Move down", toolbar_icon("Down-50.png"), "Move cell down");

    toolbar->AddSeparator();

    toolbar->AddTool(wxID_RUN_NEXT, "Run", toolbar_icon("Play-50.png"), "Run cell");
    toolbar->AddTool(wxID_RUN_ALL, "Run all", toolbar_icon("FastForward-50.png"), "Run all cells");
    toolbar->AddTool(wxID_KERNEL_INTERRUPT, "Stop", toolbar_icon("Stop-50.png"), "Interrupt kernel");
    toolbar->AddTool(wxID_KERNEL_RESTART, "Restart", toolbar_icon("Synchronize-50.png"), "Restart kernel");

    toolbar->AddSeparator();

    toolbar->AddTool(wxID_CELL_CODE, "Code", toolbar_icon("Edit-50.png"), "Cell type code");
    toolbar->AddTool(wxID_CELL_MARKDOWN, "Markdown", toolbar_icon("Pen-50.png"), "Cell type markdown");
    toolbar->AddTool(wxID_CELL_RAW, "Raw", toolbar_icon("Fantasy-50.png"), "Cell type raw");

    toolbar->AddSeparator();

    toolbar->AddTool(wxID_KERNEL_BUSY, "Busy",
        toolbar_icon("Led-Yellow-On-1632.png"),
        toolbar_icon("Led-Yellow-Off-1632.png"),
        wxITEM_NORMAL, "Kernel busy");
    toolbar->EnableTool(wxID_KERNEL_BUSY, false);

    toolbar->Realize();
}

void MainFrame::SetupWebView()
{
    webview = wxWebView::New(this, wxID_ANY);
    // Turn off right-click as much as possible
    webview->EnableContextMenu(false);
//    webview->SetZoom(wxWEBVIEW_ZOOM_LARGE);
}

void MainFrame::SetupLayout(const wxPoint &/* pos */, const wxSize &size)
{
    wxBoxSizer* frame_sizer = new wxBoxSizer(wxVERTICAL);
    frame_sizer->Add(webview, 1, wxEXPAND, 10);
    webview->Show();
    SetSizerAndFit(frame_sizer);
    SetSize(wxDefaultCoord, wxDefaultCoord, size.GetWidth(), size.GetHeight());
}

void MainFrame::SetupBindings()
{
    // Inline helpers to save typing
    // You give it (wxID_DELETE, "delete_cell") and it calls Bind
    // with the right lambda.
    auto bind_jupyter_click_cell = [this](int id, std::string cell) -> void {

        Bind(wxEVT_COMMAND_MENU_SELECTED,
            [this, cell](wxCommandEvent &/* event */)->void {
                jupyter_click_cell(webview, cell);
            }, id);

    };

    auto bind_goto_url = [this](int id, std::string url) -> void {

        Bind(wxEVT_COMMAND_MENU_SELECTED,
            [this, url](wxCommandEvent &/* event */)->void {
                wxLaunchDefaultBrowser(url);
            }, id);

    };

    auto bind_export = [this](int id, std::string format) -> void {

        Bind(wxEVT_COMMAND_MENU_SELECTED,
            [this, format](wxCommandEvent &/* event */)->void {
                Export(format);
            }, id);

    };

    /// Bind simple menu items (simple Jupyter functions on the cell)
    bind_jupyter_click_cell(wxID_TRUST, "trust_notebook");
    bind_jupyter_click_cell(wxID_CUT, "cut_cell");
    bind_jupyter_click_cell(wxID_COPY, "copy_cell");
    bind_jupyter_click_cell(wxID_PASTE, "paste_cell_below");
    bind_jupyter_click_cell(wxID_INSERT, "insert_cell_below");
    bind_jupyter_click_cell(wxID_DELETE, "delete_cell");
    bind_jupyter_click_cell(wxID_UNDELETE, "undelete_cell");
    bind_jupyter_click_cell(wxID_SPLIT, "split_cell");
    bind_jupyter_click_cell(wxID_MERGE, "merge_cell_below");
    bind_jupyter_click_cell(wxID_MOVE_UP, "move_cell_up");
    bind_jupyter_click_cell(wxID_MOVE_DOWN, "move_cell_down");
    bind_jupyter_click_cell(wxID_RUN, "run_cell");
    bind_jupyter_click_cell(wxID_RUN_NEXT, "run_cell_select_below");
    bind_jupyter_click_cell(wxID_RUN_ALL, "run_all_cells");
    bind_jupyter_click_cell(wxID_RUN_ALL_ABOVE, "run_all_cells_above");
    bind_jupyter_click_cell(wxID_RUN_ALL_BELOW, "run_all_cells_below");
    bind_jupyter_click_cell(wxID_CELL_CODE, "to_code");
    bind_jupyter_click_cell(wxID_CELL_MARKDOWN, "to_markdown");
    bind_jupyter_click_cell(wxID_CELL_RAW, "to_raw");
    bind_jupyter_click_cell(wxID_KERNEL_INTERRUPT, "int_kernel");
    bind_jupyter_click_cell(wxID_KERNEL_RESTART, "restart_kernel");
    bind_jupyter_click_cell(wxID_KERNEL_RECONNECT, "reconnect_kernel");
    bind_jupyter_click_cell(wxID_HELP_KEYBOARD, "keyboard_shortcuts");

    /// Bind url opens in default browser
    bind_goto_url(wxID_HELP_NOTEBOOK, "http://nbviewer.ipython.org/github/ipython/ipython/blob/3.x/examples/Notebook/Index.ipynb");
    bind_goto_url(wxID_HELP_MARKDOWN, "https://help.github.com/articles/markdown-basics/");

    /// Bind custom menu items
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnNew, wxID_NEW);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnOpen, wxID_OPEN);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnSave, this, wxID_SAVE);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnSaveAs, this, wxID_SAVEAS);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnMenuClose, this, wxID_CLOSE);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnMenuCloseAll, this, wxID_CLOSE_ALL);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainApp::OnAbout, wxID_ABOUT);
    MainApp *theApp = &wxGetApp();
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainApp::OnQuit, theApp, wxID_EXIT);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnProperties, this, wxID_PROPERTIES);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnPrint, this, wxID_PRINT);

    bind_export(wxID_EXPORT_PYTHON, "python");
    bind_export(wxID_EXPORT_MARKDOWN, "markdown");

    /// Bind other events
    Bind(wxEVT_WEBVIEW_TITLE_CHANGED, &MainFrame::OnTitleChanged, this, wxID_ANY);
    Bind(wxEVT_WEBVIEW_LOADED, &MainFrame::OnPageLoad, this, wxID_ANY);
    Bind(wxEVT_WEBVIEW_NEWWINDOW, &MainFrame::OnNewWindow, this, wxID_ANY);
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this, wxID_ANY);

    /// Setup permanent handler for kernel busy/idle updates
    handler.register_callback(config::token_kernel_busy, AsyncResult::Success,
        [this](Callback::argument x) {
            if (x == std::string("true")) {
                this->toolbar->EnableTool(wxID_KERNEL_BUSY, true);
            }
            if (x == std::string("false")) {
                this->toolbar->EnableTool(wxID_KERNEL_BUSY, false);
            }
        },
        CallbackType::Infinite);
}

void MainFrame::LoadDocument(bool indirect_load)
{
    if (indirect_load) {
        if (wxGetApp().load_page.size() == 0) {
            // Read loading page
            wxGetApp().load_page = read_all_file(config::loading_html_filename);
        }
        // Do template replacement for url
        std::string contents{wxGetApp().load_page};
        replace_one(contents, "{{url}}", url);
        webview->SetPage(wxString(contents), "");
    } else {
        webview->LoadURL(url);
    }
}


/**
 * Alternate ways to create MainFrames
 */

void MainFrame::Spawn(std::string url, std::string filename, bool indirect_load)
{
    wxGetApp().frames.push_back(
        new MainFrame(url, filename, url,
            wxPoint(wxDefaultCoord, wxDefaultCoord),
            wxSize(config::initial_width, config::initial_height),
            indirect_load)
    );
}

void MainFrame::CreateNew(bool indirect_load)
{
    wxFileName fullname;
    if (FindNewFileName(fullname)) {
        std::cout << "FILENAME " << fullname.GetFullPath() << std::endl;
        // FIXME: drive must be the same as we mounted for windows!!!
        std::ofstream out(fullname.GetFullPath());
        out << wxGetApp().blank_notebook << std::endl;
        // Get path in UNIX so it is a URI
        std::string uri(fullname.GetFullPath(wxPATH_UNIX));
        std::string filename(fullname.GetFullPath());
        // Remember it as recently used
        wxGetApp().recently_used.Add(filename);
        // Open new window for it
        Spawn(url_from_filename(uri), filename, indirect_load);
        return;
    }

    std::stringstream ss;
    ss << "Could not create new untitled notebook";
    ss << "Last attempt was to create " << std::string(fullname.GetFullPath()) << std::endl;
    wxMessageBox(ss.str(), "ERROR", wxOK | wxICON_ERROR);
}

bool MainFrame::FindNewFileName(wxFileName &fullname)
{
    wxString datadir = wxStandardPaths::Get().GetAppDocumentsDir();
    int n = 1;
    do {
        std::stringstream ss;
        if (n > 1) {
            ss << config::untitled_prefix << n << config::untitled_suffix;
        } else {
            ss << config::untitled_prefix << config::untitled_suffix;
        }
        fullname = wxFileName(datadir, ss.str());
        std::cout << "TRYING " << fullname.GetFullPath() << std::endl;
        if (!fullname.IsOk()) break;
        if (fullname.IsOk() && !fullname.FileExists()) break;
        if (n > config::max_num_untitled) break;
        n++;
    } while (1);
    if (fullname.IsOk() && !fullname.FileExists()) {
        return true;
    }
    return false;
} 


/**
 * JavaScript integration
 */

/// Evaluate JavaScript expression, do not setup callback for result
void MainFrame::eval_js(std::string expression)
{
    webview->RunScript(expression);
}

/// Evaluate JavaScript expression,  setup callback to get answer (JSON text)
void MainFrame::eval_js(std::string expression, Callback::t continuation)
{
    CallbackHandler::token id = handler.fresh_id();
    handler.register_callback(id, AsyncResult::Success, continuation);
    std::stringstream ss;
    ss << "try { document.title=\"" << config::protocol_prefix << id
        << "|\"+JSON.stringify(eval(\"" << expression << "\")); } "
        << "catch(e) { document.title = \"" << config::protocol_prefix << id
        << "|exception\"; }";
    std::cout << "Trying to eval " << ss.str() << std::endl;
    webview->RunScript(ss.str());
}

/// Evaluate JavaScript expression
/// Ignore local expression value
/// Callback gets called when Jupyter event is triggered
void MainFrame::eval_js_event(std::string expression, std::string evtname, Callback::t continuation)
{
    CallbackHandler::token id = handler.fresh_id();
    handler.register_callback(id, AsyncResult::Success, continuation);
    std::stringstream ss;

    // Register callback
    ss << "{";
    ss << "  var success = function(evt) {";
    ss << "    var old = document.title;";
    ss << "    document.title = \"" << config::protocol_prefix << id << "|1\";";
    ss << "    document.title = old;";
    ss << "  };";
    ss << "  var failure = function(evt) {";
    ss << "    var old = document.title;";
    ss << "    document.title = \"" << config::protocol_prefix << id << "|0\";";
    ss << "    document.title = old;";
    ss << "  };";
    ss << "  require('base/js/events').on(\"" << evtname << "\"";
    ss << "  , success);";
    ss << "}";
    // Evaluate expression
    ss << expression;
    std::cout << "Trying to eval " << ss.str() << std::endl;
    webview->RunScript(ss.str());
}

std::string MainFrame::jupyter_click_code(std::string id)
{
    return std::string("Jupyter.menubar.element.find('#" + id + "').click(); true");
}

void MainFrame::Save()
{
    eval_js(jupyter_click_code("save_checkpoint"));
}

void MainFrame::Save(Callback::t continuation)
{
    // call continuation when saved
    eval_js_event(jupyter_click_code("save_checkpoint"), "notebook_saved.Notebook", continuation);
}


/**
 * MainFrame handlers for wx events
 */

void MainFrame::OnNew(wxCommandEvent &/* event */)
{
    CreateNew(false);
}

void MainFrame::OnOpen(wxCommandEvent &/* event */)
{
    wxFileDialog dialog(nullptr, "Open Notebook file", "", "",
        "Notebook files (*.ipynb)|*.ipynb", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    
    if (dialog.ShowModal() == wxID_CANCEL) return;

    std::string filename = std::string(dialog.GetPath());
    std::cout << "OPEN " << filename << std::endl;
    wxGetApp().recently_used.Add(filename);
    Spawn(url_from_filename(filename), filename, false);
}

void MainFrame::OnSave(wxCommandEvent &/* event */)
{
    Save();
}

void MainFrame::OnSaveAs(wxCommandEvent &/* event */)
{
    wxFileDialog dialog(this, "Save Notebook file", "", "",
        "Notebook files (*.ipynb)|*.ipynb", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_CANCEL) return;

    std::string new_filename = std::string(dialog.GetPath());
    std::cout << "SAVE AS " << local_filename << " -> " << new_filename << std::endl;

    // Copy the old file to the new filename
    std::ifstream ifs(local_filename);
    std::string contents = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    std::ofstream ofs(new_filename);
    ofs << contents;
    local_filename = new_filename;
    url = url_from_filename(new_filename);

    // Remember this as a recent file
    wxGetApp().recently_used.Add(local_filename);

    webview->LoadURL(url);
}

void MainFrame::OnClose(wxCloseEvent &event)
{
    // Skip saving if we haven't loaded Jupyter yet
    if (event.CanVeto() && jupyter_ready) {
        // Always save to disk, there is no choice
        // Once save is finished, close for real
        Save([this](Callback::argument /* x */) {
                Close(true); // unvetoable
                // Make sure this event gets through without blocking
                // (Stops on Ubuntu)
                wxGetApp().WakeUpIdle();
            }
        );
        // Don't close yet, wait for final save to finish
        event.Veto();
        return;
    }
    Destroy();
    // Remove ourselves from app list of main frames
    wxGetApp().frames.erase(
        std::remove(wxGetApp().frames.begin(), wxGetApp().frames.end(), this),
        wxGetApp().frames.end());
    // If we were last one the app was waiting on to exit, exit
    if (wxGetApp().waiting_to_quit && wxGetApp().frames.size() == 0) {
        wxGetApp().ExitMainLoop();
    }
}

void MainFrame::OnPageLoad(wxWebViewEvent &/* event */)
{
    std::cout << "PAGE LOAD" << std::endl;
    eval_js("if (Jupyter) { 1 } else { 0 }", [this](std::string x) {
        if (x == std::string("1")) {
            jupyter_ready = true;
        }
        std::cout << "PAGE LOAD - Jupyter = " << jupyter_ready << std::endl;
    });
}

void MainFrame::OnNewWindow(wxWebViewEvent &event)
{
    std::cout << "NEW WINDOWS - " << event.GetURL() << std::endl;
    wxLaunchDefaultBrowser(event.GetURL());
}

void MainFrame::OnTitleChanged(wxWebViewEvent &event)
{
    std::string title = event.GetString().ToStdString();
    std::cout << "TITLE CHANGED - " << title << std::endl;
    // Check if starts with special prefix
    std::string prefix = config::protocol_prefix;
    if (std::equal(prefix.begin(), prefix.end(), title.begin())) {
        // Prefix present
        std::string txt = title.substr(prefix.size());
        std::vector<std::string> items(split(txt, '|'));
        if (items.size() < 2) {
            items.push_back("");
        }
        CallbackHandler::token id;
        try {
            id = std::stoi(items[0]);
        } catch (...) {
            std::cerr << "SPECIAL TITLE MALFORMED - " << txt << std::endl;
            return;
        }
        handler.call(id, AsyncResult::Success, items[1]);
        return;
    }
    // Otherwise actually change the title
    SetLabel(config::title_prefix + title);
}

void MainFrame::OnProperties(wxCommandEvent &/* event */)
{
    eval_js(std::string("2+2"), [](Callback::argument x) {
        std::cout << "Result of 2+2 is " << x << std::endl;
    });

    std::stringstream ss;
    ss << "Name: " << std::endl;
    wxMessageBox(ss.str(), "Properties", wxOK | wxICON_INFORMATION);
}

void MainFrame::OnMenuClose(wxCommandEvent &/* event */)
{
    Close(false);
}

void MainFrame::OnMenuCloseAll(wxCommandEvent &/* event */)
{
    wxGetApp().CloseAll();
}

void MainFrame::OnPrint(wxCommandEvent &/* event */)
{
    webview->Print();
}

void MainFrame::Export(std::string format)
{
    // Export data first, then prompt user
    std::string url(export_url_from_filename(local_filename, format));
    std::cout << "EXPORT " << format << " " << local_filename << " " << url << std::endl;
    wxHTTP http;
    std::string contents;
    std::string content_type;
    http.Connect(config::hostname, config::portnumber);
    if (http.GetError() == wxPROTO_NOERR) {
        wxInputStream *in = http.GetInputStream(url);
        content_type = http.GetHeader("content-type");
        if (in && in->IsOk()) {
            while(!in->Eof()) {
                wxByte buf[1024];
                in->Read(&buf, 1024);
                contents.append(buf, buf+in->LastRead());
            }
        } else {
            wxMessageBox("There was a problem generating the file.1");
            return;
        }
        delete in;
    } else {
        wxMessageBox("There was a problem generating the file.2");
        return;
    }

    std::string msg, desc;
    if (content_type == std::string("text/x-python; charset=utf-8")) {
        msg = "Save Python file";
        desc = "Python files (*.py)|*.py";
    }
    else if (content_type == std::string("text/markdown; charset=utf-8")) {
        msg = "Save Markdown file";
        desc = "Markdown files (*.md)|*.md";
    } else if (content_type == std::string("application/zip")) {
        msg = "Save Markdown ZIP archive";
        desc = "ZIP files (*.zip)|*.zip";
    }
    wxFileDialog dialog(this, msg, "", "", desc,
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_CANCEL) return;

    std::string new_filename = std::string(dialog.GetPath());

    write_file(new_filename, contents);
    std::stringstream ss;
    ss << "Wrote " << contents.size() << " bytes." << std::endl;
    wxMessageBox(ss.str());
}
