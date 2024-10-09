export type IndexType = 'stringIndex' | 'boolIndex' | 'numberIndex' | 'mapIndex' | 'arrayIndex';

export const getVector: (key: string, result: string[]) => boolean;

export const getDefaultMMKV: (mode: number, cryptKey?: string) => bigint;

export const mmkvWithID: (mmapID: string, mode: number, cryptKey?: string, rootPath?: string, expectedCapacity?: bigint) => bigint;

export const setStringMMKV: (key: string, value: string, id: string) => boolean | undefined;

export const setupMMKVInstance: (id: string, mode: number, key: string, rootpath: string, indexing: boolean) => boolean;

export const getStringMMKV: (key: string, id: string) => string;

export const removeValueMMKV: (key: string, id: string) => boolean | undefined;

export const getSecureKey: (alias: string) => string | null;

export const setMultiMMKV: (key: string[], value: string[], type: string, id: string) =>boolean;

export const getIndexMMKV: (type: IndexType, id: string) => string[] | undefined;

export const encryptMMKV: (cryptKey: string, id: string) => boolean | undefined;

export const decryptMMKV: (id: string) => boolean | undefined;

export const setMapMMKV: (key: string, value: string, id: string) => boolean | undefined;

export const getMapMMKV: (key: string, id: string) => string | undefined;

export const setArrayMMKV: (key: string, value: string, id: string) => boolean | undefined;

export const getArrayMMKV: (key: string, id: string) => string | undefined;

export const setNumberMMKV: (key: string, value: number, id: string) => boolean | undefined;

export const getNumberMMKV: (key: string, id: string) => number | undefined;

export const setBoolMMKV: (key: string, value: boolean, id: string) => boolean | undefined;

export const getBoolMMKV: (key: string, id: string) => boolean | undefined;

export const removeValuesMMKV: (key: string[], id: string) => boolean | undefined;

export const getAllKeysMMKV: (id: string) => string[] | undefined;

export const containsKeyMMKV: (key: string, id: string) => boolean | undefined;

export const  clearMMKV: (id: string) => boolean | undefined;

export const  clearMemoryCache: (id: string) => boolean | undefined;

export const  getMultiMMKV: (keys: string[], id: string) => string[];

