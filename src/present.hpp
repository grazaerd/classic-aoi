#ifndef PRESENT_HPP
#define PRESENT_HPP

#include <d3d11.h>

namespace DX11 {

using PFN_Present = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
using PFN_ResizeBuffers = HRESULT(STDMETHODCALLTYPE*)(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

inline PFN_Present pPresent = nullptr;
inline PFN_ResizeBuffers pResizeBuffers = nullptr;
    
// LRESULT WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    
HRESULT HookPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    
HRESULT HookResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    
}


#endif