/**
 * Tabs
 *
 * Allows notebooks to be broken into sheets that can be switched
 * between using vertical tabs.
 */
define([
    'jquery',
    'base/js/namespace',
    'base/js/events',
    'notebook/js/notebook',
    'notebook/js/cell'
], function($, IPython, events, notebook, cell) {

    var sheet = 0;
    var tabSelectorElement = undefined;

    /**
     * Set codecell to start new sheet (or not)
     * 
     *  @param {Object} cell current notebook cell
     *  @param {Boolean} val is cell starts new sheet
     */
    var setSheetNew = function (cell, val) {
        if (val == undefined) {
            val = false;
        }
        cell.metadata.new_sheet = val;
    };
    var getSheetNew = function (cell) {
        return cell.metadata.new_sheet;
    };

    // Which sheet is the cell on?
    var getSheetNum = function (cell) {
        var cells = IPython.notebook.get_cells();
        var sheetNum = 0;
        var numCells = 0;
        for(var i in cells) {
            var acell = cells[i];
            if (getSheetNew(acell) && numCells > 0) {
                sheetNum++;
            }
            numCells++;
            if (cell === acell) {
                break;
            }
        }
        return sheetNum;
    };

    // What indexes are on the current sheet?
    var getSheetRange = function (sheet) {
        var cells = IPython.notebook.get_cells();
        var sheetNum = 0;
        var numCells = 0;
        var lo = null;
        var hi = null;
        for(var i = 0; i < cells.length; i++) {
            var acell = cells[i];
            if (getSheetNew(acell) && numCells > 0) {
                sheetNum++;
            }
            numCells++;
            if (sheetNum === sheet && i !== undefined) {
                if (i < lo || lo === null) lo = i;
                if (i > hi || hi === null) hi = i;
            }
        }
        return {lo: lo, hi: hi};
    };

    // How many sheets are there?
    var numSheets = function () {
        var cells = IPython.notebook.get_cells();
        var sheetNum = 0;
        var numCells = 0;
        for(var i in cells) {
            var acell = cells[i];
            if (getSheetNew(acell) && numCells > 0) {
                sheetNum++;
            }
            numCells++;
        }
        return sheetNum + 1;
    };

    // Show particular sheet
    var showSheet = function (index) {
        sheet = index;
        // Undefined means show all
        if (sheet === undefined) {
            for(var i in cells) {
                var cell = cells[i];
                if (cell) {
                    cell.element.show();
                }
            }
        } else {
            var num = numSheets();
            // Force sheet to be in valid range
            if (sheet < 0) sheet = 0;
            if (sheet >= num) sheet = num - 1;
            // Show/hide cells depending on their sheet number
            var cells = IPython.notebook.get_cells();
            for(var i in cells) {
                var cell = cells[i];
                if (cell) {
                    var num = getSheetNum(cell);
                    if (num === sheet) {
                        // Use animation to ensure codemirror is drawn
                        cell.element.show(1);
                    } else {
                        cell.element.hide(1);
                    }
                }
            }
        }
        updateSelector();
    };

    // Update selector to match contents of notebook
    function updateSelector() {
        var elem = tabSelectorElement;
        if (!elem) return;
        elem.empty();
        var num = numSheets();
        if (num <= 1) {
            return;
        }
        var $space = $('<div class="tab-space"></div>');
        elem.append($space);
        for (var i = 0; i < num; i++) {
            var $link = $('<div class="tab"></div>');
            if (i === sheet) {
                $link.addClass('active');
            }
            $link.on('click', (function(i) {
                return function() {
                    showSheet(i);
                };
            })(i));
            elem.append($link);
        }
        var $space = $('<div class="tab-space"></div>');
        elem.append($space);
    }

    // Make sure selection is within bounds
    function onCurrentSheet(index) {
        var cell = IPython.notebook.get_cells()[index];
        var num = getSheetNum(cell);
        return (num === sheet);
    };

    // Toggle whether the current cell is a new sheet cell or not
    function toggleSheetNew() {
        var cell = IPython.notebook.get_selected_cell();
        setSheetNew(cell, !getSheetNew(cell));
        showSheet(sheet);
    }

    // Wait until notebook loaded, then update all cells
    events.on('notebook_loaded.Notebook', function() {
        var cells = IPython.notebook.get_cells();
        for(var i in cells){
            var cell = cells[i];
            setSheetNew(cell, cell.metadata.new_sheet);
        };
        // Add tab selector to notebook
        $('#notebook').after('<div class="tab-selector"></div>');
        tabSelectorElement = $('div.tab-selector');
        updateSelector();
        showSheet(0);
    });

    /// Monkey patch to update selector as notebook changes
    var old_delete_cell = notebook.Notebook.prototype.delete_cell;
    notebook.Notebook.prototype.delete_cell = function (index) {
        var res = old_delete_cell.apply(this, [index]);
        showSheet(sheet);
        return res;
    };
    var old_insert_cell_at_index = notebook.Notebook.prototype.insert_cell_at_index;
    notebook.Notebook.prototype.insert_cell_at_index = function (type, index) {
        var res = old_insert_cell_at_index.apply(this, [type, index]);
        showSheet(sheet);
        return res;
    };
    var old_select = notebook.Notebook.prototype.select;
    notebook.Notebook.prototype.select = function (index) {
        var range = getSheetRange(sheet);
        if (index < range.lo) index = range.lo;
        if (index > range.hi) index = range.hi;
        return old_select.apply(this, [index]);
    };

    return {
        toggleSheetNew: toggleSheetNew,
        getSheetNum: getSheetNum,
        showSheet: showSheet,
        numSheets: numSheets,
        updateSelector: updateSelector,
        getSheetRange: getSheetRange,
    };
});
