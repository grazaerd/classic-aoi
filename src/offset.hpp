#ifndef OFFSET_HPP
#define OFFSET_HPP

#include "mem.hpp"
#include <string_view>

namespace ADRS {
    // Cam
    inline void* CamShake = nullptr;
    inline void* CamBlur = nullptr;
    inline void* CamZoomMountOffOffset = nullptr;
    inline void* CamZoomCurrentOffset = nullptr;
    inline void* CamZoomScrollOffset = nullptr;
    inline void* CamZoomMountOffStatic = nullptr;
    inline void* CamZoomHook = nullptr;

    // FOV
    inline void* SystemSettingsCall = nullptr;
    inline void* SystemSettingsArgs = nullptr;
    inline void* CamFOVCull = nullptr;

}

// fix this
enum range {
    one = 1, two, three, four, five, sixteen = 16, twenty = 20
};

namespace SP {
    // Use unsigned char to avoid FFs
    // https://stackoverflow.com/questions/31090616/printf-adds-extra-ffffff-to-hex-print-from-a-char-array

    // Cam
    inline std::array<unsigned char, one> CamShakeSave{}; // Ret
    inline std::array<unsigned char, one> CamBlurSave{}; // Ret

    inline std::array<unsigned char, sixteen> CamFOVCullSave{}; // Ret

}

class OFS {
public:
    OFS() {
        // Cam
        Memory::PatternScan(CamShake, ADRS::CamShake);
        Memory::PatternScan(CamBlur, ADRS::CamBlur);
        Memory::PatternScan(CamZoomMountOffOffset, ADRS::CamZoomMountOffOffset);
        Memory::PatternScan(CamZoomCurrentOffset, ADRS::CamZoomCurrentOffset);
        Memory::PatternScan(CamZoomScrollOffset, ADRS::CamZoomScrollOffset);
        Memory::PatternScan(CamZoomMountOffStatic, ADRS::CamZoomMountOffStatic);
        Memory::PatternScan(CamZoomHook, ADRS::CamZoomHook);

        // FOV
        Memory::PatternScan(SystemSettingsCall, ADRS::SystemSettingsCall);
        Memory::PatternScan(SystemSettingsArgs, ADRS::SystemSettingsArgs);
        Memory::PatternScan(CamFOVCull, ADRS::CamFOVCull);


    }

private:
    // Cam
    const std::string_view CamShake = "40 56 48 83 ec ?? 48 8b f1 48 8b 49";
    const std::string_view CamBlur = "74 ?? F3 0F 10 49 ?? C6 05";
    const std::string_view CamZoomMountOffOffset = "F3 0F 10 88 ?? ?? ?? ?? 0F 2F C8 72"; //150
    const std::string_view CamZoomMountOffStatic = "48 8B 05 ?? ?? ?? ?? F3 0F 10 88 ?? ?? ?? ?? 0F 2F C8 72"; //150
    const std::string_view CamZoomCurrentOffset = "F3 0F 11 8F ?? ?? ?? ?? 44 89 AF"; //C34
    const std::string_view CamZoomScrollOffset = "f3 0f 10 8f ?? ?? ?? ?? f3 0f 5e 05"; //C08
    const std::string_view CamZoomHook = "f3 0f 10 9f ?? ?? ?? ?? 0f 28 cb";

    // FOV
    const std::string_view SystemSettingsCall = "40 53 48 83 ec ?? 83 79 ?? ?? 48 8b d9 c7 41";
    const std::string_view SystemSettingsArgs = "48 8b 1d ?? ?? ?? ?? 48 89 b4 24";
    const std::string_view CamFOVCull = "48 8B CB E8 ?? ?? ?? ?? 84 C0 0F 85 ?? ?? ?? ?? 48 8B 4B ?? 48 85 C9";
};

template <typename T, uint64_t size>
class SavePattern {
public:
	SavePattern() = default;
	SavePattern(std::array<T, size>* t_Values, void* t_Address)
		: m_Values(t_Values),
		m_Address(t_Address) {

		SaveScan(m_Values, m_Address);
	}

private:
	static void SaveScan(std::array<T, size>* values, void* address);

	std::array<T, size>* m_Values{};
	void* m_Address = nullptr;

};

template <typename T, uint64_t size>
void SavePattern<T, size>::SaveScan(std::array<T, size>* values, void* address) {
    if (address != nullptr) {
	    std::copy(static_cast<T*>(address), static_cast<T*>(address) + values->size(), values->begin());
    }
    return;
}

namespace SaveByte {
    inline void SVFunc() {
        // Cam
        SavePattern CamShake(&SP::CamShakeSave, ADRS::CamShake);
        SavePattern CamBlur(&SP::CamBlurSave, ADRS::CamBlur);

        // FOV
        SavePattern CamFOVCull(&SP::CamFOVCullSave, ADRS::CamFOVCull);
    }
}

#endif
