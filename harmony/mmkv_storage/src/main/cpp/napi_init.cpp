
#include "MMKVLog.h"
#include "napi/native_api.h"
#include "RNOH/ArkJS.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <jsi/jsi.h>
#include "MMKV.h"
#include "MMKVPredef.h"
#include "MMBuffer.h"
#include <algorithm>
#include "CodedInputData.h"
#include "CodedOutputData.h"
#include "MMKVPredef.h"

#include "MMBuffer.h"
#include "MMKV.h"
#include "MMKVLog.h"
#include "MemoryFile.h"
#include "MiniPBCoder.h"
#include "napi/native_api.h"
#include <cstdint>
#include <system_error>

using namespace std;
using namespace mmkv;

#define NAPI_CALL_RET(call, return_value)                                                                              \
    do {                                                                                                               \
        napi_status status = (call);                                                                                   \
        if (status != napi_ok) {                                                                                       \
            const napi_extended_error_info *error_info = nullptr;                                                      \
            napi_get_last_error_info(env, &error_info);                                                                \
            MMKVInfo("NAPI Error: code %d, msg %s", error_info->error_code, error_info->error_message);                \
            bool is_pending;                                                                                           \
            napi_is_exception_pending(env, &is_pending);                                                               \
            if (!is_pending) {                                                                                         \
                auto message = error_info->error_message ? error_info->error_message : "null";                         \
                napi_throw_error(env, nullptr, message);                                                               \
                return return_value;                                                                                   \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)

#define NAPI_CALL(call) NAPI_CALL_RET(call, nullptr)

static std::vector<MMKV *> mmkvInstances;
static const string dataTypes[] = {
    "stringIndex", "numberIndex", "boolIndex", "mapIndex", "arrayIndex",
};
bool IsNValueUndefined(napi_env env, napi_value value) {
    napi_valuetype type;
    if (napi_typeof(env, value, &type) == napi_ok && type == napi_undefined) {
        return true;
    }
    return false;
}

static std::string NValueToString(napi_env env, napi_value value, bool maybeUndefined = false) {
    if (maybeUndefined && IsNValueUndefined(env, value)) {
        return "";
    }

    size_t size;
    NAPI_CALL_RET(napi_get_value_string_utf8(env, value, nullptr, 0, &size), "");
    std::string result(size, '\0');
    NAPI_CALL_RET(napi_get_value_string_utf8(env, value, (char *)result.data(), size + 1, nullptr), "");
    return result;
}

static napi_value StringToNValue(napi_env env, const std::string &value) {
    napi_value result;
    napi_create_string_utf8(env, value.data(), value.size(), &result);
    return result;
}
// Function to search for a value in the vector
bool hasValue(const std::vector<std::string> &vec, std::string value) {
    return std::binary_search(vec.begin(), vec.end(), value);
}

static std::vector<std::string> NValueToStringArray(napi_env env, napi_value value, bool maybeUndefined = false) {
    std::vector<std::string> keys;
    if (maybeUndefined && IsNValueUndefined(env, value)) {
        return keys;
    }

    uint32_t length = 0;
    if (napi_get_array_length(env, value, &length) != napi_ok || length == 0) {
        return keys;
    }
    keys.reserve(length);

    for (uint32_t index = 0; index < length; index++) {
        napi_value jsKey = nullptr;
        if (napi_get_element(env, value, index, &jsKey) != napi_ok) {
            continue;
        }
        keys.push_back(NValueToString(env, jsKey));
    }
    return keys;
}

static napi_value StringArrayToNValue(napi_env env, const std::vector<std::string> &value) {
    napi_value jsArr = nullptr;
    napi_create_array_with_length(env, value.size(), &jsArr);
    for (size_t index = 0; index < value.size(); index++) {
        auto jsKey = StringToNValue(env, value[index]);
        napi_set_element(env, jsArr, index, jsKey);
    }
    return jsArr;
}

static napi_value DoubleToNValue(napi_env env, double value);
static double NValueToDouble(napi_env env, napi_value value);

static std::vector<double> NValueToDoubleArray(napi_env env, napi_value value, bool maybeUndefined = false) {
    std::vector<double> vec;
    if (maybeUndefined && IsNValueUndefined(env, value)) {
        return vec;
    }

    uint32_t length = 0;
    if (napi_get_array_length(env, value, &length) != napi_ok || length == 0) {
        return vec;
    }
    vec.reserve(length);

    for (uint32_t index = 0; index < length; index++) {
        napi_value jsKey = nullptr;
        if (napi_get_element(env, value, index, &jsKey) != napi_ok) {
            continue;
        }
        vec.push_back(NValueToDouble(env, jsKey));
    }
    return vec;
}

void addValue(std::vector<std::string> &vec, std::string value) {
    auto insertPosition = std::lower_bound(vec.begin(), vec.end(), value);
    vec.insert(insertPosition, value);
}
static napi_value DoubleArrayToNValue(napi_env env, const std::vector<double> &value) {
    napi_value jsArr = nullptr;
    napi_create_array_with_length(env, value.size(), &jsArr);
    for (size_t index = 0; index < value.size(); index++) {
        auto jsKey = DoubleToNValue(env, value[index]);
        napi_set_element(env, jsArr, index, jsKey);
    }
    return jsArr;
}

static napi_value BoolToNValue(napi_env env, bool value);
static bool NValueToBool(napi_env env, napi_value value);

static std::vector<bool> NValueToBoolArray(napi_env env, napi_value value, bool maybeUndefined = false) {
    std::vector<bool> keys;
    if (maybeUndefined && IsNValueUndefined(env, value)) {
        return keys;
    }

    uint32_t length = 0;
    if (napi_get_array_length(env, value, &length) != napi_ok || length == 0) {
        return keys;
    }
    keys.reserve(length);

    for (uint32_t index = 0; index < length; index++) {
        napi_value jsKey = nullptr;
        if (napi_get_element(env, value, index, &jsKey) != napi_ok) {
            continue;
        }
        keys.push_back(NValueToBool(env, jsKey));
    }
    return keys;
}

static napi_value BoolArrayToNValue(napi_env env, const std::vector<bool> &value) {
    napi_value jsArr = nullptr;
    napi_create_array_with_length(env, value.size(), &jsArr);
    for (size_t index = 0; index < value.size(); index++) {
        auto jsKey = BoolToNValue(env, value[index]);
        napi_set_element(env, jsArr, index, jsKey);
    }
    return jsArr;
}

static void my_finalizer(napi_env env, void *finalize_data, void *finalize_hint) {
    // MMKVInfo("free %p", finalize_data);
    free(finalize_data);
}

static mmkv::MMBuffer NValueToMMBuffer(napi_env env, napi_value value, bool maybeUndefined = false) {
    if (maybeUndefined && IsNValueUndefined(env, value)) {
        return mmkv::MMBuffer();
    }

    void *data = nullptr;
    size_t length = 0;
    if (napi_get_arraybuffer_info(env, value, &data, &length) == napi_ok) {
        return mmkv::MMBuffer(data, length, mmkv::MMBufferNoCopy);
    }
    return mmkv::MMBuffer();
}

static napi_value MMBufferToNValue(napi_env env, const mmkv::MMBuffer &value) {
    napi_value result = nullptr;
    void *data = nullptr;
    if (napi_create_arraybuffer(env, value.length(), &data, &result) == napi_ok) {
        memcpy(data, value.getPtr(), value.length());
    }
    return result;
}

static napi_value MMBufferToNValue(napi_env env, mmkv::MMBuffer &&value) {
    if (!value.isStoredOnStack()) {
        napi_value result = nullptr;
        auto ret =
            napi_create_external_arraybuffer(env, value.getPtr(), value.length(), my_finalizer, nullptr, &result);
        if (ret == napi_ok) {
            // MMKVInfo("using napi_create_external_arraybuffer %p", value.getPtr());
            value.detach();
            return result;
        }
    }
    return MMBufferToNValue(env, (const mmkv::MMBuffer &)value);
}

// static napi_value MMBufferToTypeArray(napi_env env, const MMBuffer &value, napi_typedarray_type type) {
//     napi_value result = nullptr;
//     void *data = nullptr;
//     if (napi_create_typedarray(env, type, value.length(), &data, &result) == napi_ok) {
//         memcpy(data, value.getPtr(), value.length());
//     }
//     return result;
// }
//
// static napi_value MMBufferToTypeArray(napi_env env, MMBuffer &&value) {
//     if (!value.isStoredOnStack()) {
//         napi_value result = nullptr;
//         auto ret =
//             napi_create_external_arraybuffer(env, value.getPtr(), value.length(), my_finalizer, nullptr, &result);
//         if (ret == napi_ok) {
//             // MMKVInfo("using napi_create_external_arraybuffer %p", value.getPtr());
//             value.detach();
//             return result;
//         }
//     }
//     return MMBufferToNValue(env, (const MMBuffer &)value);
// }

static napi_value NAPIUndefined(napi_env env) {
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

static napi_value NAPINull(napi_env env) {
    napi_value result;
    napi_get_null(env, &result);
    return result;
}

static napi_value BoolToNValue(napi_env env, bool value) {
    napi_value result;
    napi_value resultBool;
    napi_create_double(env, value, &result);
    napi_coerce_to_bool(env, result, &resultBool);
    return resultBool;
}

static bool NValueToBool(napi_env env, napi_value value) {
    bool result;
    if (napi_get_value_bool(env, value, &result) == napi_ok) {
        return result;
    }
    return false;
}

static napi_value Int32ToNValue(napi_env env, int32_t value) {
    napi_value result;
    napi_create_int32(env, value, &result);
    return result;
}

static int32_t NValueToInt32(napi_env env, napi_value value) {
    int32_t result;
    napi_get_value_int32(env, value, &result);
    return result;
}

static napi_value UInt32ToNValue(napi_env env, uint32_t value) {
    napi_value result;
    napi_create_uint32(env, value, &result);
    return result;
}

static uint32_t NValueToUInt32(napi_env env, napi_value value) {
    uint32_t result;
    napi_get_value_uint32(env, value, &result);
    return result;
}

static napi_value DoubleToNValue(napi_env env, double value) {
    napi_value result;
    napi_create_double(env, value, &result);
    return result;
}

static double NValueToDouble(napi_env env, napi_value value) {
    double result;
    napi_get_value_double(env, value, &result);
    return result;
}

static napi_value Int64ToNValue(napi_env env, int64_t value) {
    napi_value result;
    napi_create_bigint_int64(env, value, &result);
    return result;
}

static int64_t NValueToInt64(napi_env env, napi_value value) {
    int64_t result;
    bool lossless;
    napi_get_value_bigint_int64(env, value, &result, &lossless);
    return result;
}

static napi_value UInt64ToNValue(napi_env env, uint64_t value) {
    napi_value result;
    napi_create_bigint_uint64(env, value, &result);
    return result;
}

static uint64_t NValueToUInt64(napi_env env, napi_value value, bool maybeUndefined = false) {
    if (maybeUndefined && IsNValueUndefined(env, value)) {
        return 0;
    }
    uint64_t result;
    bool lossless;
    napi_get_value_bigint_uint64(env, value, &result, &lossless);
    return result;
}

static napi_value mmkvWithID(napi_env env, napi_callback_info info) {
    size_t argc = 5;
    napi_value args[5] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));

    MMKV *kv = nullptr;
    auto mmapID = NValueToString(env, args[0]);
    if (!mmapID.empty()) {
        int32_t mode = NValueToInt32(env, args[1]);
        auto cryptKey = NValueToString(env, args[2], true);
        auto rootPath = NValueToString(env, args[3], true);
        auto expectedCapacity = NValueToUInt64(env, args[4], true);

        auto cryptKeyPtr = cryptKey.empty() ? nullptr : &cryptKey;
        auto rootPathPtr = rootPath.empty() ? nullptr : &rootPath;
        // MMKVInfo("rootPath: %p, %s, %s", rootPathPtr, rootPath.c_str(), rootPathPtr ? rootPathPtr->c_str() : "");
        kv = MMKV::mmkvWithID(mmapID, mmkv::DEFAULT_MMAP_SIZE, (MMKVMode)mode, cryptKeyPtr, rootPathPtr,
                              expectedCapacity);
    }

    return UInt64ToNValue(env, (uint64_t)kv);
}

static napi_value initialize(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));

    auto rootDir = NValueToString(env, args[0]);
    auto cacheDir = NValueToString(env, args[1]);

    int32_t logLevel;
    NAPI_CALL(napi_get_value_int32(env, args[2], &logLevel));

    MMKVInfo("rootDir: %s, cacheDir: %s, log level:%d", rootDir.c_str(), cacheDir.c_str(), logLevel);

    MMKV::initializeMMKV(rootDir, (MMKVLogLevel)logLevel);
    mmkv::g_android_tmpDir = cacheDir;

    return StringToNValue(env, MMKV::getRootDir());
}

