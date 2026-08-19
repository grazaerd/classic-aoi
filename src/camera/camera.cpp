#include "camera.hpp"
#include "imgui.h"
#include "../state.hpp"
#include "../offset.hpp"
#include "fov.hpp"
#include "../patch.hpp"
#include <algorithm>
#include <bit>
#include <memory>
#include <type_traits>

#include "polyhook2/Detour/x64Detour.hpp"

using CameraZoomHook = void(STDMETHODCALLTYPE*)();
CameraZoomHook CZH = nullptr;
uint64_t hookcam = 0ull;

float ZoomPreviousValue = 600.0f;
uint64_t ZoomOffset1 = 0u; // BF8 //Scrollable Zoom
uint64_t ZoomOffset2 = 0u; // C24 //Current Zoom
uint64_t ZoomOffset3 = 0u; // 108 //Mount Off Zoom but unknown tbh
uint64_t ZoomMountOffStatic = 0ull;

std::unique_ptr<PLH::x64Detour> CamDetour;

namespace Cam {

    void FuncHT() {
    	asm goto
    		("movss %[ZoomValueAsm], %%xmm3 \n\t"
    		"comiss %[ZoomPreviousValueAsm], %%xmm3 \n\t"
    		"je %l[jmpend] \n\t"
    		"movss %%xmm3, %[ZoomPreviousValueAsm] \n\t"
    	    "add %[ZoomOffset1Asm], %%rdi \n\t"
    		"movss %%xmm3, (%%rdi)  \n\t"
    		"sub %[ZoomOffset1Asm], %%rdi \n\t"
    		"add %[ZoomOffset2Asm], %%rdi \n\t"
    		"movss %%xmm3, (%%rdi) \n\t"
    		"mov %[ZoomMountOffStatic], %%rdi \n\t"
    		"mov (%%rdi), %%rdi \n\t"
    		"add %[ZoomOffset3Asm], %%rdi \n\t"
    		"movss %%xmm3, (%%rdi) \n\t"
    	::
    	[ZoomOffset1Asm] "m" (ZoomOffset1),
    	[ZoomOffset2Asm] "m" (ZoomOffset2),
    	[ZoomOffset3Asm] "m" (ZoomOffset3),
    	[ZoomValueAsm] "m" (ST::CamZoom),
    	[ZoomPreviousValueAsm] "m" (ZoomPreviousValue),
    	[ZoomMountOffStatic] "m" (ZoomMountOffStatic)
    	: "rdi" : jmpend);
    jmpend:
    	return PLH::FnCast(hookcam, CZH)();
    }

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

    void Initialize() {
        if (ADRS::CamZoomScrollOffset != nullptr) {
    		ZoomOffset1 = *std::bit_cast<uint32_t*>(std::bit_cast<uintptr_t>(ADRS::CamZoomScrollOffset) + 4);
    	}
        
    	if (ADRS::CamZoomCurrentOffset != nullptr) {
    		ZoomOffset2 = *std::bit_cast<uint32_t*>(std::bit_cast<uintptr_t>(ADRS::CamZoomCurrentOffset) + 4);
    	}
     
    	if (ADRS::CamZoomMountOffOffset != nullptr) {
    		ZoomOffset3 = *std::bit_cast<uint32_t*>(std::bit_cast<uintptr_t>(ADRS::CamZoomMountOffOffset) + 4);
    	}
     
    	if (ADRS::CamZoomMountOffStatic != nullptr) {
    		ZoomMountOffStatic = Memory::moduleBase + Memory::FindRVA(Memory::moduleBase, ADRS::CamZoomMountOffStatic, 3, 7);
    	}
     
        if (ADRS::CamZoomHook != nullptr) {
            CamDetour = std::make_unique<PLH::x64Detour>(std::bit_cast<uint64_t>(ADRS::CamZoomHook), std::bit_cast<uint64_t>(&FuncHT), &hookcam);
            // fix later
            CamDetour->hook();
        }
    }


}
