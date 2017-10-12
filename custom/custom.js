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
    'custom/nbextensions/theme',
    'custom/nbextensions/readonly',
    'custom/nbextensions/button',
    'custom/nbextensions/tabs',
], function(IPython, events, notebook, cell, theme, readOnly, button, tabs) {

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

    // When notebook is loaded and kernel_selector filled, respond
    events.on('command_mode.Notebook', function(evt) {
        var selector = IPython.notebook.kernel_selector;
        var response = function() {
            flash('$$$$-3|' + JSON.stringify(selector.kernelspecs));
        };
        if (selector._loaded) {
            response();
        } else {
            selector.loaded.then(response);
        }
    });


    return {
        set_theme: theme.set_theme,
        toggleReadOnly: readOnly.toggleReadOnly,
        setSelectionButton: button.setSelectionButton,
        toggleSheetNew: tabs.toggleSheetNew
    };
});