static napi_value getDefaultMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));

    int32_t mode;
    NAPI_CALL(napi_get_value_int32(env, args[0], &mode));
    auto crypt = NValueToString(env, args[1], true);

    MMKV *kv = nullptr;
    if (crypt.length() > 0) {
        kv = MMKV::defaultMMKV((MMKVMode)mode, &crypt);
    }

    if (!kv) {
        kv = MMKV::defaultMMKV((MMKVMode)mode, nullptr);
    }

    return UInt64ToNValue(env, (uint64_t)kv);
}

void sortVector(std::vector<std::string> &vec) { std::sort(vec.begin(), vec.end()); }

static MMKV *getInstance(const std::string &ID) {
    auto kv =
        std::find_if(mmkvInstances.begin(), mmkvInstances.end(), [&ID](MMKV *inst) { return inst->mmapID() == ID; });
    if (kv == mmkvInstances.end()) {
        return nullptr;
    }
    return *kv;
}

std::unordered_map<std::string, bool> indexing_enabled = {};
std::unordered_map<std::string, std::unordered_map<std::string, std::vector<std::string>>> index_cache = {};


static std::vector<std::string> getIndex(MMKV *kv, const std::string &type) {
    if (!indexing_enabled[kv->mmapID()])
        return {};

    auto kvIndex = index_cache[kv->mmapID()];

    if (kvIndex.count(type) == 0) {
        auto exists = kv->getVector(type, kvIndex[type]);
        if (!exists) {
            kvIndex[type] = std::vector<std::string>();
        } else {
            sortVector(kvIndex[type]);
        }
    }

    return kvIndex[type];
}

