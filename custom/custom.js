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
    'custom/nbextensions/button'
], function(IPython, events, notebook, cell, theme, readOnly, button) {
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

    return {
        set_theme: theme.set_theme,
        toggleReadOnly: readOnly.toggleReadOnly,
        setSelectionButton: button.setSelectionButton
    };
});
