#pragma once
#include "d3drender.h"
#include "..\\MinHook.h"

// Объявляем прототипы, по которым потом будем вызывать оригинальные функции
typedef HRESULT(__stdcall *Prototype_Present)(IDirect3DDevice9 *, CONST RECT *, CONST RECT *, HWND, CONST RGNDATA *);
typedef HRESULT(__stdcall *Prototype_Reset)(IDirect3DDevice9 *, D3DPRESENT_PARAMETERS *);

class CD3DHook
{
public:
	// Конструктор
	CD3DHook()
	{
		/* 
			Получаем адреса функций Present и Reset по их номерам в 
			виртуальной таблице методов IDirect3DDevice9
			Найти номера методов можно например здесь:
			https://www.unknowncheats.me/forum/direct3d/66594-d3d9-vtables.html
		*/
		void* hookPrs = reinterpret_cast<void*>(this->GetDeviceAddress(17));
		void* hookRes = reinterpret_cast<void*>(this->GetDeviceAddress(16));
		// Создаем хуки и включаем их
		MH_CreateHook(hookPrs, this->Hooked_Present, reinterpret_cast<LPVOID*>(&this->Orginal_Present));
		MH_EnableHook(hookPrs);
		MH_CreateHook(hookRes, this->Hooked_Reset, reinterpret_cast<LPVOID*>(&this->Orginal_Reset));
		MH_EnableHook(hookRes);
	};
	// Параметры Present
	D3DPRESENT_PARAMETERS pPresentParam;

	CD3DRender *pRender = new CD3DRender(128);
	// Создаем новый Рендер шрифтов с размером шрифты 10
	CD3DFont *pD3DFont = new CD3DFont("Arial", 10, FCR_BORDER);
	bool bD3DRenderInit = false;

	// Трамплины хукнутых функций
	Prototype_Present Orginal_Present;
	Prototype_Reset Orginal_Reset;

	// Прототипы функций для хуков
	static HRESULT __stdcall Hooked_Present(IDirect3DDevice9* pDevice, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion);
	static HRESULT __stdcall Hooked_Reset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentParams);

private:
	// Опять таки инкапсулируем, и выдаем на выход только готовые и удобные для использования функции
	DWORD FindDevice(DWORD Len)
	{
		DWORD dwObjBase = 0;
		char infoBuf[MAX_PATH];
		GetSystemDirectoryA(infoBuf, MAX_PATH);
		strcat_s(infoBuf, MAX_PATH, "\\d3d9.dll");
		dwObjBase = (DWORD) LoadLibraryA(infoBuf);
		while (dwObjBase++ < dwObjBase + Len)
		{
			if ((*(WORD*) (dwObjBase + 0x00)) == 0x06C7 &&
				(*(WORD*) (dwObjBase + 0x06)) == 0x8689 &&
				(*(WORD*) (dwObjBase + 0x0C)) == 0x8689)
			{
				dwObjBase += 2;
				break;
			}
		}
		return(dwObjBase);
	};
	// Находит метод в виртуальной таблице
	DWORD GetDeviceAddress(int VTableIndex)
	{
		PDWORD VTable;
		*(DWORD*) &VTable = *(DWORD*) FindDevice(0x128000);
		return VTable[VTableIndex];
	};
};
