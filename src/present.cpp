#include "present.hpp"
#include "state.hpp"
#include "util.hpp"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "camera/camera.hpp"
#include "mem.hpp"
#include "config/config.hpp"
#include "offset.hpp"

#include <bit>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace DX11 {

using WNDPROC = LRESULT(STDMETHODCALLTYPE*)(HWND, UINT, WPARAM, LPARAM);

HWND window = nullptr;
WNDPROC oWndProc = nullptr;
ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pContext = nullptr;
ID3D11RenderTargetView* mainRenderTargetView = nullptr;

bool once = false;
bool KeyPress = false;

LRESULT WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) {
	    return true;
	}

	return CallWindowProcA(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool PresentInit(IDXGISwapChain* pSwapChain) {
	if (FAILED(pSwapChain->GetDevice(__uuidof(pDevice), std::bit_cast<void**>(&pDevice)))) { return false; }
	pDevice->GetImmediateContext(&pContext);
	pContext->OMGetRenderTargets(1, &mainRenderTargetView, nullptr);
	DXGI_SWAP_CHAIN_DESC sd;
	pSwapChain->GetDesc(&sd);
	window = sd.OutputWindow;
	if (!mainRenderTargetView) {
		ID3D11Texture2D* pBackBuffer = nullptr;
		if (FAILED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), std::bit_cast<LPVOID*>(&pBackBuffer)))) { return false; }
		pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
		pContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
		pBackBuffer->Release();
		oWndProc = std::bit_cast<WNDPROC>(SetWindowLongPtrA(window, GWLP_WNDPROC, std::bit_cast<LONG_PTR>(&WndProc)));
       	ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(pDevice, pContext);
	}

	return true;
}

void Tab() {
	static constexpr float FloatX = 50.0F;
	static constexpr float FloatY = 30.0F;

	if (ImGui::Button("Camera", ImVec2(FloatX, FloatY))) { ST::tab = 0; }
	ImGui::SameLine();
	if (ImGui::Button("Debug", ImVec2(FloatX, FloatY))) { ST::tab = 1; }
}

void Debug() {
    ImGui::Text("Camera Shake: %s", (ADRS::CamShake ? "Found" : "Not Found"));
    ImGui::Text("Camera Blur: %s", (ADRS::CamBlur ? "Found" : "Not Found"));
    ImGui::Text("Cam Offset 1: %s", (ADRS::CamZoomMountOffOffset ? "Found" : "Not Found"));
    ImGui::Text("Cam Offset 2: %s", (ADRS::CamZoomCurrentOffset ? "Found" : "Not Found"));
    ImGui::Text("Cam Offset 3: %s", (ADRS::CamZoomScrollOffset ? "Found" : "Not Found"));
    ImGui::Text("Cam Offset 4: %s", (ADRS::CamZoomMountOffStatic ? "Found" : "Not Found"));
    ImGui::Text("Cam Detour: %s", (ADRS::CamZoomHook ? "Found" : "Not Found"));
    ImGui::Text("Settings Detour: %s", (ADRS::SystemSettingsCall ? "Found" : "Not Found"));
    ImGui::Text("Settings Args: %s", (ADRS::SystemSettingsArgs ? "Found" : "Not Found"));
    ImGui::Text("Camera FOV Cull: %s", (ADRS::CamFOVCull ? "Found" : "Not Found"));
    ImGui::Text("Socket: %s", (ADRS::SetSocket ? "Found" : "Not Found"));
    ImGui::Text("Connect: %s", (ADRS::Connect ? "Found" : "Not Found"));
    
}

HRESULT HookPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {

	if (unlikely(!pDevice)) {
	    PresentInit(pSwapChain);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	static constexpr auto flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;

	if ((Memory::KeyState(VK_OEM_MINUS) && Memory::KeyState(VK_OEM_PLUS)) && !KeyPress) {
		ST::Show = !ST::Show;
		Config::SaveConfig();
		KeyPress = true;
	}

	if (!(Memory::KeyState(VK_OEM_MINUS) && Memory::KeyState(VK_OEM_PLUS))) { KeyPress = false; }

	if (ST::Show) {
    	if (ImGui::Begin("Classic-aoi", nullptr, flags)) {
            Tab();
            ImGui::Separator();
    	    if (ST::tab == 0) { Cam::GUI(); }
            if (ST::tab == 1) { Debug(); }
    	}
	}


	ImGui::Render();
	if (pContext != nullptr) { pContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr); }
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return pPresent(pSwapChain, SyncInterval, Flags);
}

HRESULT HookResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
	if (mainRenderTargetView != nullptr) {
		mainRenderTargetView->Release();
		mainRenderTargetView = nullptr;
	}
	const HRESULT res = pResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
	ID3D11Texture2D* pBuffer = nullptr;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), std::bit_cast<void**>(&pBuffer));
	if (pBuffer != nullptr) {
		pDevice->CreateRenderTargetView(pBuffer, nullptr, &mainRenderTargetView);
		pBuffer->Release();
		pBuffer = nullptr;
	}
	return res;
}

}
