#pragma once

#include "RNOH/ArkTSTurboModule.h"

namespace rnoh {

class JSI_EXPORT MMKVNativeTurboModule : public ArkTSTurboModule {
  public:
    MMKVNativeTurboModule(const ArkTSTurboModule::Context ctx, const std::string name);
};

} // namespace rnoh