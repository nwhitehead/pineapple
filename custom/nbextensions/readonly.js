/**
 * Read-Only extension
 *
 * Allows cells to be marked read-only, get special styling,
 * and prevented from being moved/edited (until made not read-only).
 */
define([
    'base/js/namespace',
    'base/js/events',
    'notebook/js/notebook',
    'notebook/js/cell'
], function(IPython, events, notebook, cell) {

    /**
     * Set codecell to read-only 
     * 
     *  @param {Object} cell current notebook cell
     *  @param {Boolean} val is cell read-only
     */
    var setReadOnly = function (cell, val) {
        if (val == undefined) {
            val = false;
        }
        if (cell.metadata.run_control == undefined) {
            cell.metadata.run_control = {};
        }
        cell.metadata.run_control.read_only = val;
        cell.metadata.deletable = !val;
        cell.read_only = val;
        var prompt = cell.element;
        if (val == true) {
            prompt.addClass("read-only");
        } else {
            prompt.removeClass("read-only");
        }
        cell.code_mirror.setOption('readOnly', val);
    };

    function getReadOnly(cell) {
        if (cell.metadata.run_control == undefined) {
            cell.metadata.run_control = {};
        }
        return cell.metadata.run_control.read_only;
    }

    function toggleReadOnly() {
        var cell = IPython.notebook.get_selected_cell();
        setReadOnly(cell, !getReadOnly(cell));
    }

    // Wait until notebook loaded, then update readonly data
    events.on('notebook_loaded.Notebook', function() {
        // loop through notebook and set read-only cells defined in metadata
        var cells = IPython.notebook.get_cells();
        for(var i in cells){
            var cell = cells[i];
            if (cell.metadata.run_control != undefined) {
                setReadOnly(cell, cell.metadata.run_control.read_only);
            } else {
                setReadOnly(cell, false);
            }
        };
    });

    /// Only allow unrendering if not readonly
    var old_unrender = cell.Cell.prototype.unrender;
    cell.Cell.prototype.unrender = function() {
        if (!getReadOnly(this)) {
            return old_unrender.apply(this);
        }
        return false;
    };

    /// Don't allow moving readonly cells
    var old_move_cell_up = notebook.Notebook.prototype.move_cell_up;
    notebook.Notebook.prototype.move_cell_up = function (index) {
        var i = this.index_or_selected(index);
        if (this.is_valid_cell_index(i)) {
            var cell = this.get_cell(i);
            if (!getReadOnly(cell)) {
                return old_move_cell_up.apply(this, [index]);
            }
        }
        return this;
    };

    var old_move_cell_down = notebook.Notebook.prototype.move_cell_down;
    notebook.Notebook.prototype.move_cell_down = function (index) {
        var i = this.index_or_selected(index);
        if (this.is_valid_cell_index(i)) {
            var cell = this.get_cell(i);
            if (!getReadOnly(cell)) {
                return old_move_cell_down.apply(this, [index]);
            }
        }
        return this;
    };

    return {
        toggleReadOnly: toggleReadOnly,
    };
});
