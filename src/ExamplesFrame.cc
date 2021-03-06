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

#include "ExamplesFrame.hh"

#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <wx/filename.h>
#include <wx/frame.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/stdpaths.h>
#include <wx/treectrl.h>

#include "gui_util.hh"
#include "util.hh"
#include "MainFrame.hh"
#include "MainApp.hh"

/**
 * Data structure for keeping track of tree and filenames
 */

using tree_item_type = std::tuple<std::string, std::string>;
using tree_items_type = std::vector<tree_item_type>;
using tree_category_type = std::pair<std::string, tree_items_type>;
using tree_type = std::vector<tree_category_type>;

tree_type tree_data {
    { "Topics", {
            std::make_tuple("Welcome", "Welcome.ipynb"),
            std::make_tuple("Basics", "Basics.ipynb"),
            std::make_tuple("Editing", "Editing.ipynb"),
            std::make_tuple("Markdown", "Markdown.ipynb"),
            std::make_tuple("Execution", "Execution.ipynb"),
            std::make_tuple("Read Only", "ReadOnly.ipynb"),
            std::make_tuple("Plotting", "Plotting.ipynb"),
            std::make_tuple("Timing", "Timing.ipynb"),
            std::make_tuple("Rich Output", "RichOutput.ipynb"),
            std::make_tuple("Packages", "Packages.ipynb")
        }
    },
    { "Examples", {
            std::make_tuple("Video Demo", "Video.ipynb")
        }
    }
};

/// This is a map from TreeItemId to (Name, filename)
std::map<wxTreeItemId, tree_item_type> tree_map;

/**
 * ExamplesFrame constructor and associated helpers
 */

ExamplesFrame::ExamplesFrame(wxWindow */* parent */)
        : wxFrame(nullptr, wxID_ANY, "Choose example")
{
    wxLogDebug("ExamplesFrame::ExamplesFrame");

    SetupTree();
    Bind(wxEVT_CLOSE_WINDOW, &ExamplesFrame::OnClose, this, wxID_ANY);
    Show();
}

void ExamplesFrame::OnClose(wxCloseEvent &/* event */)
{
    wxLogDebug("ExamplesFrame::OnClose");
    wxGetApp().examples_frame = nullptr;
    Destroy();
}

void ExamplesFrame::SpawnExample(std::string name, std::string original_filename, bool indirect_load)
{
    // Copy resource .ipynb file to user area
    // (Cannot change files in digitally signed resource area)
    wxLogDebug("SpawnExample Filename [%s]", original_filename);
    wxFileName wxs_new_filename;
    if (FindNewFileName(wxs_new_filename, name, config::example_suffix,
                        config::example_max_num, true)) {
        std::string new_filename(wxs_new_filename.GetFullPath());
        // Output filename doesn't exist, write it
        write_file(new_filename, read_all_file(original_filename));
        wxLogDebug("SpawnExample Loading file [%s]", new_filename);
        // Remember it as recently used
        wxGetApp().recently_used.Add(new_filename);
        MainFrame::Spawn(wxGetApp().UrlFromFilename(new_filename), new_filename, indirect_load);
    } else {
        wxLogError("Could not create temporary example file");
    }
}

void ExamplesFrame::SpawnExampleId(wxTreeItemId id)
{
    auto tuple = tree_map[id];
    std::string name(std::get<0>(tuple));
    std::string example_file(std::get<1>(tuple));
    wxLogDebug("SpawnExampleId Item selected [%s]", example_file);
    std::string original_filename(resource_filename(example_file));
    SpawnExample(name, original_filename, false);
}

void ExamplesFrame::SetupTree()
{
    wxTreeCtrl *tree = new wxTreeCtrl(this, 1, wxDefaultPosition, wxDefaultSize,
        wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT);
    wxTreeItemId root = tree->AddRoot("Top");
    tree_map.clear();
    for (auto category : tree_data) {
        auto subtree = tree->AppendItem(root, category.first);
        for (auto item : category.second) {
            auto id = tree->AppendItem(subtree, std::get<0>(item));
            tree_map[id] = item;
        }
    }
    Bind(wxEVT_TREE_ITEM_ACTIVATED, [this](wxTreeEvent &event) {
        wxTreeItemId id(event.GetItem());
        if (tree_map.find(id) != tree_map.end()) {
            SpawnExampleId(id);
        }
    }, tree->GetId());
    tree->ExpandAll();
    tree->Show();
    SetSize(wxDefaultCoord, wxDefaultCoord, 400, 400);
}

