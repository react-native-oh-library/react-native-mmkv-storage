#include "MMKVNativeTurboModule.h"
#include "RNOH/Package.h"

namespace rnoh {

class RNOHMMKVStoragePackageTurboModuleFactoryDelegate : public TurboModuleFactoryDelegate {
  public:
    SharedTurboModule createTurboModule(Context ctx, const std::string &name) const override {
        if (name == "MMKVNative") {
            return std::make_shared<MMKVNativeTurboModule>(ctx,name);
        }
        return nullptr;
    };
};

class RNOHMMKVStoragePackage : public Package {
public:
    RNOHMMKVStoragePackage(Package::Context ctx) : Package(ctx){};

    std::unique_ptr<TurboModuleFactoryDelegate> createTurboModuleFactoryDelegate() override {
        return std::make_unique<RNOHMMKVStoragePackageTurboModuleFactoryDelegate>();
    }

};

}
