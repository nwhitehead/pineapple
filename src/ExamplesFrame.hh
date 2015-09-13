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

#include <signal.h>
#include <string>

#include <wx/filename.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/treectrl.h>

class ExamplesFrame: public wxFrame
{
public:
    ExamplesFrame(wxWindow *parent);
    void SetupTree();
    void OnClose(wxCloseEvent &event);
    static void SpawnExample(std::string name, std::string original_filename, bool indirect_load=true);
    static void SpawnExampleId(wxTreeItemId id);
};
