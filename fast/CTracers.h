#pragma once

#include "game_sa.h"


class CTracers {
private:
	
	// Объявляем в структуре необходимые нам параметры, а именно:
	struct Tracer {
		CVector fOrigin = { 0.0f, 0.0f, 0.0f }; // Откуда летит пуля
		CVector fTarget = { 0.0f, 0.0f, 0.0f }; // Куда летит пуля
		unsigned char eType = 0xff; // в какой тип Entity мы попали (ENTITY_TYPE)
									// 0xff для определения, что в массиве под этим индексом есть трасер
		unsigned char bOwner = 1; // Кто создал пулю
		unsigned long long uLongTicks = 0; // Количество тиков, на которых создалась пуля
	};
	// Массив хранящий структуры трасера
	Tracer tracers[100];

	// Получение координат игровой точки на экране
	bool GetScreenCoords(float& x, float& y, float& z, CVector* vecWorld);
	// Конвертер Float color в D3DCOLOR
	inline DWORD floatToHex(float* color, bool colType = true);

public:
	enum ownerType {
		OWNER_LOCALPLAYER = 0,
		OWNER_NOTLOCALPLAYER = 1
	};

	CTracers();

	~CTracers();

	void Render();
	void AddTracer(CVector origin, CVector target, unsigned char eType, unsigned char bOwner);

	/*
	From IDA Pro
	void __thiscall sub_73B550(CWeapon* this, CEntity* owner, CEntity victim, CVector* startPoint, CVector* endPoint,
								CColPoint* colPoint, int arg5, int useless_0, char useless_1, int useless_2,
								int useless_3, int useless_4, int useless_5)
	*/
	/*
	 Так как DoBulletImpact - функция класса, она имеет тип вызова __thiscall (еще бывают __fastcall; __stdcall (WINAPI);
	 __cdecl (Обычные функции); __declspec (Спец функция с параметрами);
	 (На самом деле это называется соглашение о вызове)
	 __thiscall передает первым параметром указатель на объект класса (typename T* this) (и кладет его в один из регистров)
	 Но адрес __thiscall функции нельзя получить, поэтому мы берем наиболее приближенный к нему вызов __fastcall
	 __fastcall передает аргументы через регистры, что быстрее обычной передачи через стек. 
	 Но если использовать его вместо __thiscall, то у нас появляется бесполезный второй аргумент
	 Поэтому у нас вторым аргументом стоит void* EDX, а не сразу CEntity* owner 
	*/
	static void __fastcall DoBulletImpactHooked(void* weapon, void* EDX, CEntity* owner, CEntity* victim, CVector* startPoint, CVector* endPoint, CColPoint* colPoint, int arg5);

	// Объявляем прототип функции, чтобы потом можно было вызвать через трамплин
	typedef void(__fastcall* DoBulletImpact)(void*, void*, CEntity*, CEntity*, CVector*, CVector*, CColPoint*, int);
};