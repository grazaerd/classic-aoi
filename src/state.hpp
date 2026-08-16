#ifndef STATE_HPP
#define STATE_HPP

#include <algorithm>
namespace ST {

    inline bool CamShake = false;
    inline bool CamBlur = false;

    inline bool CamFOV = false;
    inline constexpr float FOVmin = 20.0f;
    inline constexpr float FOVmax = 145.0f;
    inline float FOV = std::clamp(45.0f, FOVmin, FOVmax);

    inline float CamZoom = 600.0f; // sea value: 430.0f lowest and 600.0f max
    inline bool Show = false; // GUI
}

#endif
