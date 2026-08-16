#include "camera.hpp"
#include "imgui.h"
#include "../state.hpp"
#include "../offset.hpp"
#include "fov.hpp"
#include "../patch.hpp"
#include <algorithm>


namespace Cam {


    void GUI() {

        if (ImGui::Checkbox("Disable Camera Shake", &ST::CamShake)) {
            pch::patch.at(MN::CamShakePatch)();
		}

	    if (ImGui::Checkbox("Disable Radial Blur", &ST::CamBlur)) {
			pch::patch.at(MN::CamBlurPatch)();
		}

		if (ImGui::Checkbox("Modify FOV", &ST::CamFOV)) {
		    pch::patch.at(MN::CamFOVParticleCullPatch)();
		    goto settings;
		}

		if (ST::CamFOV) {
       		if (ImGui::SliderFloat("Camera FOV", &ST::FOV, ST::FOVmin, ST::FOVmax, "%.0f")) {
                ST::FOV = std::clamp(ST::FOV, ST::FOVmin, ST::FOVmax);
            settings:
                if (ADRS::SystemSettingsCall != nullptr && ADRS::SystemSettingsArgs != nullptr) {
                    if (Memory::RWPtr(std::bit_cast<uintptr_t*>(FOV::SSCArgs))) {
    					std::uint64_t dp_SSCArgs = *std::bit_cast<std::uint64_t*>(FOV::SSCArgs);
    					if (Memory::RWPtr(std::bit_cast<uintptr_t*>(dp_SSCArgs))) {
    						FOV::SSC(dp_SSCArgs);
    					}
    				}
                }
       		}
		}
		// Unimplemented
		// ImGui::PushItemWidth(75);
		// ImGui::AlignTextToFramePadding();
		// ImGui::Text("Camera Zoom:");
		// ImGui::SameLine();
		// ImGui::InputFloat(" ", &ST::CamZoom, 0, 0, "%.0f", ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_CharsNoBlank);
		// ImGui::PopItemWidth();

    }


}
