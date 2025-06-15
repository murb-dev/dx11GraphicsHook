#include <dxgi.h>
#include <d3d11.h>
#include <Windows.h>
#include <iostream>

#include "graphicsHook.h"
#pragma comment(lib, "d3d11.lib")


//internal namespace to store the VMT pointers
namespace dx11VMT {
	uintptr_t** swapChain = nullptr;
	uintptr_t** device = nullptr;
	uintptr_t** deviceContext = nullptr;
}

//most of this is from msdn
DXGI_SWAP_CHAIN_DESC createSwapChainDescriptor() {
	 DXGI_SWAP_CHAIN_DESC sd;
	 ZeroMemory(&sd, sizeof(sd));
	 sd.BufferCount = 1;
	 sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	 sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	 sd.OutputWindow = GetForegroundWindow();
	 sd.SampleDesc.Count = 1;
	 sd.Windowed = TRUE;
	 sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	 return sd;
}

//swaps the function pointer at the specified index in the VMT with a new function pointer
bool swapVMT(uintptr_t** vmt, int index, void* newFunction, void** originalFunction) {
	 
	 //check if the VMT pointer is valid
	 if (!vmt || !*vmt) return false;
	 if (originalFunction) *originalFunction = (void*)vmt[index];
	 
	
	 DWORD oldProtect;
	 if (!VirtualProtect(&vmt[index], sizeof(uintptr_t),  PAGE_EXECUTE_READWRITE, &oldProtect)) {
		 return false; //failed to change memory protection
	 }
	 vmt[index] = (uintptr_t*)newFunction;

	 VirtualProtect(&vmt[index], sizeof(uintptr_t), oldProtect, &oldProtect);

	 return true;
 }

 //initialize the graphics hook and store the VMT pointers
 //returns true if successful, false otherwise
bool graphicsHook::init() {

	 const D3D_FEATURE_LEVEL featureLevels[2] = {
		 D3D_FEATURE_LEVEL_11_0,
		 D3D_FEATURE_LEVEL_10_0
	 };

	 IDXGISwapChain* swapChain = nullptr;
	 ID3D11Device* device = nullptr;
	 ID3D11DeviceContext* deviceContext = nullptr;

	 DXGI_SWAP_CHAIN_DESC swapChainDesc = createSwapChainDescriptor();

	 //create a dummy device and chain to initialize the VMT pointers
	 HRESULT status = D3D11CreateDeviceAndSwapChain(
		 NULL,
		 D3D_DRIVER_TYPE_HARDWARE,
		 NULL, 
		 0,
		 featureLevels,
		 2,
		 D3D11_SDK_VERSION,
		 &swapChainDesc,
		 &swapChain,
		 &device,
		 nullptr,
		 &deviceContext);

	 //check that everything was created successfully
	 //if any of the pointers are null, we cant proceed
	 if (FAILED(status) || !swapChain || !device || !deviceContext) {
		 return false;
	 }

	 //read the VMT pointers from the created objects
	 //virtual method table pointers are stored at 0x0 of the objects
	 dx11VMT::swapChain = *(uintptr_t***)(swapChain + 0x0);
	 dx11VMT::device = *(uintptr_t***)(device + 0x0);
	 dx11VMT::deviceContext = *(uintptr_t***)(deviceContext + 0x0);
	
	 //release the created objects
	 if (swapChain) swapChain->Release();
	 if (device) device->Release();
	 if (deviceContext) deviceContext->Release();

	 return true;
}


bool graphicsHook::hook(DX11_VM_TABLE table, int tableIndex, void* hookFunction, void** originalFunction)
 {

	 uintptr_t** vmt = nullptr;

	 switch (table) {
	 case DX11_VM_TABLE::DEVICE:
		 vmt = dx11VMT::device;
		 break;
	 case DX11_VM_TABLE::DEVICE_CONTEXT:
		 vmt = dx11VMT::deviceContext;
		 break;
	 case DX11_VM_TABLE::SWAP_CHAIN:
		 vmt = dx11VMT::swapChain;
		 break;
	 default:
		 return false;

	 }

	 return swapVMT(vmt, tableIndex, hookFunction, originalFunction);
}
