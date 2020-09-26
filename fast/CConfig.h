#pragma once
#include "CLiteRegedit.h"
#include <string>

class CConfig {
public:
	float				fColors[7 * 2][4];
	int					iTracerTime;
	int					iTracersCount;
	unsigned char		iButtonMenuOpen;
	std::string			sButtonName;

	CConfig(); // Конструктор 
	~CConfig(); // Деструктор

	void LoadSettings();
	void SaveSettings();
	void LoadDefaults();

private:
	// Делаем его приватным, т.к. все операции с реестром мы инкапсулируем
	// (Один из принципов ООП)
	// (ООП - Объектно Ориентированное Программирование)
	CLiteRegeditEasy* pReg;
};