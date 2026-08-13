#include "camera.hpp"
#include "imgui.h"
#include "../state.hpp"

namespace Cam {


    void GUI() {

        ImGui::Begin("Camera");
        ImGui::Checkbox("Disable Camera Shake", &ST::CamShake);
        ImGui::Checkbox("Disable Camera Blur", &ST::CamBlur);

        ImGui::End();
        
    }
    
    
}