#pragma once

#include <signal.h>
#include <string>

#include <wx/filename.h>
#include <wx/frame.h>
#include <wx/menu.h>

class ExamplesFrame: public wxFrame
{
public:
    ExamplesFrame(wxWindow *parent);
    void SetupTree();
};
