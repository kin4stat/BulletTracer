#pragma once

struct IDirect3DDevice9;

class CMenu {
private:
	

public:
	CMenu(IDirect3DDevice9* pDevice);

	bool bMainMnState;
	bool bWaitingPressButton = false;

	void Render();
	void Theme();
};