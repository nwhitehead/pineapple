
#include "ExamplesFrame.hh"

#include <map>
#include <string>
#include <utility>
#include <vector>

#include <wx/filename.h>
#include <wx/frame.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>

#include "util.hh"


/**
 * ExamplesFrame constructor and associated helpers
 */

ExamplesFrame::ExamplesFrame(wxWindow */* parent */)
        : wxFrame(nullptr, wxID_ANY, "Choose example")
{
    wxLogDebug("ExamplesFrame::ExamplesFrame");

    SetupTree();
    SetupBindings();
    Show();
}

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
            wxLogDebug("ExamplesFrame::Tree Item selected [%s]", tree_map[id]);
            
        }
    }, tree->GetId());
    tree->ExpandAll();
    tree->Show();
    SetSize(wxDefaultCoord, wxDefaultCoord, 400, 400);
}

void ExamplesFrame::SetupBindings()
{

}

void ExamplesFrame::OnClose(wxCloseEvent &/* event */)
{

}

void ExamplesFrame::OnChooseItem(wxCommandEvent &/* event */)
{

}

