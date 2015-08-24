
#include "ExamplesFrame.hh"

#include <map>
#include <string>
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

using tree_item_type = std::pair<std::string, std::string>;
using tree_items_type = std::vector<tree_item_type>;
using tree_category_type = std::pair<std::string, tree_items_type>;
using tree_type = std::vector<tree_category_type>;

tree_type tree_data = {
    { std::string("Topics"), {
            { "Basics", "basics.ipynb" },
            { "Editing", "editing.ipynb" }
        }
    },
    { std::string("Examples"), {
            { "Heat diffusion", "heat.ipynb" }
        }
    }
};

std::map<wxTreeItemId, std::string> tree_map;

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

void SpawnExample(wxFrame *parent, wxTreeItemId id)
{
    // Copy resource .ipynb file to user area
    // (Cannot change files in digitally signed resource area)
    std::string example_file(tree_map[id]);
    wxLogDebug("SpawnExample Item selected [%s]", example_file);
    std::string original_filename(resource_filename(example_file));
    wxLogDebug("SpawnExample Filename [%s]", original_filename);
    std::string prefix(wxStandardPaths::Get().GetUserDataDir());
    wxFileName wxf(prefix, example_file);
    std::string new_filename(wxf.GetFullPath());
    // Overwrite old copy with permission
    if (wxf.IsOk() && wxf.Exists()) {
        if (files_different(original_filename, new_filename)) {
            wxLogDebug("SpawnExample Files different");
            wxMessageDialog dlg(parent, "Keep your changes to the example?",
                "Example notebook has changed", wxYES_NO | wxICON_WARNING | wxNO_DEFAULT);
            if (dlg.ShowModal() == wxID_NO) {
                wxLogDebug("SpawnExample You picked NO");
                // Overwrite changes with fresh copy
                write_file(new_filename, read_all_file(original_filename));
            }
        }
    } else {
        // Output filename doesn't exist, write it
        write_file(new_filename, read_all_file(original_filename));
    }
    wxLogDebug("SpawnExample Loading file [%s]", new_filename);
    MainFrame::Spawn(url_from_filename(new_filename), new_filename, false);
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
            auto id = tree->AppendItem(subtree, item.first);
            tree_map[id] = item.second;
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

