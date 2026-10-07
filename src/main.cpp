#include <thread>

#include "camera/camera.hpp"
#include "proxy.hpp"
#include "mem.hpp"
#include "dxport.hpp"
#include "util.hpp"
#include "offset.hpp"
#include "kiero.h"
#include "present.hpp"
#include "camera/fov.hpp"
#include "config/config.hpp"
#include "socket.hpp"

HMODULE ourModule = nullptr;
bool attached = false;

void MainThread() {
	util::HighResSleep(10000); // this is needed for classic

	const OFS ofs;
	SaveByte::SVFunc();
    Config::GenerateConfig();
    Config::GetConfig();

    DXPORT::Initialize();
	FOV::Initialize();
	Cam::Initialize();
	Socket::Initialize();
	
	// bypass gpk crashes

	// void* gpk1 = nullptr;
	// void* gpk2 = nullptr;
	// const char* const gpk1byte = "7e ?? 49 8b f6 48 8b 0c 0e";
	// const char* const gpk2byte = "74 ?? 48 8b 01 ff 50 ?? 33 c0 48 83 c4 ?? c3";
	// Memory::PatternScan(gpk1byte, gpk1);
	// Memory::PatternScan(gpk2byte, gpk2);
	// if (gpk1 != nullptr) {
	// 	Memory::Patch(gpk1, "\xEB", 1);
	// }
	// if (gpk2 != nullptr) {
	// 	Memory::Patch(gpk2, "\xEB", 1);
	// }

    while (!attached) {
        util::HighResSleep(16);
        if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
            attached = true;
            kiero::bind(8, std::bit_cast<void**>(&DX11::pPresent), std::bit_cast<void*>(&DX11::HookPresent));
            kiero::bind(13, std::bit_cast<void**>(&DX11::pResizeBuffers), std::bit_cast<void*>(&DX11::HookResizeBuffers));
        }
    }
}

BOOL DllMain(HMODULE hMod, DWORD dwReason, [[maybe_unused]] LPVOID lpReserved) {
	switch (dwReason) {
		case DLL_PROCESS_ATTACH:
    		K32GetModuleInformation(Memory::hProcess, std::bit_cast<HMODULE>(Memory::moduleBase), &Memory::MODINF, sizeof(MODULEINFO));
			ourModule = hMod;
            Proxy_Attach();
			DisableThreadLibraryCalls(hMod);
			std::thread(MainThread).detach();
			break;
		case DLL_PROCESS_DETACH:
            Proxy_Detach();
			break;
	}
	return TRUE;
}