static void setIndex(MMKV *kv, const string &type, const string &key) {
    if (!indexing_enabled[kv->mmapID()])
        return;
    auto index = getIndex(kv, type);
    if (!hasValue(index, key)) {
        addValue(index, key);
        kv->set(index, type);
    }
}


static napi_value pageSize(napi_env env, napi_callback_info info) {
    napi_value value;
    NAPI_CALL(napi_create_uint32(env, DEFAULT_MMAP_SIZE, &value));
    return value;
}

static napi_value getVector(napi_env env, napi_callback_info info) {
    ArkJS arkJs(env);
    auto args = arkJs.getCallbackArgs(info, 2);
    std::string key = arkJs.getString(args[0]);
    // 获取数组的长度
    std::uint32_t arrayLength = arkJs.getArrayLength(args[1]);
    // 创建一个空数组
    napi_value jsArray = nullptr;
    napi_create_array(env, &jsArray);
    // 根据数组长度循环，将数组的元素取出
    for (int i = 0; i < arrayLength; i++) {

        napi_value value = arkJs.getArrayElement(args[1], i);
        napi_set_element(env, jsArray, i, value);
    };
    // 创建一个新数组，将数组返回
    std::vector<std::string> array = NValueToStringArray(env, jsArray);
    auto handle = NValueToUInt64(env, args[0]);
    MMKV *kv = reinterpret_cast<MMKV *>(handle);
    //     bool   vectorValue = kv->getVector(key,array);

    //     return BoolToNValue(env,vectorValue);
    return BoolToNValue(env, true);
}
void removeValue(std::vector<std::string> &vec, std::string value) {
    auto position = std::lower_bound(vec.begin(), vec.end(), value);
    if (position != vec.end() && *position == value) {
        vec.erase(position);
    }
}
static void removeFromIndex(MMKV *kv, const string &key) {
    if (!indexing_enabled[kv->mmapID()])
        return;
    for (const auto &idx : dataTypes) {
        auto index = getIndex(kv, idx);
        if (hasValue(index, key)) {
            removeValue(index, key);
            kv->set(index, idx);
            return;
        }
    }
}
static void removeKeysFromIndex(MMKV *kv, const vector<string> &arrKeys) {
    if (!indexing_enabled[kv->mmapID()])
        return;

    auto strings = false;
    auto objects = false;
    auto arrays = false;
    auto numbers = false;
    auto booleans = false;

    for (const auto &key : arrKeys) {
        auto removed = false;
        for (const auto &idx : dataTypes) {
            if (removed)
                continue;

            auto index = getIndex(kv, idx);
            if (hasValue(index, key)) {
                removeValue(index, key);

                if (idx == "stringIndex")
                    strings = true;
                if (idx == "numberIndex")
                    numbers = true;
                if (idx == "boolIndex")
                    booleans = true;
                if (idx == "mapIndex")
                    objects = true;
                if (idx == "arrayIndex")
                    arrays = true;
                removed = true;
            }
        }
    }

    if (strings)
        kv->set(getIndex(kv, "stringIndex"), "stringIndex");
    if (objects)
        kv->set(getIndex(kv, "mapIndex"), "mapIndex");
    if (arrays)
        kv->set(getIndex(kv, "arrayIndex"), "arrayIndex");
    if (numbers)
        kv->set(getIndex(kv, "numberIndex"), "numberIndex");
    if (booleans)
        kv->set(getIndex(kv, "boolIndex"), "boolIndex");
}

