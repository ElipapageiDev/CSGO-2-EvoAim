#pragma once
#include <d3d9.h>



namespace gui
{
	
	// constant window size
	constexpr int WIDTH = 634;
	constexpr int HEIGHT = 410;
	
	inline bool isRunning = true;

	inline HWND window = nullptr;
	inline WNDCLASSEX windowClass = { };

	inline POINTS position = { };

	inline PDIRECT3D9 d3d = nullptr;
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline D3DPRESENT_PARAMETERS presentParameters = { };

	
	void CreateHWindow(const char* windowName) noexcept;
	void DestroyHWindow() noexcept;

	
	bool CreateDevice() noexcept;
	void ResetDevice() noexcept;
	void DestroyDevice() noexcept;

	
	void CreateImGui() noexcept;
	void LoadTextures();
	void DestroyImGui() noexcept;

	void BeginRender() noexcept;
	void EndRender() noexcept;
	void Render() noexcept;
}
