#include <Windows.h>
#include <iostream>
#include "CMenu.h"
#include "dllmain.hpp"
#include "patcher.h"

/*
	Темы для ImGui на C++ можно брать тут:
	https://github.com/ocornut/imgui/issues/707
*/

#include "ImGui\\imgui.h"
#include "ImGui\\imgui_impl_win32.h"
#include "ImGui\\imgui_impl_dx9.h"
#include "ImGui\\imgui_internal.h"
#include "ImGui\\imgui_stdlib.h"

// Говорим компилятору, что в будущем такая структура будет определена
struct IDirect3DDevice9;

// Переменная для сохранения оригинального обработчика окна
WNDPROC m_pWindowProc;
/*
	Вытаскиваем обработчик событий окна ImGui
	Чтобы ImGui понимал что мы нажимаем на кнопочки
*/
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void show_cursor(bool show)
{
	if (show) {
		patcher::fill(0x541DF5, 5, 0x90, true); // don't call CControllerConfigManager::AffectPadFromKeyBoard
		patcher::fill(0x53F417, 5, 0x90, true); // don't call CPad__getMouseState
		patcher::setRaw(0x53F41F, "\x33\xC0\x0F\x84", 4, true); // test eax, eax -> xor eax, eax
																// jl loc_53F526 -> jz loc_53F526
		patcher::PutRetn(0x6194A0, true);						// disable RsMouseSetPos (ret)
		ImGui::GetIO().MouseDrawCursor = true;
		(*(IDirect3DDevice9**)0xC97C28)->ShowCursor(true);
		
	}
	else {
		patcher::setRaw(0x541DF5, "\xE8\x46\xF3\xFE\xFF", 5, true); // call CControllerConfigManager::AffectPadFromKeyBoard
		patcher::setRaw(0x53F417, "\xE8\xB4\x7A\x20\x00", 5, true); // call CPad__getMouseState
		patcher::setRaw(0x53F41F, "\x85\xC0\x0F\x8C", 4, true); // xor eax, eax -> test eax, eax
														// jz loc_53F526 -> jl loc_53F526
		patcher::setUChar(0x6194A0, 0xE9, true); // jmp setup
		(*(IDirect3DDevice9**)0xC97C28)->ShowCursor(false);
		ImGui::GetIO().MouseDrawCursor = false;
		//ShowCursor(false);
	}
	(*reinterpret_cast<CMouseControllerState*>(0xB73418)).X = 0.0f;
	(*reinterpret_cast<CMouseControllerState*>(0xB73418)).Y = 0.0f;
	((void(__cdecl*)())(0x541BD0))(); // CPad::ClearMouseHistory
	((void(__cdecl*)())(0x541DD0))(); // CPad::UpdatePads
}

// Сам обработчик событий
auto __stdcall WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)->LRESULT {
	
	static bool buttonNotChanged = true;
	switch (msg)
	{
	// При нажатия на кнопку
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		// Если меню ждет нажатия кнопки
		if (pMenu->bWaitingPressButton) {
			int	iKey = (int)wParam;
			uint32_t ScanCode = LOBYTE(HIWORD(lParam));
			// Проверка на Спец клавиши, нажатия которых передаются в LPARAM
			switch (iKey)
			{
			case VK_SHIFT:
				if (ScanCode == MapVirtualKey(VK_LSHIFT, 0)) pConfig->iButtonMenuOpen = VK_LSHIFT;
				if (ScanCode == MapVirtualKey(VK_RSHIFT, 0)) pConfig->iButtonMenuOpen = VK_RSHIFT;
				break;

			case VK_CONTROL:
				if (ScanCode == MapVirtualKey(VK_LCONTROL, 0)) pConfig->iButtonMenuOpen = VK_LCONTROL;
				if (ScanCode == MapVirtualKey(VK_RCONTROL, 0)) pConfig->iButtonMenuOpen = VK_RCONTROL;
				break;

			case VK_MENU:
				if (ScanCode == MapVirtualKey(VK_LMENU, 0)) pConfig->iButtonMenuOpen = VK_LMENU;
				if (ScanCode == MapVirtualKey(VK_RMENU, 0)) pConfig->iButtonMenuOpen = VK_RMENU;
				break;
			case VK_RETURN:
			case VK_ESCAPE:
				break;
			default:
				pConfig->iButtonMenuOpen = iKey;
				break;
			}
			// Получаем название клавиши
			char TempBuf[64];
			GetKeyNameTextA((MapVirtualKeyA(pConfig->iButtonMenuOpen, MAPVK_VK_TO_VSC) << 16), TempBuf, 64);
			pConfig->sButtonName = TempBuf;
			pMenu->bWaitingPressButton = false;
			buttonNotChanged = false;
			// Выходим из конструкции Switch - Case
			break;
		}
		if (wParam == pConfig->iButtonMenuOpen) {
			pMenu->bMainMnState ^= 1;
		}
		break;
	}

	static bool popen = false;
	// Если не меняем клавишу...
	if (buttonNotChanged) {
		// Проверяем состояние окна
		if (pMenu->bMainMnState)
		{
			// Если окно открыто, но не включена мышь
			if (!popen)
				show_cursor(true);
			popen = true;
			// Вызываем обработчик событий ImGui, чтобы виджеты могли работать
			ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		}
		else
		{
			// Если окно закрыто, но мышь все еще включена
			if (popen)
			{
				popen = false;
				show_cursor(false);
			}
		}
		// Вызываем оригинальный обработчик событий окна
		return CallWindowProcA(m_pWindowProc, hWnd, msg, wParam, lParam);
	}
	// Чтобы не происходило лишних действий пока у нас активен выбор новой клавиши, мы просто говорим цепочке обработчиков событий, что событие успешно обработано
	buttonNotChanged = true;
	return TRUE;
}

