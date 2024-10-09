"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.stringToHex = exports.options = exports.DATA_TYPES = exports.ProcessingModes = exports.IOSAccessibleStates = exports.promisify = void 0;
function promisify(fn) {
    return function (...args) {
        return new Promise(resolve => {
            resolve(fn(...args));
        });
    };
}
exports.promisify = promisify;
/**
 * Accessible modes for iOS Keychain
 */
exports.IOSAccessibleStates = {
    WHEN_UNLOCKED: 'AccessibleWhenUnlocked',
    AFTER_FIRST_UNLOCK: 'AccessibleAfterFirstUnlock',
    /** @deprected in iOS 16+ */
    ALWAYS: 'AccessibleAlways',
    WHEN_PASSCODE_SET_THIS_DEVICE_ONLY: 'AccessibleWhenPasscodeSetThisDeviceOnly',
    WHEN_UNLOCKED_THIS_DEVICE_ONLY: 'AccessibleWhenUnlockedThisDeviceOnly',
    AFTER_FIRST_UNLOCK_THIS_DEVICE_ONLY: 'AccessibleAfterFirstUnlockThisDeviceOnly',
    /** @deprected in iOS 16+ */
    ALWAYS_THIS_DEVICE_ONLY: 'AccessibleAlwaysThisDeviceOnly'
};
/**
 * Processing modes for storage.
 */
exports.ProcessingModes = {
    SINGLE_PROCESS: 1,
    MULTI_PROCESS: 2
};
exports.DATA_TYPES = Object.freeze({
    STRING: 1,
    NUMBER: 2,
    BOOL: 3,
    MAP: 4,
    ARRAY: 5
});
/**
 * Information about all storage instances
 */
exports.options = {};
const stringToHex = (input) => {
    let str = '';
    //@ts-ignore
    for (const char of input) {
        str += char.charCodeAt(0).toString(16);
    }
    return str;
};
exports.stringToHex = stringToHex;
