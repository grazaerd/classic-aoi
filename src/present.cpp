#include "present.hpp"
#include "state.hpp"
#include "util.hpp"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "camera/camera.hpp"
#include "mem.hpp"
#include "config/config.hpp"

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
HRESULT HookPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {

	if (unlikely(!pDevice)) {
	    PresentInit(pSwapChain);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	static constexpr auto flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNav;
	ImGui::SetNextWindowSize(ImVec2(250.0f, 150.0f), ImGuiCond_Always);

	if ((Memory::KeyState(VK_OEM_MINUS) && Memory::KeyState(VK_OEM_PLUS)) && !KeyPress) {
		ST::Show = !ST::Show;
		Config::SaveConfig();
		KeyPress = true;
	}

	if (!(Memory::KeyState(VK_OEM_MINUS) && Memory::KeyState(VK_OEM_PLUS))) { KeyPress = false; }

	if (ST::Show) {
    	if (ImGui::Begin("Classic-aoi", nullptr, flags)) {

    	    Cam::GUI();

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
