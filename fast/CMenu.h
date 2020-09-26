#pragma once

// Говорим компилятору, что потом определение этой структуры появится
struct IDirect3DDevice9;

class CMenu {
public:
	// Конструктор
	CMenu(IDirect3DDevice9* pDevice);

	bool bMainMnState;
	bool bWaitingPressButton = false;

	void Render();
	void Theme();
};