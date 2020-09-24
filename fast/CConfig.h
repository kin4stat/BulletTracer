#pragma once
#include "CLiteRegedit.h"

class CConfig {
public:
	float				fColors[7 * 2][4];
	int					iTracerTime;
	int					iTracersCount;
	unsigned char		iButtonMenuOpen;
	std::string			sButtonName;

	CConfig();
	~CConfig();

	void LoadSettings();
	void SaveSettings();
	void LoadDefaults();

private:
	CLiteRegeditEasy* pReg;
};