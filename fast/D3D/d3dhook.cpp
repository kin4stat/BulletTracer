#include <windows.h>
#include "d3dhook.h"
#include "..\\dllmain.h"
#include "..\\ImGui\\imgui_impl_dx9.h"

HRESULT __stdcall CD3DHook::Hooked_Present(IDirect3DDevice9* pDevice, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion)
{

	// Здесь необходимые функции хука D3D
	if (!pDevice)
		// Если указатель девайс - nullptr, то возвращаем оригинальный Present
		return pD3DHook->Orginal_Present(pDevice, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);
	if (pD3DHook->bD3DRenderInit == false)
	{
		// Если еще не инициализировались - проходим инициализацию
		pD3DHook->pRender->Initialize(pDevice);
		pD3DHook->pD3DFont->Initialize(pDevice);
		// Отмечаем что инициалзировались
		pD3DHook->bD3DRenderInit = true;
	}
	// Проверяем, инициализировалась ли игра
	if (*(BYTE*)0xC8D4C0 == 5) {
		// static означает что переменная останется в памяти до закрытия программы
		static bool inited = false;
		if (!inited) {
			// Инициализируем меню единожды
			pMenu = new CMenu(pDevice);
			// Отмечаем что инициализировались
			inited = true;
		}
	}
	if (*(BYTE*)0xC8D4C0 == 9) {
		// Рисуем наше меню и трасера
		pMenu->Render();
		pTracers->Render();
	}
	// Вызываем оригинальный Present
	return pD3DHook->Orginal_Present(pDevice, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);
}

HRESULT __stdcall CD3DHook::Hooked_Reset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS *pPresentParams)
{
	// Опять таки необходимые функции D3D хука
	if (!pDevice)
		// Если указатель девайс - nullptr, то возвращаем оригинальный Reset
		return pD3DHook->Orginal_Reset(pDevice, pPresentParams);
	// Если отрисовка инициализирована
	if (pD3DHook->bD3DRenderInit == true)
	{
		// Проводим валидацию
		pD3DHook->pD3DFont->Invalidate();
		pD3DHook->pRender->Invalidate();
		// Отмечаем что инициализация под следующий кадр не проведена
		pD3DHook->bD3DRenderInit = false;
	}
	// Проходим валидацию объектов ImGui
	ImGui_ImplDX9_InvalidateDeviceObjects();
	// Вызываем оригинальный Reset
	HRESULT ret = pD3DHook->Orginal_Reset(pDevice, pPresentParams);

	if (ret == D3D_OK)
	{
		// Инициализируем под следующий кадр
		pD3DHook->pRender->Initialize(pDevice);
		pD3DHook->pD3DFont->Initialize(pDevice);
		pD3DHook->pPresentParam = *pPresentParams;
		// Отмечаем что мы готовы рисовать следующий кадр
		pD3DHook->bD3DRenderInit = true;
	}
	return ret;
}
