// Extension metadata
const ExtensionUtils = imports.misc.extensionUtils;
const Me = ExtensionUtils.getCurrentExtension();

class Extension {
    constructor() {
        this._enabled = false;
    }

    enable() {
        this._enabled = true;
        // Hook into the required components
        this._hookGlobalDisplay();
        this._hookMainWm();
        this._hookAltTabWindowSwitcher();
    }

    disable() {
        this._enabled = false;
        // Unhook the components
        this._unhookGlobalDisplay();
        this._unhookMainWm();
        this._unhookAltTabWindowSwitcher();
    }

    _hookGlobalDisplay() {
        // Store original methods to restore later
        this._originalDisplayMethods = {};

        // Hook into global.display methods
        if (global && global.display) {
            // Example: Hook into window management events
            this._displaySignals = [];

            // Connect to various display signals
            let signalIds = [
                global.display.connect('window-created', (display, window) => {
                    this._onWindowCreated(window);
                }),
                global.display.connect('window-entered-monitor', (display, monitor, window) => {
                    this._onWindowEnteredMonitor(monitor, window);
                }),
                global.display.connect('window-left-monitor', (display, monitor, window) => {
                    this._onWindowLeftMonitor(monitor, window);
                })
            ];

            this._displaySignals.push(...signalIds);
            log('Extension: Successfully hooked into global.display');
        } else {
            log('Extension: global.display not available');
        }
    }

    _unhookGlobalDisplay() {
        // Disconnect all signals
        if (this._displaySignals) {
            this._displaySignals.forEach(id => {
                if (global.display) {
                    global.display.disconnect(id);
                }
            });
            this._displaySignals = null;
        }
        log('Extension: Unhooked from global.display');
    }

    _onWindowCreated(window) {
        log(`Extension: Window created - ${window.get_title ? window.get_title() : 'Unknown'}`);
    }

    _onWindowEnteredMonitor(monitor, window) {
        log(`Extension: Window entered monitor ${monitor} - ${window.get_title ? window.get_title() : 'Unknown'}`);
    }

    _onWindowLeftMonitor(monitor, window) {
        log(`Extension: Window left monitor ${monitor} - ${window.get_title ? window.get_title() : 'Unknown'}`);
    }

    _hookMainWm() {
        try {
            // Import Main module
            const Main = imports.ui.main;

            // Store original methods to restore later
            this._originalWmMethods = {};

            // Hook into Main.wm if available
            if (Main && Main.wm) {
                // Example: Override window management behaviors
                if (Main.wm.allowKeybinding) {
                    this._originalWmMethods.allowKeybinding = Main.wm.allowKeybinding;
                    Main.wm.allowKeybinding = this._overrideAllowKeybinding.bind(this);
                }

                if (Main.wm.focusWindow) {
                    this._originalWmMethods.focusWindow = Main.wm.focusWindow;
                    Main.wm.focusWindow = this._overrideFocusWindow.bind(this);
                }

                log('Extension: Successfully hooked into Main.wm');
            } else {
                log('Extension: Main.wm not available');
            }
        } catch (e) {
            log('Extension: Error hooking into Main.wm: ' + e.message);
        }
    }

    _unhookMainWm() {
        try {
            const Main = imports.ui.main;

            // Restore original methods
            if (Main && Main.wm && this._originalWmMethods) {
                if (this._originalWmMethods.allowKeybinding) {
                    Main.wm.allowKeybinding = this._originalWmMethods.allowKeybinding;
                }

                if (this._originalWmMethods.focusWindow) {
                    Main.wm.focusWindow = this._originalWmMethods.focusWindow;
                }
            }

            log('Extension: Unhooked from Main.wm');
        } catch (e) {
            log('Extension: Error unhooking from Main.wm: ' + e.message);
        }
    }

    _overrideAllowKeybinding(name) {
        // Custom logic for allowing keybindings
        log(`Extension: Checking keybinding: ${name}`);
        // Call original method
        const Main = imports.ui.main;
        if (this._originalWmMethods && this._originalWmMethods.allowKeybinding) {
            return this._originalWmMethods.allowKeybinding.call(Main.wm, name);
        }
        return true; // Default behavior
    }

    _overrideFocusWindow(window, time) {
        // Custom logic for focusing window
        log(`Extension: Focusing window: ${window.title || 'Unknown'}`);
        // Call original method
        const Main = imports.ui.main;
        if (this._originalWmMethods && this._originalWmMethods.focusWindow) {
            return this._originalWmMethods.focusWindow.call(Main.wm, window, time);
        }
    }

    _hookAltTabWindowSwitcher() {
        try {
            // Import AltTab module
            const AltTab = imports.ui.altTab;

            // Store original methods to restore later
            this._originalAltTabMethods = {};

            // Hook into AltTab.WindowSwitcherPopup if available
            if (AltTab && AltTab.WindowSwitcherPopup) {
                // Override the WindowSwitcherPopup constructor or methods
                if (AltTab.WindowSwitcherPopup.prototype._createList) {
                    this._originalAltTabMethods._createList = AltTab.WindowSwitcherPopup.prototype._createList;
                    AltTab.WindowSwitcherPopup.prototype._createList = this._overrideCreateList.bind(this);
                }

                if (AltTab.WindowSwitcherPopup.prototype._finish) {
                    this._originalAltTabMethods._finish = AltTab.WindowSwitcherPopup.prototype._finish;
                    AltTab.WindowSwitcherPopup.prototype._finish = this._overrideFinish.bind(this);
                }

                log('Extension: Successfully hooked into AltTab.WindowSwitcherPopup');
            } else {
                log('Extension: AltTab.WindowSwitcherPopup not available');
            }
        } catch (e) {
            log('Extension: Error hooking into AltTab.WindowSwitcherPopup: ' + e.message);
        }
    }

    _unhookAltTabWindowSwitcher() {
        try {
            const AltTab = imports.ui.altTab;

            // Restore original methods
            if (AltTab && AltTab.WindowSwitcherPopup && this._originalAltTabMethods) {
                if (this._originalAltTabMethods._createList) {
                    AltTab.WindowSwitcherPopup.prototype._createList = this._originalAltTabMethods._createList;
                }

                if (this._originalAltTabMethods._finish) {
                    AltTab.WindowSwitcherPopup.prototype._finish = this._originalAltTabMethods._finish;
                }
            }

            log('Extension: Unhooked from AltTab.WindowSwitcherPopup');
        } catch (e) {
            log('Extension: Error unhooking from AltTab.WindowSwitcherPopup: ' + e.message);
        }
    }

    _overrideCreateList() {
        log('Extension: Overriding AltTab WindowSwitcherPopup _createList');
        // Call original method
        if (this._originalAltTabMethods && this._originalAltTabMethods._createList) {
            return this._originalAltTabMethods._createList.apply(this, arguments);
        }
    }

    _overrideFinish(backward, panelPosition) {
        log('Extension: Overriding AltTab WindowSwitcherPopup _finish');
        // Call original method
        if (this._originalAltTabMethods && this._originalAltTabMethods._finish) {
            return this._originalAltTabMethods._finish.apply(this, [backward, panelPosition]);
        }
    }
}

function init() {
    return new Extension();
}