void initIndexForId(std::string id) { index_cache[id] = std::unordered_map<std::string, std::vector<std::string>>(); }
static void setIndexes(MMKV *kv, const string &type, const std::vector<std::string> keys[]) {
    if (!indexing_enabled[kv->mmapID()])
        return;
    auto index = getIndex(kv, type);
    int size = keys->size();
    for (int i = 0; i < size; i++) {
        addValue(index, keys->at(i));
    }
    kv->set(index, type);
}
// setupMMKVInstance
static napi_value setupMMKVInstance(napi_env env, napi_callback_info info) {
    size_t argc = 5;
    napi_value args[5] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto id = NValueToString(env, args[0]);
    if (!id.empty()) {
        auto mode = NValueToInt32(env, args[1]);
        auto cryptKey = NValueToString(env, args[2], true);
        auto path = NValueToString(env, args[3], true);
        auto it =
            find_if(mmkvInstances.begin(), mmkvInstances.end(), [&id](MMKV *inst) { return inst->mmapID() == id; });
        if (it != mmkvInstances.end())

            mmkvInstances.erase(it);
        MMKV *kv = nullptr;
        auto cryptKeyPtr = cryptKey.empty() ? nullptr : &cryptKey;
        auto rootPathPtr = path.empty() ? nullptr : &path;
        MMKVInfo("rootPath: %p, %s, %s", rootPathPtr, path.c_str(), rootPathPtr ? rootPathPtr->c_str() : "");
        MMKVInfo("id: %p, %s, %s", &id, id.c_str(), id.empty() ? id.c_str() : "");
        MMKVInfo("cryptKeyPtr: %p, %s, %s", cryptKeyPtr, cryptKey.c_str(), cryptKeyPtr ? cryptKeyPtr->c_str() : "");
        try {
            MMKV::initializeMMKV(path);
            kv = MMKV::mmkvWithID(id, DEFAULT_MMAP_SIZE, (MMKVMode)mode, cryptKeyPtr, rootPathPtr, 2048);
        } catch (const std::exception &e) {
            std::cerr << "Caught exception: " << e.what() << std::endl;
        }
        mmkvInstances.push_back(kv);
        indexing_enabled[id] = NValueToBool(env, args[4]);
        initIndexForId(id);
    }

    return BoolToNValue(env, true);
}
static napi_value setStringMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto key = NValueToString(env, args[0]);
    auto value = NValueToString(env, args[1]);
    auto id = NValueToString(env, args[2]);
    MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    setIndex(kv, "stringIndex", key);
    kv->set(value, key);
    return BoolToNValue(env, true);
}
static napi_value getStringMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto key = NValueToString(env, args[0]);
    auto id = NValueToString(env, args[1]);
    MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    std::string result;
    bool exists = kv->getString(key, result);
    if (!exists) {
        return NAPIUndefined(env);
    }

    return StringToNValue(env, result);
}
static napi_value setMapMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto key = NValueToString(env, args[0]);
    auto value = NValueToString(env, args[1]);
    auto id = NValueToString(env, args[2]);
    MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    setIndex(kv, "mapIndex", key);
    kv->set(value, key);
    return BoolToNValue(env, true);
}
static napi_value getMapMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto key = NValueToString(env, args[0]);
    auto id = NValueToString(env, args[1]);
    MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    std::string result;
    bool exists = kv->getString(key, result);
    if (!exists) {
        return NAPIUndefined(env);
    }

    return StringToNValue(env, result);
}
static napi_value setArrayMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto key = NValueToString(env, args[0]);
    auto value = NValueToString(env, args[1]);
    auto id = NValueToString(env, args[2]);
    MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    setIndex(kv, "arrayIndex", key);
    kv->set(value, key);
    return BoolToNValue(env, true);
}
static napi_value getArrayMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto key = NValueToString(env, args[0]);
    auto id = NValueToString(env, args[1]);
    MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    std::string result;
    bool exists = kv->getString(key, result);
    if (!exists) {
        return NAPIUndefined(env);
    }

    return StringToNValue(env, result);
}
static napi_value setNumberMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto key = NValueToString(env, args[0]);
    auto value = NValueToDouble(env, args[1]);
    auto id = NValueToString(env, args[2]);
    MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    setIndex(kv, "numberIndex", key);
    kv->set(value, key);
    return BoolToNValue(env, true);
}
static napi_value getNumberMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto key = NValueToString(env, args[0]);
    auto id = NValueToString(env, args[1]);
    MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    bool exists = kv->containsKey(key);
    if (!exists) {
        return NAPIUndefined(env);
    }
    double result = 0;
    double res = kv->getDouble(key, result);
    return DoubleToNValue(env, res);
}
static napi_value setBoolMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto key = NValueToString(env, args[0]);
    auto value = NValueToBool(env, args[1]);
    auto id = NValueToString(env, args[2]);
    MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    setIndex(kv, "boolIndex", key);
    kv->set(value, key);
    return BoolToNValue(env, true);
}
static napi_value getBoolMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto key = NValueToString(env, args[0]);
    auto id = NValueToString(env, args[1]);
    MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    bool result;
    bool exists = kv->containsKey(key);
    if (!exists) {
        return NAPIUndefined(env);
    }

    return BoolToNValue(env, kv->getBool(key));
}
static napi_value removeValueMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto key = NValueToString(env, args[0]);
    auto id = NValueToString(env, args[1]);
    MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    kv->removeValueForKey(key);
    removeFromIndex(kv, key);
    return BoolToNValue(env, true);
}
static napi_value removeValuesMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto keys = NValueToStringArray(env, args[0]);
    auto id = NValueToString(env, args[1]);
    MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    std::vector<std::string> keys_vec = {};
    uint32_t length;
    napi_value result;
    auto keysValue = StringArrayToNValue(env, keys);
    napi_get_array_length(env, keysValue, &length);
    // 创建返回值
    napi_create_uint32(env, length, &result);
    auto res = NValueToInt32(env, result);
    for (int i = 0; i < res; i++) {
        // 获取请求索引位置的元素值并存储在result中
        napi_value result;
        napi_get_element(env, args[0], i, &result);
        auto resElement = NValueToString(env, result);
        keys_vec.push_back(resElement);
    }

    kv->removeValuesForKeys(keys_vec);
    removeKeysFromIndex(kv, keys_vec);
    return BoolToNValue(env, true);
}
static napi_value getAllKeysMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto id = NValueToString(env, args[0]);
    MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    vector<string> keys = kv->allKeys();
    uint32_t length;
    vector<string> keys_vec;
    napi_value result;
    auto keysValue = StringArrayToNValue(env, keys);
    napi_get_array_length(env, keysValue, &length);
    napi_create_uint32(env, length, &result);
    auto res = NValueToInt32(env, result);
    for (int i = 0; i < res; i++) {
        napi_value result;
        napi_get_element(env, keysValue, i, &result);
        auto resElement = NValueToString(env, result);
        keys_vec.push_back(resElement);
    }
    return StringArrayToNValue(env, keys_vec);
}
static napi_value containsKeyMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto key = NValueToString(env, args[0]);
    auto id = NValueToString(env, args[1]);
    MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    if (kv && key.length() > 0) {
        return BoolToNValue(env, kv->containsKey(key));
    }
    return BoolToNValue(env, false);
}
static napi_value clearMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto id = NValueToString(env, args[0]);
    MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    kv->clearAll();
    initIndexForId(kv->mmapID());
    return BoolToNValue(env, true);
}
// TODO: getSecureKey
static napi_value getSecureKey(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    return NAPIUndefined(env);
}
static napi_value getIndexMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto id = NValueToString(env, args[1]);
    auto key = NValueToString(env, args[0]);
    MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    auto keys = getIndex(kv, key);
    auto keysNapi = StringArrayToNValue(env, keys);
    auto size = keys.size();
    napi_value jsArray;
    napi_create_array_with_length(env, size, &jsArray);
    for (int i = 0; i < size; i++) {
        napi_value string;
        napi_get_element(env, keysNapi, i, &string);
        napi_set_element(env, jsArray, static_cast<uint32_t>(i), string);
    }
    return jsArray;
}
static napi_value encryptMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto id = NValueToString(env, args[1]);
    auto cryptKey = NValueToString(env, args[0]);
     MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    auto result = kv->reKey(cryptKey);
    return BoolToNValue(env, result);
}
static napi_value decryptMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto id = NValueToString(env, args[0]);
     MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    kv->reKey("");
    return BoolToNValue(env, true);
}
static napi_value clearMemoryCache(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto id = NValueToString(env, args[0]);
    MMKV *kv = getInstance(id);
    if (!kv) {
        return NAPIUndefined(env);
    }
    kv->clearMemoryCache();
    return BoolToNValue(env, true);
}

