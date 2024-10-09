"use strict";
var rn = require("react-native");
var {TurboModuleRegistry } = rn;
Object.defineProperty(exports, "__esModule", { value: true });
exports.mmkvBridgeModule = void 0;
//@ts-ignore
const isDebugMode = global.location && global.location.pathname && global.location.pathname.includes('/debugger-ui');
exports.mmkvBridgeModule = !isDebugMode
    ? require('react-native').NativeModules.MMKVNative
    : {
        install: () => {
            console.warn(`Remote debugging is not supported by JSI modules. MMKV is running with a memory adapter currently and is fully functional for testing only. Hence any values will not persist on App refresh/reload. `);
            require('../../../jest/dist/jest/memoryStore.js').mock();
            return true;
        }
    };
/**
 * All jsi functions bound to global object.
 *
 * The last param `id` is the instance id of the storage instance we want to get/set the value.
 *
 * `undefined`: It means that instance is not loaded
 *
 * `null`: Value does not exist or some error occured while getting the value
 *
 */
//@ts-ignore
var mmkvModule = TurboModuleRegistry.get('MMKVNative')
mmkvModule.install()
const mmkvJsiModule = global;
exports.default = mmkvJsiModule;