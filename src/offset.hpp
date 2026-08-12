#ifndef OFFSET_HPP
#define OFFSET_HPP

#include "mem.hpp"

namespace ADRS {
    // Cam
    inline void* CamShake = nullptr;
    inline void* CamBlur = nullptr;

}

// fix this
enum range {
    one = 1, two, three, four, five, twenty = 20
};

namespace SP {
    // Use unsigned char to avoid FFs
    // https://stackoverflow.com/questions/31090616/printf-adds-extra-ffffff-to-hex-print-from-a-char-array

    // Cam
    inline std::array<unsigned char, one> CamShakeSave{}; // Ret
    inline std::array<unsigned char, one> CamBlurSave{}; // Ret

}

class OFS {
public:
    OFS() {
        // Cam
        Memory::PatternScan(CamShake, ADRS::CamShake);
        Memory::PatternScan(CamBlur, ADRS::CamBlur);

    }

private:
    // Cam
    const char* const CamShake = "48 89 5C 24 ?? 57 48 81 EC ?? ?? ?? ?? 0F 29 74 24 ?? 0F 29 7C 24";
    const char* const CamBlur = "48 89 5C 24 ?? 48 89 4C 24 ?? 56 57 41 56 48 83 EC ?? 0F 29 74 24 ?? 41 8B F0";


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
    }
}

#endif
