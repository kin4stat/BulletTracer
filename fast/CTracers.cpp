#include "CTracers.h"
#include "dllmain.hpp"
#include "MinHook.h"
#include <stdio.h>

// Указатель на трамплин
// Трамлин - штука которую можно вызвать, и тогда выполнится затертая хуком часть, и продолжится выполнение оригинальной функции
static CTracers::DoBulletImpact fpDoImpact = 0;

class CWeapon {
public:
	char field;

	void __thiscall DoBulletImpactHooked(void* weapon, void* EDX, CEntity* owner, CEntity* victim, CVector* startPoint, CVector* endPoint, CColPoint* colPoint, int arg5);
};

// Конструктор
CTracers::CTracers() {
	// Устанавливаем хук на функцию CWeapon::DoBulletImpact
	MH_CreateHook((void*)0x73B550, &pTracers->DoBulletImpactHooked, reinterpret_cast<LPVOID*>(&fpDoImpact));
	// Включаем хук
	MH_EnableHook((void*)0x73B550);
}

// Деструктор
CTracers::~CTracers() {
	MH_DisableHook((void*)0x726AF0);
}

// Получает экранные координаты объекта по игровым
// Возвращает, на экране ли объект
bool CTracers::GetScreenCoords(float& x, float& y, float& z, CVector* vecWorld)
{
	D3DXVECTOR3 vecScreen;

	D3DXMATRIX    m((float*)(0xB6FA2C));
	DWORD* dwLenX = (DWORD*)(0xC17044);
	DWORD* dwLenY = (DWORD*)(0xC17048);

	vecScreen.x = (vecWorld->z * m._31) + (vecWorld->y * m._21) + (vecWorld->x * m._11) + m._41;
	vecScreen.y = (vecWorld->z * m._32) + (vecWorld->y * m._22) + (vecWorld->x * m._12) + m._42;
	vecScreen.z = (vecWorld->z * m._33) + (vecWorld->y * m._23) + (vecWorld->x * m._13) + m._43;

	double    fRecip = (double)1.0 / vecScreen.z;
	vecScreen.x *= (float)(fRecip * (*dwLenX));
	vecScreen.y *= (float)(fRecip * (*dwLenY));

	x = vecScreen.x;
	y = vecScreen.y;
	z = vecScreen.z;
	return vecScreen.z >= 1.0f;
}

// Переводит float[3/4] (ImGui) в D3DCOLOR (d3d9hook)
DWORD CTracers::floatToHex(float* color, bool colType) {
	if (colType)
		return D3DCOLOR_ARGB(static_cast<int>(color[3] * 255.0f), static_cast<int>(color[0] * 255.0f), static_cast<int>(color[1] * 255.0f), static_cast<int>(color[2] * 255.0f));
	return D3DCOLOR_XRGB(static_cast<int>(color[0] * 255.0f), static_cast<int>(color[1] * 255.0f), static_cast<int>(color[2] * 255.0f));
}

// Рендер трасеров
void CTracers::Render() {
	
	for (int i = 0; i < pConfig->iTracersCount; i++) {
		float originX, originY, originZ, targetX, targetY, targetZ;

		// Получаем координаты точек начала и конца трасера для дальнейшего рисования
		bool resultOrigin = this->GetScreenCoords(originX, originY, originZ, &this->tracers[i].fOrigin);
		bool resultTarget = this->GetScreenCoords(targetX, targetY, targetZ, &this->tracers[i].fTarget);

		if (tracers[i].eType != 0xff) {
			if (resultOrigin && resultTarget) {
				// Проверяем, сколько секунд назад был создан трасер
				if (GetTickCount64() - tracers[i].uLongTicks < pConfig->iTracerTime * 1000) {
					// Рисуем Линию, цвет берем из конфига и конвертируем его из float [4] в D3DCOLOR(ARGB)
					pD3DHook->pRender->Line(originX, originY, targetX, targetY,
						this->floatToHex(pConfig->fColors[this->tracers[i].eType + 7 * this->tracers[i].bOwner]));
				}
			}
		}
		else {
			/*
				Можем выйти из цикла, т.к. мы нашли пустой элемент,
				А массив заполняется последовательно
			*/
			break;
		}
	}
}
// Смещает массив с трасерами и заменяет первый трасер новым
void CTracers::AddTracer(CVector origin, CVector target, unsigned char eType, unsigned char bOwner) {
	// Сдвигаем массив
	for (int i = 99; i > 0; i--) {
		tracers[i] = tracers[i - 1];
	}
	// Просто по приколу проверяем
	if (eType != ENTITY_TYPE_NOTHING) {
		// После смещения на 1, ставим в данные первого элемента
		// Данные нового трасера
		tracers[0].eType = eType;
		tracers[0].fOrigin = origin;
		tracers[0].fTarget = target;
		tracers[0].uLongTicks = GetTickCount64();
		tracers[0].bOwner = bOwner;
	}
}

// Собственно сама функция хука, чтобы узнать почему здесь __fastcall и непонятный аргумент void* EDX - смотреть CTracers.h
void __fastcall CTracers::DoBulletImpactHooked(void* weapon, void* EDX, CEntity* owner, CEntity* victim, CVector* startPoint, CVector* endPoint, CColPoint* colPoint, int arg5) {
	// Проверяем, попала ли пуля куда-то, иначе нас крашнет
	if (victim != nullptr) {
		// Проверяем кто выпустил пулю игрок, или другой пед
		unsigned char own = (owner == *(CEntity**)0xB6F5F0) ? CTracers::ownerType::OWNER_LOCALPLAYER: CTracers::ownerType::OWNER_NOTLOCALPLAYER;
		// Добавляем трасер
		pTracers->AddTracer(*startPoint, colPoint->m_vecPoint, victim->m_nType, own);
	}
	// Вызываем оригинальную функцию, чтобы не крашнуло
	return fpDoImpact(weapon, EDX, owner, victim, startPoint, endPoint, colPoint, arg5);
}