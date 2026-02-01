const Main = imports.ui.main;
const Meta = imports.gi.Meta;
const AltTab = imports.ui.altTab;

class Extension {
    constructor() {
        this._globalDisplaySignals = [];
        this._mainWmOriginalMethods = null;
        this._altTabOriginalMethods = null;
    }

    enable() {
        this._hookGlobalDisplay();
        this._hookMainWm();
        this._hookAltTabWindowSwitcher();
    }

    disable() {
        this._unhookAltTabWindowSwitcher();
        this._unhookMainWm();
        this._unhookGlobalDisplay();
    }

    _hookGlobalDisplay() {
        // Hook into global.display signals
        if (global.display) {
            let signalIds = [
                global.display.connect('window-created', (display, window) => {
                    this._onWindowCreated(window);
                }),
                global.display.connect('window-entered-monitor', (display, monitor, window) => {
                    this._onWindowEnteredMonitor(monitor, window);
                }),
                global.display.connect('window-left-monitor', (display, monitor, window) => {
                    this._onWindowLeftMonitor(monitor, window);
                }),
                global.display.connect('grab-op-began', (display, window, op) => {
                    this._onGrabOpStarted(window, op);
                }),
                global.display.connect('grab-op-ended', (display, window, op) => {
                    this._onGrabOpEnded(window, op);
                })
            ];

            this._globalDisplaySignals.push(...signalIds);
            global.log('Extension: Hooked into global.display');
        }
    }

    _unhookGlobalDisplay() {
        // Disconnect all signals
        if (global.display) {
            this._globalDisplaySignals.forEach(id => {
                global.display.disconnect(id);
            });
        }
        this._globalDisplaySignals = [];
        global.log('Extension: Unhooked from global.display');
    }

    _onWindowCreated(window) {
        global.log(`Extension: Window created - ${window.title || 'Unknown'}`);
    }

    _onWindowEnteredMonitor(monitor, window) {
        global.log(`Extension: Window entered monitor ${monitor.index} - ${window.title || 'Unknown'}`);
    }

    _onWindowLeftMonitor(monitor, window) {
        global.log(`Extension: Window left monitor ${monitor.index} - ${window.title || 'Unknown'}`);
    }

    _onGrabOpStarted(window, op) {
        global.log(`Extension: Grab operation started - ${op} for window ${window.title || 'Unknown'}`);
    }

    _onGrabOpEnded(window, op) {
        global.log(`Extension: Grab operation ended - ${op} for window ${window.title || 'Unknown'}`);
    }

    _hookMainWm() {
        try {
            const Main = imports.ui.main;

            // Store original methods to restore later
            if (Main && Main.wm) {
                this._mainWmOriginalMethods = {
                    allowKeybinding: Main.wm.allowKeybinding,
                    focusWindow: Main.wm.focusWindow
                };

                // Override Main.wm methods
                Main.wm.allowKeybinding = (name) => {
                    global.log(`Extension: Checking keybinding: ${name}`);
                    // Call original method if it exists
                    if (this._mainWmOriginalMethods.allowKeybinding) {
                        return this._mainWmOriginalMethods.allowKeybinding.call(Main.wm, name);
                    }
                    return true;
                };

                Main.wm.focusWindow = (window, time) => {
                    global.log(`Extension: Focusing window: ${window.title || 'Unknown'}`);
                    // Call original method if it exists
                    if (this._mainWmOriginalMethods.focusWindow) {
                        return this._mainWmOriginalMethods.focusWindow.call(Main.wm, window, time);
                    }
                };

                global.log('Extension: Hooked into Main.wm');
            }
        } catch (e) {
            global.log('Extension: Error hooking into Main.wm: ' + e.message);
        }
    }

    _unhookMainWm() {
        try {
            const Main = imports.ui.main;

            // Restore original methods
            if (Main && Main.wm && this._mainWmOriginalMethods) {
                if (this._mainWmOriginalMethods.allowKeybinding) {
                    Main.wm.allowKeybinding = this._mainWmOriginalMethods.allowKeybinding;
                }

                if (this._mainWmOriginalMethods.focusWindow) {
                    Main.wm.focusWindow = this._mainWmOriginalMethods.focusWindow;
                }

                this._mainWmOriginalMethods = null;
            }
            global.log('Extension: Unhooked from Main.wm');
        } catch (e) {
            global.log('Extension: Error unhooking from Main.wm: ' + e.message);
        }
    }

    _hookAltTabWindowSwitcher() {
        try {
            const AltTab = imports.ui.altTab;

            // Store original methods
            if (AltTab && AltTab.WindowSwitcherPopup) {
                this._altTabOriginalMethods = {
                    select: AltTab.WindowSwitcherPopup.prototype.select,
                    _createList: AltTab.WindowSwitcherPopup.prototype._createList
                };

                // Store references to original methods in closure variables
                const originalSelect = this._altTabOriginalMethods.select;
                const originalCreateList = this._altTabOriginalMethods._createList;

                // Override AltTab.WindowSwitcherPopup methods
                AltTab.WindowSwitcherPopup.prototype.select = function(backward, panelPosition) {
                    global.log('Extension: AltTab window selection intercepted');
                    // Call original method using closure variable
                    if (originalSelect) {
                        return originalSelect.apply(this, arguments);
                    }
                };

                AltTab.WindowSwitcherPopup.prototype._createList = function() {
                    global.log('Extension: Creating AltTab window list');
                    // Call original method using closure variable
                    if (originalCreateList) {
                        return originalCreateList.apply(this, arguments);
                    }
                };

                global.log('Extension: Hooked into AltTab.WindowSwitcherPopup');
            }
        } catch (e) {
            global.log('Extension: Error hooking into AltTab.WindowSwitcherPopup: ' + e.message);
        }
    }

    _unhookAltTabWindowSwitcher() {
        try {
            const AltTab = imports.ui.altTab;

            // Restore original methods
            if (AltTab && AltTab.WindowSwitcherPopup && this._altTabOriginalMethods) {
                if (this._altTabOriginalMethods.select) {
                    AltTab.WindowSwitcherPopup.prototype.select = this._altTabOriginalMethods.select;
                }

                if (this._altTabOriginalMethods._createList) {
                    AltTab.WindowSwitcherPopup.prototype._createList = this._altTabOriginalMethods._createList;
                }

                this._altTabOriginalMethods = null;
            }
            global.log('Extension: Unhooked from AltTab.WindowSwitcherPopup');
        } catch (e) {
            global.log('Extension: Error unhooking from AltTab.WindowSwitcherPopup: ' + e.message);
        }
    }
}

function init() {
    return new Extension();
}