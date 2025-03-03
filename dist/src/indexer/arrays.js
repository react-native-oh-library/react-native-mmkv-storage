"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    function adopt(value) { return value instanceof P ? value : new P(function (resolve) { resolve(value); }); }
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : adopt(result.value).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const handlers_1 = require("../handlers");
const module_1 = __importDefault(require("../module"));
const INDEX_TYPE = 'arrayIndex';
/**
 * Index of all array values stored in storage
 */
class arrayIndex {
    constructor(id) {
        this.instanceID = id;
    }
    /**
     *
     * Get all keys
     */
    getKeys() {
        return __awaiter(this, void 0, void 0, function* () {
            return yield (0, handlers_1.handleActionAsync)(module_1.default.getIndexMMKV, INDEX_TYPE, this.instanceID);
        });
    }
    /**
     * Check if a key exists.
     */
    hasKey(key) {
        let keys = (0, handlers_1.handleAction)(module_1.default.getIndexMMKV, INDEX_TYPE, this.instanceID);
        return keys && keys.indexOf(key) > -1;
    }
    /**
     * Get all arrays from storage.
     */
    getAll() {
        return __awaiter(this, void 0, void 0, function* () {
            return new Promise(resolve => {
                let keys = (0, handlers_1.handleAction)(module_1.default.getIndexMMKV, INDEX_TYPE, this.instanceID);
                if (!keys)
                    keys = [];
                let items = [];
                for (let i = 0; i < keys.length; i++) {
                    //@ts-ignore
                    let item = [];
                    item[0] = keys[i];
                    let array = module_1.default.getArrayMMKV(keys[i], this.instanceID);
                    item[1] = array ? JSON.parse(array) : null;
                    items.push(item);
                }
                resolve(items);
            });
        });
    }
}
exports.default = arrayIndex;