static napi_value setMultiMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 4;
    napi_value args[4] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto keys = NValueToStringArray(env, args[0]);
    auto values = NValueToStringArray(env, args[1]);
    auto dataType = NValueToString(env, args[2]);
    auto kvName = NValueToString(env, args[3]);
    auto size = keys.size();
    MMKV *kv = getInstance(kvName);
    if (!kv) {
        return NAPIUndefined(env);
    }
    std::vector<std::string> keysArray = {};
    std::vector<std::string> keysToRemove = {};
    auto keysValue = StringArrayToNValue(env, keys);
    auto value = StringArrayToNValue(env, values);
    for (int i = 0; i < size; i++) {
        napi_value result;
        napi_get_element(env, keysValue, i, &result);
        auto resElement = NValueToString(env, result);
        keysArray.push_back(resElement);
        napi_value isString;
        napi_get_element(env, value, i, &isString);
        napi_valuetype valuetype;
        napi_typeof(env, isString, &valuetype);
        if (valuetype == napi_string) {
            auto value = NValueToString(env, isString);
            kv->set(value, resElement);
        } else {
            if (kv->containsKey(resElement)) {
                keysToRemove.push_back(resElement);
            }
        }
    }
    kv->removeValuesForKeys(keysToRemove);
    removeKeysFromIndex(kv, keysToRemove);
    setIndexes(kv, dataType, &keysArray);
    return BoolToNValue(env, true);
}
static napi_value getMultiMMKV(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr));
    auto keys = NValueToStringArray(env, args[0]);
    auto kvName = NValueToString(env, args[1]);
    auto size = keys.size();

    MMKV *kv = getInstance(kvName);
    if (!kv) {
        return NAPIUndefined(env);
    }
    auto keysValue = StringArrayToNValue(env, keys);
    napi_value jsArray;
    napi_create_array_with_length(env, size, &jsArray);
    for (int i = 0; i < size; i++) {
        napi_value result;
        napi_get_element(env, keysValue, i, &result);
        auto key = NValueToString(env, result);
        MMKVInfo("key: %p, %s, %s", &key, key.c_str(), key.empty() ? key.c_str() : "");
        if (kv->containsKey(key)) {
        std:
            string arrayElement;
            kv->getString(key, arrayElement);
            auto resultString = StringToNValue(env, arrayElement);
            napi_set_element(env, jsArray, static_cast<uint32_t>(i), resultString);
        } else {
            napi_set_element(env, jsArray, static_cast<uint32_t>(i), nullptr);
        }
    }
    return jsArray;
}

EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        {"initialize", nullptr, initialize, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"getDefaultMMKV", nullptr, getDefaultMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"mmkvWithID", nullptr, mmkvWithID, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"setStringMMKV", nullptr, setStringMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"getVector", nullptr, getVector, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"setupMMKVInstance", nullptr, setupMMKVInstance, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"getStringMMKV", nullptr, getStringMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"setMapMMKV", nullptr, setMapMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"getMapMMKV", nullptr, getMapMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"setArrayMMKV", nullptr, setArrayMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"getArrayMMKV", nullptr, getArrayMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"setNumberMMKV", nullptr, setNumberMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"getNumberMMKV", nullptr, getNumberMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"setBoolMMKV", nullptr, setBoolMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"getBoolMMKV", nullptr, getBoolMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"removeValueMMKV", nullptr, removeValueMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"removeValuesMMKV", nullptr, removeValuesMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"getAllKeysMMKV", nullptr, getAllKeysMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"containsKeyMMKV", nullptr, containsKeyMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"clearMMKV", nullptr, clearMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"getIndexMMKV", nullptr, getIndexMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"encryptMMKV", nullptr, encryptMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"decryptMMKV", nullptr, decryptMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"clearMemoryCache", nullptr, clearMemoryCache, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"setMultiMMKV", nullptr, setMultiMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"getMultiMMKV", nullptr, getMultiMMKV, nullptr, nullptr, nullptr, napi_default, nullptr},

    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module mmkvStorageModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "rnoh_mmkv_storage",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterMmkv_storageModule(void) {
    napi_module_register(&mmkvStorageModule);
}
