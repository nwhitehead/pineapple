/**
 * Pineapple custom js
 *
 * This code runs once on document load
 */
define([
    'base/js/namespace',
    'base/js/events',
    'notebook/js/notebook',
    'notebook/js/cell',
], function(IPython, events, notebook, cell) {
    /// Register permanent events
    var flash = function(txt) {
        var old = document.title;
        document.title = txt;
        document.title = old;
    };
    events.on('kernel_busy.Kernel', function (evt) {
        flash('$$$$-1|true');
    });
    events.on('kernel_idle.Kernel', function (evt) {
        flash('$$$$-1|false');
    });

    /// Theming support
    var set_theme = function(url) {
        var cssId = 'theme';
        var elem = document.getElementById(cssId);
        if (elem) {
            elem.parentNode.removeChild(elem);
        }
        var head  = document.getElementsByTagName('head')[0];
        var link  = document.createElement('link');
        link.id   = cssId;
        link.rel  = 'stylesheet';
        link.type = 'text/css';
        link.href = url;
        link.media = 'all';
        head.appendChild(link);
        console.log("Changed theme to url: " + url);
    };
    
    if (typeof global_start_theme !== 'undefined') {
        set_theme(global_start_theme);
    }

    /**
     * Concatenate associative array objects
     *
     * Source: http://stackoverflow.com/questions/2454295/javascript-concatenate-properties-from-multiple-objects-associative-array
     */
    function collect() {
        var ret = {};
        var len = arguments.length;
        for (var i=0; i<len; i++) {
            for (var p in arguments[i]) {
                if (arguments[i].hasOwnProperty(p)) {
                    ret[p] = arguments[i][p];
                }
            }
        }
        return ret;
    }

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
                return old_move_cell_up.apply(this, index);
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
                return old_move_cell_down.apply(this, index);
            }
        }
        return this;
    };


    return {
        set_theme: set_theme,
        toggleReadOnly: toggleReadOnly
    };
});
