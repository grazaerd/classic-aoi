#ifndef PATCH_HPP
#define PATCH_HPP

#include <cstdint>
#include <unordered_map>
#include <functional>

#include "state.hpp"
#include "mem.hpp"
#include "offset.hpp"

namespace MN {

    enum ModNames {
        CamShakePatch = 0,
        CamBlurPatch,
        CamFOVParticleCullPatch,
    };
}

/******************** Camera **********************/

inline void CamShakePatch() {
    ST::CamShake ?
    Memory::Patch(ADRS::CamShake, Memory::ret, sizeof(Memory::ret)) :
    Memory::Patch(ADRS::CamShake, static_cast<void*>(&SP::CamShakeSave), sizeof(Memory::ret));
}

inline void CamBlurPatch() {
    ST::CamBlur ?
    Memory::Patch(ADRS::CamBlur, Memory::ret, sizeof(Memory::ret)) :
    Memory::Patch(ADRS::CamBlur, static_cast<void*>(&SP::CamBlurSave), sizeof(Memory::ret));
}

inline void CamFOVParticleCullPatch() {
    ST::CamFOV ?
    Memory::Patch(ADRS::CamFOVCull, Memory::nop_16, sizeof(Memory::nop_16)) :
    Memory::Patch(ADRS::CamFOVCull, static_cast<void*>(&SP::CamFOVCullSave), sizeof(Memory::nop_16));
}



/******************** End **********************/

namespace pch {

    inline std::unordered_map<std::uint8_t, std::function<void()>> patch = {
        { MN::CamShakePatch, []{ CamShakePatch(); } },
        { MN::CamBlurPatch, []{ CamBlurPatch(); } },
        { MN::CamFOVParticleCullPatch, []{ CamFOVParticleCullPatch(); } },

    };
}

#endif
