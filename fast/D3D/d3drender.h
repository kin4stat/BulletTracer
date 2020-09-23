/*
	SA:MP ASI Framework
	Author: CentiuS
*/

#ifndef _D3DRENDER_H
#define _D3DRENDER_H
#include <d3d9.h>
#include "d3dx9.h"
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include <cstdint>

class	CD3DFont;
class	CD3DRender;

#define FCR_NONE	0x0
#define FCR_BOLD	0x1
#define FCR_ITALICS 0x2
#define FCR_BORDER	0x4

//FVF Macros
#define D3DFVF_BITMAPFONT	( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define D3DFVF_PRIMITIVES	( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )

typedef struct d3dvertex_s
{
	d3dvertex_s () {}
	d3dvertex_s(float _x, float _y, D3DCOLOR _color)
	{
		x = _x;
		y = _y;
		z = 0;
		rhw = 0;
		color = _color;
	}
	d3dvertex_s(float _x, float _y, float _z, float _rhw, D3DCOLOR _color, float _tu, float _tv)
	{
		x = _x;
		y = _y;
		z = _z;
		color = _color;
		rhw = _rhw;
		tu = _tu;
		tv = _tv;
	}
	float	x, y, z, rhw;
	D3DCOLOR color;
	float	tu, tv;
} d3dvertex_s;
//Initialization Macros
#undef SAFE_RELEASE
#define SAFE_RELEASE( d ) \
	if ( d ) \
	{ \
		d->Release(); \
		d = NULL; \
	}

#define D3DFVF_XYZ			0x002
#define D3DFVF_RESERVED1	0x020
#define D3DFVF_DIFFUSE		0x040
#define D3DFVF_SPECULAR		0x080
#define D3DFVF_TEX1			0x100
#define D3DFVF_LVERTEX		( D3DFVF_XYZ | D3DFVF_RESERVED1 | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1 )

typedef float	D3DVALUE, *LPD3DVALUE;

typedef struct D3DLVERTEX
{
	union
	{
		D3DVALUE	x;			/* Homogeneous coordinates */
		D3DVALUE	dvX;
	};
	union
	{
		D3DVALUE	y;
		D3DVALUE	dvY;
	};
	union
	{
		D3DVALUE	z;
		D3DVALUE	dvZ;
	};
	DWORD	dwReserved;
	union
	{
		D3DCOLOR	color;		/* Vertex color */
		D3DCOLOR	dcColor;
	};
	union
	{
		D3DCOLOR	specular;	/* Specular component of vertex */
		D3DCOLOR	dcSpecular;
	};
	union
	{
		D3DVALUE	tu;			/* Texture coordinates */
		D3DVALUE	dvTU;
	};
	union
	{
		D3DVALUE	tv;
		D3DVALUE	dvTV;
	};
} D3DLVERTEX, *LPD3DLVERTEX;

class CD3DBaseRender
{
public:
	CD3DBaseRender ();
	~				CD3DBaseRender ();

	static HRESULT	BeginRender ();
	static HRESULT	EndRender ();
protected:
	static HRESULT				Initialize ( IDirect3DDevice9 *pD3Ddev );
	static HRESULT				Invalidate ();

	static HRESULT				CreateStates ();
	static HRESULT				DeleteStates ();

	static IDirect3DDevice9		*m_pD3Ddev;
	static IDirect3DStateBlock9 *m_pD3DstateDraw;
	static IDirect3DStateBlock9 *m_pD3DstateNorm;

	static int					m_renderCount;
	static int					m_numShared;
	static bool					m_statesOK;
};

class CD3DFont : public CD3DBaseRender
{
public:
	CD3DRender	*m_pRender;

	CD3DFont ( const char *szFontName, int fontHeight, DWORD dwCreateFlags );
	~			CD3DFont ();

	HRESULT		Initialize ( IDirect3DDevice9 *pD3Ddev );
	HRESULT		Invalidate ();

	HRESULT		Print ( float x, float y, DWORD color, const char *szText, bool isShadow = false );
	HRESULT		PrintShadow ( float x, float y, DWORD color, const char *szText );

	float		DrawLength ( const char * ) const;
	float DrawHeight () const
	{
		return m_fChrHeight;
	};
private:
	HRESULT					CreateVertexBuffers ();
	HRESULT					DeleteVertexBuffers ();

	char					m_szFontName[31 + 1];
	int						m_fontHeight;
	DWORD					m_dwCreateFlags;

	bool					m_isReady;

	IDirect3DTexture9		*m_pD3Dtex;
	IDirect3DVertexBuffer9	*m_pD3Dbuf;

	DWORD					m_maxTriangles;

	int						m_texWidth, m_texHeight;
	int						m_chrSpacing;
	float					m_fTexCoords[224][4];
	float					m_fChrHeight;
};



class CD3DRender : public CD3DBaseRender
{
public:
	CD3DRender ( int numVertices );
	~		CD3DRender ();

	HRESULT Initialize ( IDirect3DDevice9 *pD3Ddev );
	HRESULT Invalidate ();

	HRESULT Begin ( D3DPRIMITIVETYPE primType );
	HRESULT End ();

	HRESULT D3DColor ( DWORD color );
	void	D3DBindTexture ( IDirect3DTexture9 * );
	void	D3DTexCoord2f ( float u, float v );
	HRESULT D3DVertex2f ( float x, float y );
	void	D3DTexQuad ( float sx, float sy, float ex, float ey, float su, float sv, float eu, float ev );
	void	D3DBox ( float x, float y, float w, float h, D3DCOLOR color );
	void	D3DBoxi ( int x, int y, int w, int h, D3DCOLOR color, int maxW );
	void	D3DBoxBorder ( float x, float y, float w, float h, D3DCOLOR border_color, D3DCOLOR color );
	void	D3DBoxBorderi ( int x, int y, int w, int h, D3DCOLOR border_color, D3DCOLOR color );

	//bool	DrawLine ( const D3DXVECTOR3 &a, const D3DXVECTOR3 &b, DWORD dwColor );
	//void	DrawLine (float x1, float y1, float x2, float y2, float width, D3DCOLOR color);
	void	Line(float x1, float y1, float x2, float y2, DWORD color);
private:
	D3DPRIMITIVETYPE		m_primType;
	IDirect3DVertexBuffer9	*m_pD3Dbuf;
	d3dvertex_s				*m_pVertex;

	DWORD					m_color;
	float					m_tu, m_tv;
	IDirect3DTexture9		*m_texture;
	int						m_maxVertex;
	int						m_curVertex;

	bool					m_canRender;
};
#endif
