#include "CConfig.h"

/* 
	Определяем размер массива, чтобы в будущем не бегать по всему файлу и править при смене размера массива.
	Я делаю это через define, т.к. VS подсвечивает макросы, а глобальные константы - нет
*/
#define COLORS_SIZE 7 * 2 * 4 * sizeof(float)

// Определяем конструктор
CConfig::CConfig() {
	// Создаем объект класса CLiteRegeditEasy для работы с регистром
	this->pReg = new CLiteRegeditEasy(HKEY_CURRENT_USER, "Software\\BulletTracerKiN4");
	this->LoadSettings();
}

// Определяем деструктор
CConfig::~CConfig() {
	this->SaveSettings();
	delete this->pReg;
}

void CConfig::LoadSettings() {
	for (int i = 0; i < 14; i++) {
		for (int k = 0; k < 4; k++) {
			char buffer[128];
			sprintf_s(buffer, 128, "fColor[%d][%d]", i, k);
			this->fColors[i][k] = this->pReg->ReadFloat(buffer);
		}
	}
	this->sButtonName = this->pReg->ReadString("sButtonName");
	this->iTracersCount = this->pReg->ReadInteger("iTracersCount");
	this->iTracerTime = this->pReg->ReadInteger("iTracersTime");
	this->iButtonMenuOpen = this->pReg->ReadInteger("iButtonMenuOpen");
	// Если произошла ошибка загрузки, выставляем стандартные значения
	if (pReg->IsError()) {
		this->LoadDefaults();
	}
}

void CConfig::SaveSettings() {
	// Сохраняем все цвета
	// Float сохраняется в HEX коде!
	for (int i = 0; i < 14; i++) {
		for (int k = 0; k < 4; k++) {
			char buffer[128];
			sprintf_s(buffer, 128, "fColor[%d][%d]", i, k);
			this->pReg->WriteFloat(buffer, this->fColors[i][k]);
		}
	}
	// И другие параметры
	this->pReg->WriteString("sButtonName", this->sButtonName.c_str());
	this->pReg->WriteInteger("iTracersCount", this->iTracersCount);
	this->pReg->WriteInteger("iTracersTime", this->iTracerTime);
	this->pReg->WriteInteger("iButtonMenuOpen", this->iButtonMenuOpen);
}

void CConfig::LoadDefaults() {
	/* 
		Ставим все в дефолт
	*/
	memset(this->fColors, 0, COLORS_SIZE);
	this->iButtonMenuOpen = VK_F2;
	this->iTracersCount = 100;
	this->iTracerTime = 120;
	this->sButtonName = "F2";
}