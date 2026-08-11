#ifndef STATE_HPP
#define STATE_HPP

namespace ST {

    inline bool CamShake = false;
    inline bool CamBlur = false;

    inline bool CamFOV = false; 
    inline float FOV = 45.0f;
    inline float FOVmin = 10.0f;
    inline float FOVmax = 200.0f;
    
    inline float CamZoom = 600.0f; // sea value: 430.0f lowest and 600.0f max    
    
}

#endif