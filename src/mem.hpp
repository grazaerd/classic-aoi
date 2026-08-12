#ifndef MEM_HPP
#define MEM_HPP

#include <bit>
#include <cstdint>
#include <charconv>
#include <cstring>
#include <format>
#include <processthreadsapi.h>
#include <Psapi.h>
#include <string_view>
#include <TlHelp32.h>
#include <immintrin.h>

#include <synchapi.h>
#include <windef.h>
#include <winbase.h>
#include <winnt.h>
#include <winuser.h>

namespace Memory {

    inline const uintptr_t moduleBase = std::bit_cast<uintptr_t>(GetModuleHandleA(nullptr));

    inline MODULEINFO MODINF;

    inline void* hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());

	template<typename T>
	inline void PatternScan(std::string_view sig, T& address) {
		const uint8_t* start = std::bit_cast<uint8_t*>(moduleBase);
		const std::size_t size = static_cast<std::size_t>(MODINF.SizeOfImage);

		static constexpr auto hexToByte = [](char h, char l) noexcept -> uint8_t {
			auto hexVal = [](char c) noexcept -> uint8_t {
				return static_cast<uint8_t>(
					(c >= '0' && c <= '9') ? (c - '0') :
					(c >= 'A' && c <= 'F') ? (c - 'A' + 10) :
					(c >= 'a' && c <= 'f') ? (c - 'a' + 10) :
					0
				);
			};
			return static_cast<uint8_t>((hexVal(h) << 4) | hexVal(l));
		};

		const char* pat = sig.data();

		for (const uint8_t* p = start; p < start + size; ++p) {
			const uint8_t* mem = p;
			const char* s = pat;

			while (true) {
				if (*s == '\0') { address = reinterpret_cast<T>(const_cast<uint8_t*>(p)); return; }
				if (*s == ' ') { ++s; continue; }
				if (*s == '?') { ++mem; s += (*(s + 1) == '?') ? 2 : 1; continue; }

				if (*mem != hexToByte(s[0], s[1])) break;
				++mem; s += 2;
			}
		}

		if constexpr (std::is_pointer_v<T>) {
			address = nullptr;
		} else {
			address = 0;
		}
	}

	inline uint32_t FindRVA(const uintptr_t& module, const void* Address, const uint8_t MinLen, const uint8_t MaxLen) {
		// https://godbolt.org/z/hKrxf8aEE
		const uint32_t RVA = *std::bit_cast<uint32_t*>(std::bit_cast<uintptr_t>(Address) + MinLen);
		return static_cast<uint32_t>((RVA + static_cast<uint32_t>(std::bit_cast<uintptr_t>(Address) + MaxLen)) - module);
	}

	template <typename Type, typename Func>
	inline void IatMod(Type* address, Func fn) {
		DWORD oldProtect;
		VirtualProtectEx(hProcess, static_cast<void*>(address), sizeof(uint64_t), PAGE_EXECUTE_READWRITE, &oldProtect);
		*address = std::bit_cast<uint64_t>(fn);
		VirtualProtectEx(hProcess, static_cast<void*>(address), sizeof(uint64_t), oldProtect, &oldProtect);
	}

    inline void Patch(void* __restrict dst, const void* __restrict src, std::uint32_t size) {
        if (!dst || !src || size == 0) return;
        DWORD oldProtect;
        VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldProtect);
        std::memcpy(dst, src, size);
        FlushInstructionCache(hProcess, dst, size);
        VirtualProtectEx(hProcess, dst, size, oldProtect, &oldProtect);
    }

	inline const char* btos(const bool membool) {
		if (membool) {
			return "True";
		}
		return "False";
	}

	inline bool stob(std::string_view str) {
		if (str == "True" || str == "true") { // should be using stricmp but meh
			return true;
		} else if (str == "False" || str == "false") {
			return false;
		}
		return false;
	}

	inline std::string ftos(const float memfloat) {
		return std::format("{:.3f}", memfloat);
	}

	inline std::string itos(const int memint) {
		return std::format("{}", memint);
	}

	inline int stoimem(std::string_view str){
		int value = 120;
		std::from_chars(str.data(), str.data() + str.size(), value);
		return value;
	}

	inline float stofmem(std::string_view str){
		float value = 0.0F;
		std::from_chars(str.data(), str.data() + str.size(), value);
		return value;
	}

}

#endif
