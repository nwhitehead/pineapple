/**
 * Theme extension
 *
 * Allows dynamic load of css file to set theme.
 */
define([
    'base/js/namespace',
    'base/js/events',
    'notebook/js/notebook',
    'notebook/js/cell',
], function(IPython, events, notebook, cell) {

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

    // In case we were not loaded when a theme was requested...
    if (typeof global_start_theme !== 'undefined') {
        set_theme(global_start_theme);
    }

    return {
        set_theme: set_theme,
    };
});
