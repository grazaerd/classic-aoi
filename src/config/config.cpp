
#include "config.hpp"
#include "../mem.hpp"
#include "../state.hpp"
#include "../patch.hpp"
#define MINI_CASE_SENSITIVE
#include "mini/ini.h"
#include <filesystem>


namespace Config {

    // change bool later
    void GenerateConfig() {
        if (!std::filesystem::exists("dnclassic.ini")) {
            const mINI::INIFile file("dnclassic.ini");
            mINI::INIStructure ini;
            ini["Camera Mod"]["Camera Shake"] = "False";
            ini["Camera Mod"]["Camera Blur"] = "False";
            ini["Camera Mod"]["Modify FOV"] = "False";
            ini["Camera Mod"]["Camera FOV"] = Memory::ftos(ST::FOV);

            ini["Misc"]["Show GUI"] = "False";

            file.generate(ini, true);
        }
    }

    void SaveConfig() {
        const mINI::INIFile file("dnclassic.ini");
        mINI::INIStructure ini;
        ini["Camera Mod"]["Camera Shake"] = Memory::btos(ST::CamShake);
        ini["Camera Mod"]["Camera Blur"] = Memory::btos(ST::CamBlur);
        ini["Camera Mod"]["Modify FOV"] = Memory::btos(ST::CamFOV);

        ini["Camera Mod"]["Camera FOV"] = Memory::ftos(ST::FOV);

        ini["Misc"]["Show GUI"] = Memory::btos(ST::Show);


        file.write(ini, true);
    }

    void GetConfig() {
        const mINI::INIFile file("dnclassic.ini");
        mINI::INIStructure ini;
        if (file.read(ini)) {

            ST::CamShake = Memory::stob(ini["Camera Mod"]["Camera Shake"]);
            ST::CamBlur = Memory::stob(ini["Camera Mod"]["Camera Blur"]);
            ST::CamFOV = Memory::stob(ini["Camera Mod"]["Modify FOV"]);
            ST::FOV = Memory::stofmem(ini["Camera Mod"]["Camera FOV"]);
            ST::FOV = std::clamp(ST::FOV, ST::FOVmin, ST::FOVmax);

            ST::Show = Memory::stob(ini["Misc"]["Show GUI"]);

        }

        for (std::uint8_t i = 0; i != pch::patch.size(); i++) { pch::patch.at(i)(); }
    }

}