CMenu::CMenu(IDirect3DDevice9* pDevice) {
	this->bMainMnState = false;
	// Устанавливаем хук событий окна
	// **(HWND**)0xC17054 -  HWND на главное окно игры, на него мы и вешаем обработчики
	m_pWindowProc = (WNDPROC)SetWindowLongW(**(HWND**)0xC17054, GWL_WNDPROC, (LONG)WndProcHandler);
	// Инициализируем ImGui
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(**(HWND**)0xC17054);
	ImGui::GetIO().MouseDoubleClickTime = 0.8f;
	// Грузим шрифт с кириллическими начертаниями, что вместо русского текста не было знаков вопроса
	ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.TTF", 14.0f, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
	// Применяем тему
	this->Theme();
	ImGui_ImplDX9_Init(pDevice);
}

void CMenu::Render() {
	// Создаем новый фрейм ImGui
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Если окно открыто...
	if (bMainMnState) {
		// Рисуем его...
		ImGui::Begin("Настройки", &this->bMainMnState, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::SliderInt("Время отображения трасера(В секундах)", &pConfig->iTracerTime, 0, 120);
		if (ImGui::SliderInt("Кол-во отображаемых трасеров", &pConfig->iTracersCount, 0, 100)) {
			// Защита от ввода слишком больших чисел через CTRL + ЛКМ
			if (pConfig->iTracersCount > 100 || pConfig->iTracersCount < 0) { pConfig->iTracersCount = 100; }
		}
		if (ImGui::CollapsingHeader("Свои пули")) {
			ImGui::ColorEdit4("Цвет при попадании в игрока", pConfig->fColors[ENTITY_TYPE_PED]);
			ImGui::ColorEdit4("Цвет при попадании в транспорт", pConfig->fColors[ENTITY_TYPE_VEHICLE]);
			ImGui::ColorEdit4("Цвет при попадании в стат. объект", pConfig->fColors[ENTITY_TYPE_BUILDING]);
			ImGui::ColorEdit4("Цвет при попадании в динам. объект", pConfig->fColors[ENTITY_TYPE_OBJECT]);
			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Остальные пули")) {
			ImGui::ColorEdit4("Цвет при попадании кем-то в игрока", pConfig->fColors[ENTITY_TYPE_PED + 7 * CTracers::ownerType::OWNER_NOTLOCALPLAYER]);
			ImGui::ColorEdit4("Цвет при попадании кем-то в транспорт", pConfig->fColors[ENTITY_TYPE_VEHICLE + 7 * CTracers::ownerType::OWNER_NOTLOCALPLAYER]);
			ImGui::ColorEdit4("Цвет при попадании кем-то в стат. объект", pConfig->fColors[ENTITY_TYPE_BUILDING + 7 * CTracers::ownerType::OWNER_NOTLOCALPLAYER]);
			ImGui::ColorEdit4("Цвет при попадании кем-то в динам. объект", pConfig->fColors[ENTITY_TYPE_OBJECT + 7 * CTracers::ownerType::OWNER_NOTLOCALPLAYER]);
			ImGui::Separator();
		}
		if (ImGui::InputText("Выбор кнопки открытия меню", &pConfig->sButtonName, ImGuiInputTextFlags_ReadOnly)) {
			// Если редактирование закончено (на всякий случай)
			this->bWaitingPressButton = false;
		}
		if (ImGui::IsItemClicked()) {
			// Ставим текст InputText;
			pConfig->sButtonName = "< Нажмите любую клавишу (ESC или ENTER для отмены) >";
			// Устанавливаем состояние в меню в ожидание нажатия клавиши
			this->bWaitingPressButton = true;
		}

		// Высчитываем размер текста
		float textSizeSum = ImGui::CalcTextSize("Сохранить настройки").x + ImGui::CalcTextSize("Загрузить настройки").x + ImGui::CalcTextSize("Настройки по-умолчанию").x;
		// Суммируем с переменными стиля и получаем размеры кнопок
		float buttonSizeSum = ImGui::GetStyle().WindowRounding * 6.0f + ImGui::GetStyle().ItemSpacing.x * 3.0f + textSizeSum;
		// Центрируем кнопки
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x - buttonSizeSum);
		if (ImGui::Button("Сохранить настройки")) {
			pConfig->SaveSettings();
		}
		ImGui::SameLine();

		if (ImGui::Button("Загрузить настройки")) {
			pConfig->LoadSettings();
		}
		ImGui::SameLine();

		if (ImGui::Button("Настройки по-умолчанию")) {
			pConfig->LoadDefaults();
		}
		// Заканчиваем отрисовку окна
		ImGui::End();
	}
	// Заканчиваем кадр
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void CMenu::Theme() {
	//ImGui::GetIO().Fonts->AddFontFromFileTTF("../data/Fonts/Ruda-Bold.ttf", 15.0f, &config);
	ImGui::GetStyle().FrameRounding = 4.0f;
	ImGui::GetStyle().GrabRounding = 4.0f;
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}