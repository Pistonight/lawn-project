#pragma once

#include <Piston/Upscaler.h>

namespace Piston {

class LawnAppMixinState {
public:
    UpscaleMode mUpscaleMode = UpscaleMode::None;
};

} // namespace Piston
