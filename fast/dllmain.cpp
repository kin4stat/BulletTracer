//#pragma comment(linker, "/MERGE:.data=.text")
//#pragma comment(linker, "/MERGE:.rdata=.text")
//#pragma comment(linker, "/SECTION:.text,EWR")

#include "dllmain.h"
// Для хуков будем использовать MinHook
#include "MinHook.h"
#pragma comment(lib, "libMinHook-x86-v141-md.lib")

// Определяем экспортнутые указатели

CD3DHook*	pD3DHook;
CConfig*	pConfig;
CMenu*		pMenu;
CTracers*	pTracers;


int __stdcall DllMain(HMODULE hModule, unsigned long ul_reason_for_call, void* lpReserved)
{
    if (ul_reason_for_call ==  DLL_PROCESS_ATTACH)
    {
		// Проверяем название файла, если не совпадает с нашим модулем - выгружаемся
		if (GetModuleHandleA("BulletTracersByKiN4StAt.asi") != hModule)
			return 0;
		// Инициализируем библиотеку хуков
		MH_Initialize();
		// Создаем объекты классов
		pConfig		= new CConfig();
		pD3DHook	= new CD3DHook();
		pTracers	= new CTracers();
    }
    return 1;
}
