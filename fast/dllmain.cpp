#include "dllmain.h"
/* 
	Для хуков будем использовать MinHook
	https://github.com/TsudaKageyu/minhook
*/
#include "MinHook.h"
#pragma comment(lib, "libMinHook-x86-v141-md.lib")

// Определяем экспортнутые указатели

CD3DHook*	pD3DHook;
CConfig*	pConfig;
CMenu*		pMenu;
CTracers*	pTracers;

DWORD WINAPI DllMain(HMODULE hModule, unsigned long ul_reason_for_call, void* lpReserved)
{
    if (ul_reason_for_call ==  DLL_PROCESS_ATTACH)
    {
		// Проверяем название файла, если не совпадает с нашим модулем - выгружаемся
		if (GetModuleHandleA("BulletTracersByKiN4StAt.asi") != hModule)
			return 0; // 0 - Ошибка загрузки, и библиотека отгружается
		// Инициализируем библиотеку хуков
		MH_Initialize();
		/*
			Создаем объекты классов
			(Вызывается конструктор класса Classname() )
		*/
		pConfig		= new CConfig();
		pD3DHook	= new CD3DHook();
		pTracers	= new CTracers();
    }
	// Возвращаем "успех" загрузки Dll
    return TRUE;
}
