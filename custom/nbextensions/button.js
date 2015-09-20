/**
 * Button extension
 *
 * Turn cells into buttons that can be clicked to run.
 * Types are:
 *     none (don't show any input, but user can run cell)
 *     run (input turns into button, shows output)
 *     submit (button runs previous cell and current cell)
 */
define([
    'base/js/namespace',
    'base/js/events',
    'notebook/js/notebook',
    'notebook/js/cell',
], function(IPython, events, notebook, cell) {

    /**
     * Set codecell to be button
     * 
     *  @param {Object} cell current notebook cell
     *  @param {String} val is type of button
     */
    var setButton = function (cell, val) {
        if (val == undefined) {
            val = false;
        }
        cell.metadata.button = val;
        cell.button = val;
        var title = val + '';
        cell.element.find('.cell-button').remove();
        if (val !== false) {
            cell.element.addClass("is-button");
            cell.element
                .find('.input')
                .show();
            if (val === 'run') {
                cell.element
                    .find('.inner_cell')
                    .append('<div class="btn btn-default cell-button">&#x25b6;</div>');
                cell.element
                    .find('.cell-button')
                    .click(function() {
                        cell.execute();
                    });
            }
            if (val === 'none') {
                cell.element
                    .find('.input')
                    .hide();
            }
            if (val === 'submit') {
                cell.element
                    .find('.inner_cell')
                    .append('<div class="btn btn-default cell-button">&#x25b6;</div>');
                cell.element
                    .find('.cell-button')
                    .click(function() {
                        var cells = IPython.notebook.get_cells();
                        var i = cells.indexOf(cell);
                        if (i > 0) {
                            cells[i - 1].execute();
                        }
                        cell.execute();
                    });
            }
        } else {
            cell.element.removeClass("is-button");
        }
    };
    function getButton(cell) {
        return cell.metadata.button;
    }

    // Wait until notebook loaded, then update
    events.on('notebook_loaded.Notebook', function() {
        // loop through notebook and set read-only cells defined in metadata
        var cells = IPython.notebook.get_cells();
        for(var i in cells){
            var cell = cells[i];
            setButton(cell, cell.metadata.button);
        };
    });

    /**
     * Set current cell to be button of given type
     * 
     *  @param {String} val is type of button
     */
    function setSelectionButton(val) {
        var cell = IPython.notebook.get_selected_cell();
        setButton(cell, val);
    }

    return {
        setSelectionButton: setSelectionButton
    };
});
