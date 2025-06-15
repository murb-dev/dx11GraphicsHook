#pragma once

//this is passed to graphicsHook::hook to specify which VMT to hook
enum DX11_VM_TABLE {
	SWAP_CHAIN,
	DEVICE,
	DEVICE_CONTEXT
};

//helper list of indices for swap chain
enum DX11_SWAP_CHAIN_TABLE {
	SWAP_CHAIN_PRESENT = 8,
	SWAP_CHAIN_RESIZE_BUFFERS = 13,
	SWAP_CHAIN_GET_BUFFER = 16
};

//helper list of indices for device
enum DX11_DEVICE_TABLE {
	DEVICE_CREATE_BUFFER = 8,
	DEVICE_CREATE_TEXTURE_2D = 10,
	DEVICE_CREATE_SHADER_RESOURCE_VIEW = 12,
	DEVICE_CREATE_RENDER_TARGET_VIEW = 13,
	DEVICE_CREATE_DEPTH_STENCIL_VIEW = 14
};

//helper list of indices for device context
enum DX11_DEVICE_CONTEXT_TABLE {
	DEVICE_CONTEXT_CLEAR_RENDER_TARGET_VIEW = 42,
	DEVICE_CONTEXT_CLEAR_DEPTH_STENCIL_VIEW = 43,
	DEVICE_CONTEXT_DRAW_INDEXED = 47,
	DEVICE_CONTEXT_DRAW = 48,
	DEVICE_CONTEXT_COPY_RESOURCE = 56,
	DEVICE_CONTEXT_COPY_SUBRESOURCE_REGION = 57,
	DEVICE_CONTEXT_MAP = 60,
	DEVICE_CONTEXT_UNMAP = 61
};


//main api
namespace graphicsHook
{
	/// <summary>
	/// Must be called before any other functions in this namespace.
	/// </summary>
	/// <returns>True if initialization succeeds, false if it fails</returns>
	bool init();

	/// <summary>
	/// Creates a hook at the specified location , see params for details.
	/// </summary>
	/// <param name="table">The VMT to target (SwapChain, Device, or Device Context)</param>
	/// <param name="tableIndex">Index in the VMT where the hook should be placed</param>
	/// <param name="hookFunction">Function pointer to the replacement function</param>
	/// <param name="originalFunction">Output parameter that will store the address of the original function</param>
	/// <returns>True if hook was created successfully, false if it fails</returns>
	bool hook(DX11_VM_TABLE table, int tableIndex, void* hookFunction, void** originalFunction);
} // namespace graphicsHook