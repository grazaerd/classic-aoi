#include "../state.hpp"
#include "fov.hpp"
#include "polyhook2/PE/IatHook.hpp"
#include "../mem.hpp"

#include <d3dx9math.h>
#include <memory>
#include <numbers>

std::unique_ptr<PLH::IatHook> fovhook;
std::uint64_t fovcb = 0ULL;

namespace FOV {

    D3DXMATRIX* HookedD3DXMatrixPerspectiveFovLH(D3DXMATRIX* pOut, FLOAT fovy, FLOAT Aspect, FLOAT zn, FLOAT zf) {

        static constexpr float calculation = std::numbers::pi_v<float> / 180.0F;

        if (ST::CamFOV) {
            fovy = ST::FOV * calculation;
        }

    	return PLH::FnCast(fovcb, &D3DXMatrixPerspectiveFovLH)(pOut, fovy, Aspect, zn, zf);

    }

    void Hook() {

        uint64_t pFovLH = 0ull;
		const std::string_view FovLHByte = "ff 15 ?? ?? ?? ?? 41 ff c7 48 ff c5"; // to find
		Memory::PatternScan(FovLHByte, pFovLH);
		if (pFovLH != 0ull) {
		    pFovLH = Memory::moduleBase + Memory::FindRVA(Memory::moduleBase, std::bit_cast<void*>(pFovLH), 2, 6);
			Memory::IatMod(std::bit_cast<uint64_t*>(pFovLH), HookedD3DXMatrixPerspectiveFovLH);
		}

    }

}
