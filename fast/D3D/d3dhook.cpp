#include <windows.h>
#include "d3dhook.h"
#include "..\\dllmain.h"
#include "..\\ImGui\\imgui_impl_dx9.h"

HRESULT __stdcall CD3DHook::Hooked_Present(IDirect3DDevice9* pDevice, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion)
{

	// Здесь необходимые функции хука D3D
	if (!pDevice)
		return pD3DHook->Orginal_Present(pDevice, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);
	if (pD3DHook->bD3DRenderInit == false)
	{
		pD3DHook->pRender->Initialize(pDevice);
		pD3DHook->pD3DFont->Initialize(pDevice);

		pD3DHook->bD3DRenderInit = true;
	}
	// Проверяем, инициализировалась ли игра
	if (*(BYTE*)0xC8D4C0 == 9) {
		static bool inited = false;
		if (!inited) {
			// Инициализируем меню единожды
			pMenu = new CMenu(pDevice);
			inited = true;
		}
		// Рисуем наши штуки
		pMenu->Render();
		pTracers->Render();
	}
	return pD3DHook->Orginal_Present(pDevice, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);
}

HRESULT __stdcall CD3DHook::Hooked_Reset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS *pPresentParams)
{
	// Опять таки необходимые функции D3D хука
	if (!pDevice)
		return pD3DHook->Orginal_Reset(pDevice, pPresentParams);

	if (pD3DHook->bD3DRenderInit == true)
	{
		pD3DHook->pD3DFont->Invalidate();
		pD3DHook->pRender->Invalidate();

		pD3DHook->bD3DRenderInit = false;
	}
	// Проходим валидацию объектов ImGui
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT ret = pD3DHook->Orginal_Reset(pDevice, pPresentParams);

	if (ret == D3D_OK)
	{
		pD3DHook->pRender->Initialize(pDevice);
		pD3DHook->pD3DFont->Initialize(pDevice);
		pD3DHook->pPresentParam = *pPresentParams;

		pD3DHook->bD3DRenderInit = true;
	}
	return ret;
}
