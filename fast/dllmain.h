#pragma once
#define WIN32_LEAN_AND_MEAN
#pragma execution_character_set("utf-8")
#include <Windows.h>
#include "game_sa.h"
#include "D3D/d3dhook.h"
#include "CTracers.h"
#include "CMenu.h"
#include "CConfig.h"

// Экспортируем указатели, чтобы можно было использовать в других файлах.

extern CConfig*		pConfig;
extern CMenu*		pMenu;
extern CTracers*	pTracers;
extern CD3DHook*	pD3DHook;