#include <Windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <iostream>
#include "graphicsHook.h"
typedef HRESULT(WINAPI* SwapChainPresent_T)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);


SwapChainPresent_T OriginalSwapChainPresent = nullptr;

HRESULT WINAPI HookedSwapChainPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {

	std::cout << "called" << std::endl;
	return OriginalSwapChainPresent(pSwapChain, SyncInterval, Flags);
}

BOOL WINAPI MainThread(HINSTANCE hInstance) {

	FILE* f;

	AllocConsole();
	freopen_s(&f, "CONOUT$", "w", stdout);

	std::cout << "Initializing graphics hook..." << std::endl;

	//this must be called in your main thread
	//initializes grphicsHook and stores the VMT's
	if (!graphicsHook::init()) {
		std::cout << "Failed to initialize graphics hook." << std::endl;
		return FALSE;
	}
	
	std::cout << "Graphics hook initialized successfully." << std::endl;

	//example of hooking the SwapChain Present function
	if (!graphicsHook::hook(SWAP_CHAIN, SWAP_CHAIN_PRESENT, HookedSwapChainPresent, (void**)&OriginalSwapChainPresent)) {
		std::cout << "Failed to hook SwapChain Present." << std::endl;
		return FALSE;
	}

	std::cout << "Graphics hook initialized and SwapChain Present hooked successfully." << std::endl;

	while (!GetAsyncKeyState(VK_ESCAPE)) {
		Sleep(100);
	}

	//WARNING: YOU must manully unkook each function you hooked before exiting the program
	//ghraphicsHook does not do this for you
	//unhooking should be done before a call to FreeLibraryAndExitThread or FreeLibrary

	//example of unhooking the SwapChain Present function
	if (!graphicsHook::hook(SWAP_CHAIN, SWAP_CHAIN_PRESENT, OriginalSwapChainPresent, nullptr)) {
		std::cout << "Failed to unhook SwapChain Present." << std::endl;
	};

	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread(hInstance, 0);

	return TRUE;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved) {
	if (dwReason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(hInstance);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MainThread, hInstance, 0, NULL);
	}
	return TRUE;
}