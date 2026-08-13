#include "present.hpp"
#include "util.hpp"

using Present = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
using ResizeBuffers = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
using WNDPROC = LRESULT(STDMETHODCALLTYPE*)(HWND, UINT, WPARAM, LPARAM);

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
Present oPresent = nullptr;
ResizeBuffers oResizeBuffers = nullptr;
HWND window = nullptr;
WNDPROC oWndProc = nullptr;
ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pContext = nullptr;
ID3D11RenderTargetView* mainRenderTargetView = nullptr;

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
	}

	return true;
}

HRESULT HookPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {

    if (unlikely(mainRenderTargetView == 0)) {
        PresentInit(pSwapChain);
    }


	// ImGui_ImplDX11_NewFrame();
	// ImGui_ImplWin32_NewFrame();
	// ImGui::NewFrame();



	// ImGui::Render();
	// if (pContext != nullptr) { pContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr); }
	// if (initRelease) { return 0; }
	// ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	// if (kiero::getRenderType() != kiero::RenderType::Vulkan) {
	//     SyncInterval = 0;
	//     Flags = DXGI_PRESENT_ALLOW_TEARING;
	// }
	return oPresent(pSwapChain, SyncInterval, Flags);
}

HRESULT HookResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
	if (mainRenderTargetView != nullptr) {
		mainRenderTargetView->Release();
		mainRenderTargetView = nullptr;
	}
	const HRESULT res = oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
	ID3D11Texture2D* pBuffer = nullptr;
	pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), std::bit_cast<void**>(&pBuffer));
	if (pBuffer != nullptr) {
		pDevice->CreateRenderTargetView(pBuffer, nullptr, &mainRenderTargetView);
		pBuffer->Release();
		pBuffer = nullptr;
	}
	return res;
}