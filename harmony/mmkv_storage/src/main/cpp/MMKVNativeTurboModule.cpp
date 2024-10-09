#include "MMKVNativeTurboModule.h"
#include "RNOH/ArkJS.h"
#include <string>
namespace rnoh {
using namespace facebook;

MMKVNativeTurboModule::MMKVNativeTurboModule(const ArkTSTurboModule::Context ctx, const std::string name)
    : ArkTSTurboModule(ctx, name) {

    methodMap_ = {
        // install 方法会给global对象绑定方法
        {"install",
         {0, [](jsi::Runtime &rt, react::TurboModule &turboModule, const jsi::Value *args, size_t count) {
              // 设置全局方法
              auto funcProp1 = jsi::PropNameID::forAscii(rt, "initializeMMKV");
              rt.global().setProperty(
                  rt, funcProp1,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp1, 0,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          // 调用turboModule中 getPublicKeys方法， PublicKeys使用 napi 调用c
                          DLOG(INFO) << "[MMKV]";
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "initializeMMKV", args, 0);
                      }));

              auto funcProp2 = jsi::PropNameID::forAscii(rt, "setupMMKVInstance");
              rt.global().setProperty(
                  rt, funcProp2,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp2, 5,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "setupMMKVInstance", args, 5);
                      }));
              auto funcProp3 = jsi::PropNameID::forAscii(rt, "getSecureKey");
              rt.global().setProperty(
                  rt, funcProp3,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp3, 1,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "getSecureKey", args, 1);
                      }));
              auto funcProp4 = jsi::PropNameID::forAscii(rt, "setSecureKey");
              rt.global().setProperty(
                  rt, funcProp4,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp4, 2,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "setSecureKey", args, 2);
                      }));
              auto funcProp5 = jsi::PropNameID::forAscii(rt, "secureKeyExists");
              rt.global().setProperty(
                  rt, funcProp5,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp5, 1,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "secureKeyExists", args, 1);
                      }));
              auto funcProp6 = jsi::PropNameID::forAscii(rt, "removeSecureKey");
              rt.global().setProperty(
                  rt, funcProp6,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp6, 1,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "removeSecureKey", args, 1);
                      }));
              auto funcProp7 = jsi::PropNameID::forAscii(rt, "setStringMMKV");
              rt.global().setProperty(
                  rt, funcProp7,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp7, 3,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "setStringMMKV", args, 3);
                      }));
              auto funcProp8 = jsi::PropNameID::forAscii(rt, "setMultiMMKV");
              rt.global().setProperty(
                  rt, funcProp8,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp8, 4,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "setMultiMMKV", args, 4);
                      }));
              auto funcProp9 = jsi::PropNameID::forAscii(rt, "getStringMMKV");
              rt.global().setProperty(
                  rt, funcProp9,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp9, 2,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "getStringMMKV", args, 2);
                      }));
              auto funcProp10 = jsi::PropNameID::forAscii(rt, "getMultiMMKV");
              rt.global().setProperty(
                  rt, funcProp10,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp10, 2,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "getMultiMMKV", args, 2);
                      }));
              auto funcProp11 = jsi::PropNameID::forAscii(rt, "setMapMMKV");
              rt.global().setProperty(
                  rt, funcProp11,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp11, 3,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "setMapMMKV", args, 3);
                      }));
              auto funcProp12 = jsi::PropNameID::forAscii(rt, "getMapMMKV");
              rt.global().setProperty(
                  rt, funcProp12,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp12, 2,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "getMapMMKV", args, 2);
                      }));
              auto funcProp13 = jsi::PropNameID::forAscii(rt, "setArrayMMKV");
              rt.global().setProperty(
                  rt, funcProp13,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp13, 3,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "setArrayMMKV", args, 3);
                      }));
              auto funcProp14 = jsi::PropNameID::forAscii(rt, "getArrayMMKV");
              rt.global().setProperty(
                  rt, funcProp14,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp14, 2,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "getArrayMMKV", args, 2);
                      }));
              auto funcProp15 = jsi::PropNameID::forAscii(rt, "setNumberMMKV");
              rt.global().setProperty(
                  rt, funcProp15,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp15, 3,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "setNumberMMKV", args, 3);
                      }));
              auto funcProp16 = jsi::PropNameID::forAscii(rt, "getNumberMMKV");
              rt.global().setProperty(
                  rt, funcProp16,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp16, 2,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "getNumberMMKV", args, 2);
                      }));
              auto funcProp17 = jsi::PropNameID::forAscii(rt, "setBoolMMKV");
              rt.global().setProperty(
                  rt, funcProp17,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp17, 3,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "setBoolMMKV", args, 3);
                      }));
              auto funcProp18 = jsi::PropNameID::forAscii(rt, "getBoolMMKV");
              rt.global().setProperty(
                  rt, funcProp18,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp18, 2,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "getBoolMMKV", args, 2);
                      }));
              auto funcProp19 = jsi::PropNameID::forAscii(rt, "removeValueMMKV");
              rt.global().setProperty(
                  rt, funcProp19,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp19, 2,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "removeValueMMKV", args, 2);
                      }));
              auto funcProp20 = jsi::PropNameID::forAscii(rt, "removeValuesMMKV");
              rt.global().setProperty(
                  rt, funcProp20,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp20, 2,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "removeValuesMMKV", args, 2);
                      }));
              auto funcProp21 = jsi::PropNameID::forAscii(rt, "getAllKeysMMKV");
              rt.global().setProperty(
                  rt, funcProp21,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp21, 1,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "getAllKeysMMKV", args, 1);
                      }));
              auto funcProp22 = jsi::PropNameID::forAscii(rt, "getIndexMMKV");
              rt.global().setProperty(
                  rt, funcProp22,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp22, 2,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "getIndexMMKV", args, 2);
                      }));
              auto funcProp23 = jsi::PropNameID::forAscii(rt, "containsKeyMMKV");
              rt.global().setProperty(
                  rt, funcProp23,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp23, 2,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "containsKeyMMKV", args, 2);
                      }));
              auto funcProp24 = jsi::PropNameID::forAscii(rt, "clearMMKV");
              rt.global().setProperty(
                  rt, funcProp24,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp24, 1,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "clearMMKV", args, 1);
                      }));
              auto funcProp25 = jsi::PropNameID::forAscii(rt, "clearMemoryCache");
              rt.global().setProperty(
                  rt, funcProp25,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp25, 1,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "clearMemoryCache", args, 1);
                      }));
              auto funcProp27 = jsi::PropNameID::forAscii(rt, "encryptMMKV");
              rt.global().setProperty(
                  rt, funcProp27,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp27, 2,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "encryptMMKV", args, 2);
                      }));
              auto funcProp28 = jsi::PropNameID::forAscii(rt, "decryptMMKV");
              rt.global().setProperty(
                  rt, funcProp28,
                  jsi::Function::createFromHostFunction(
                      rt, funcProp28, 1,
                      [&turboModule](jsi::Runtime &rt, const jsi::Value &, const jsi::Value *args,
                                     size_t argc) -> jsi::Value {
                          return static_cast<ArkTSTurboModule &>(turboModule).call(rt, "decryptMMKV", args, 1);
                      }));
              return jsi::Value(true);
          }}}};
}
} // namespace rnoh