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
