
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

/**
 * Data structure for keeping track of tree and filenames
 */

using tree_item_type = std::tuple<std::string, std::string>;
using tree_items_type = std::vector<tree_item_type>;
using tree_category_type = std::pair<std::string, tree_items_type>;
using tree_type = std::vector<tree_category_type>;

tree_type tree_data {
    { "Topics", {
            std::make_tuple("Basics", "Basics.ipynb"),
//            std::make_tuple("Editing", "Editing.ipynb"),
            std::make_tuple("Markdown", "Markdown.ipynb"),
            std::make_tuple("Plotting", "Plotting.ipynb"),
            std::make_tuple("Timing", "Timing.ipynb"),
            std::make_tuple("Rich Output", "RichOutput.ipynb")
        }
    },
    { "Examples", {
            std::make_tuple("Heat diffusion", "Heat.ipynb")
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
    Show();
}

void SpawnExample(wxFrame */* parent */, wxTreeItemId id)
{
    // Copy resource .ipynb file to user area
    // (Cannot change files in digitally signed resource area)
    auto tuple = tree_map[id];
    std::string prefix(std::get<0>(tuple));
    std::string example_file(std::get<1>(tuple));
    wxLogDebug("SpawnExample Item selected [%s]", example_file);
    std::string original_filename(resource_filename(example_file));
    wxLogDebug("SpawnExample Filename [%s]", original_filename);
    wxFileName wxs_new_filename;
    if (FindNewFileName(wxs_new_filename, prefix, config::example_suffix,
                        config::example_max_num, true)) {
        std::string new_filename(wxs_new_filename.GetFullPath());
        // Output filename doesn't exist, write it
        write_file(new_filename, read_all_file(original_filename));
        wxLogDebug("SpawnExample Loading file [%s]", new_filename);
        MainFrame::Spawn(url_from_filename(new_filename), new_filename, false);
    } else {
        wxLogError("Could not create temporary example file");
    }
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
            SpawnExample(this, id);
        }
    }, tree->GetId());
    tree->ExpandAll();
    tree->Show();
    SetSize(wxDefaultCoord, wxDefaultCoord, 400, 400);
}

