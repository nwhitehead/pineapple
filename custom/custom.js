/**
 * Pineapple custom js
 *
 * This code runs once on document load
 */
define([
    'base/js/namespace',
    'base/js/events',
], function(IPython, events) {
    var flash = function(txt) {
        var old = document.title;
        document.title = txt;
        document.title = old;
    };
    /// Register permanent events
    events.on('kernel_busy.Kernel', function (evt) {
        flash('$$$$-1|true');
    });
    events.on('kernel_idle.Kernel', function (evt) {
        flash('$$$$-1|false');
    });
});

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

// Default theme
set_theme('/custom/theme-light.css');
