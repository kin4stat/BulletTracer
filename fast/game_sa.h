#pragma once

typedef unsigned long DWORD;

class CMouseControllerState {
public:
	unsigned char lmb;
	unsigned char rmb;
	unsigned char mmb;
	unsigned char wheelUp;
	unsigned char wheelDown;
	unsigned char bmx1;
	unsigned char bmx2;
	char __align;
	float Z;
	float X;
	float Y;
};

enum eEntityType
{
	ENTITY_TYPE_NOTHING,
	ENTITY_TYPE_BUILDING,
	ENTITY_TYPE_VEHICLE,
	ENTITY_TYPE_PED,
	ENTITY_TYPE_OBJECT,
	ENTITY_TYPE_DUMMY,
	ENTITY_TYPE_NOTINPOOLS
};

struct CVector
{
	float x, y, z;
};

struct CEntity{
	char			CPlaceable[24];
	char			field[30];
	unsigned char	m_nType : 3; // see eEntityType
	unsigned char	m_nStatus : 5; // see eEntityStatus
};

struct tColLighting
{
	unsigned char day : 4;
	unsigned char night : 4;
};

class CColPoint {
public:
	CVector       m_vecPoint;
	float field_C;
	CVector       m_vecNormal;
	float field_1C;
	// col shape 1 info
	unsigned char m_nSurfaceTypeA; // see eSurfaceType
	unsigned char m_nPieceTypeA;
	tColLighting m_nLightingA;
private:
	char _pad;
public:
	// col shape 2 info
	unsigned char m_nSurfaceTypeB; // see eSurfaceType
	unsigned char m_nPieceTypeB;
	tColLighting m_nLightingB;
private:
	char _pad2;
public:
	// col depth
	float         m_fDepth;

	void operator=(CColPoint const& right);
};

bool ProcessLineOfSight(CVector const& origin, CVector const& target, CColPoint& outColPoint, CEntity*& outEntity, bool buildings, bool vehicles, bool peds, bool objects, bool dummies, bool doSeeThroughCheck, bool doCameraIgnoreCheck, bool doShootThroughCheck);