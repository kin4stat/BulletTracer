#include "d3drender.h"

IDirect3DDevice9 *CD3DBaseRender::		m_pD3Ddev = NULL;
IDirect3DStateBlock9 *CD3DBaseRender::	m_pD3DstateDraw = NULL;
IDirect3DStateBlock9 *CD3DBaseRender::	m_pD3DstateNorm = NULL;
int CD3DBaseRender::					m_renderCount = 0;
int CD3DBaseRender::					m_numShared = 0;
bool CD3DBaseRender::					m_statesOK = false;

inline d3dvertex_s Init2DVertex ( float x, float y, DWORD color, float tu, float tv )
{
	d3dvertex_s v = { x, y, 1.0f, 1.0f, color, tu, tv };
	return v;
}

CD3DBaseRender::CD3DBaseRender ()
{
	m_numShared++;
}

CD3DBaseRender::~CD3DBaseRender ()
{
	if ( --m_numShared == 0 )
		DeleteStates();
}

HRESULT CD3DBaseRender::Initialize ( IDirect3DDevice9 *pD3Ddev )
{
	if ( m_pD3Ddev == NULL && (m_pD3Ddev = pD3Ddev) == NULL )
		return E_FAIL;

	if ( !m_statesOK && FAILED(CreateStates()) )
		return E_FAIL;

	return S_OK;
}

HRESULT CD3DBaseRender::Invalidate ()
{
	DeleteStates();
	return S_OK;
}

HRESULT CD3DBaseRender::BeginRender ()
{
	if ( !m_statesOK )
		return E_FAIL;

	if ( ++m_renderCount == 1 )
	{
		m_pD3DstateNorm->Capture();
		m_pD3DstateDraw->Apply();
	}

	return S_OK;
}

HRESULT CD3DBaseRender::EndRender ()
{
	if ( !m_statesOK )
		return E_FAIL;

	m_renderCount--;

	if ( m_renderCount == 0 )
		m_pD3DstateNorm->Apply();
	else if ( m_renderCount < 0 )
		m_renderCount = 0;

	return S_OK;
}

HRESULT CD3DBaseRender::CreateStates ()
{
	for ( int iStateBlock = 0; iStateBlock < 2; iStateBlock++ )
	{
		m_pD3Ddev->BeginStateBlock();
		m_pD3Ddev->SetPixelShader( NULL );
		m_pD3Ddev->SetVertexShader( NULL );

		m_pD3Ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pD3Ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		m_pD3Ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		m_pD3Ddev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		m_pD3Ddev->SetRenderState( D3DRS_ALPHAREF, 0x08 );
		m_pD3Ddev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		m_pD3Ddev->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
		m_pD3Ddev->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
		m_pD3Ddev->SetRenderState( D3DRS_STENCILENABLE, FALSE );
		m_pD3Ddev->SetRenderState( D3DRS_CLIPPING, TRUE );
		m_pD3Ddev->SetRenderState( D3DRS_CLIPPLANEENABLE, FALSE );
		m_pD3Ddev->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );
		m_pD3Ddev->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
		m_pD3Ddev->SetRenderState( D3DRS_FOGENABLE, FALSE );
		m_pD3Ddev->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS, FALSE );
		m_pD3Ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
								   D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );

		m_pD3Ddev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
		m_pD3Ddev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pD3Ddev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		m_pD3Ddev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
		m_pD3Ddev->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pD3Ddev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		m_pD3Ddev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		m_pD3Ddev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		m_pD3Ddev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
		m_pD3Ddev->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

		m_pD3Ddev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		m_pD3Ddev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		m_pD3Ddev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

		if ( iStateBlock )
			m_pD3Ddev->EndStateBlock( &m_pD3DstateDraw );
		else
			m_pD3Ddev->EndStateBlock( &m_pD3DstateNorm );
	}

	m_statesOK = true;

	return S_OK;
}

HRESULT CD3DBaseRender::DeleteStates ()
{
	SAFE_RELEASE( m_pD3DstateDraw );
	SAFE_RELEASE( m_pD3DstateNorm );
	m_statesOK = false;

	return S_OK;
}

CD3DFont::CD3DFont ( const char *szFontName, int fontHeight, DWORD dwCreateFlags )
{
	strcpy_s( m_szFontName, (szFontName ? szFontName : "Arial"));

	m_fontHeight = fontHeight;
	m_dwCreateFlags = dwCreateFlags;

	m_isReady = false;
	m_statesOK = false;

	m_pD3Dtex = NULL;
	m_pD3Dbuf = NULL;
	m_pRender = NULL;

	m_maxTriangles = 224 * 2;

	m_texWidth = m_texHeight = 0;
	m_chrSpacing = 0;

	memset( m_fTexCoords, 0, sizeof(float) * 224 * 4 );

	m_fChrHeight = 0.0;
}

CD3DFont::~CD3DFont ()
{
	Invalidate();
}

/* god, what a mess */
HRESULT CD3DFont::Initialize ( IDirect3DDevice9 *pD3Ddev )
{
	if ( FAILED(CD3DBaseRender::Initialize(pD3Ddev)) )
		return E_FAIL;

	if ( m_pRender == NULL && (m_pRender = new CD3DRender(16)) == NULL )
		return E_FAIL;

	if ( FAILED(m_pRender->Initialize(pD3Ddev)) )
		return E_FAIL;

	m_texWidth = m_texHeight = 512;

	if ( FAILED(m_pD3Ddev->CreateTexture(m_texWidth, m_texHeight, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &m_pD3Dtex,
				 NULL)) )
		return E_FAIL;

	if ( FAILED(m_pD3Ddev->CreateVertexBuffer(m_maxTriangles * 3 * sizeof(d3dvertex_s),
				 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_pD3Dbuf, NULL)) )
	{
		SAFE_RELEASE( m_pD3Dtex );
		return E_FAIL;
	}

	DWORD		*pBitmapBits;
	BITMAPINFO	bmi;

	memset( &bmi.bmiHeader, 0, sizeof(BITMAPINFOHEADER) );
	bmi.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	bmi.bmiHeader.biWidth = m_texWidth;
	bmi.bmiHeader.biHeight = -m_texHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount = 32;

	HDC		hDC = CreateCompatibleDC( NULL );
	HBITMAP hbmBitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS, (void **) &pBitmapBits, NULL, 0 );
	SetMapMode( hDC, MM_TEXT );

	int		iHeight = -m_fontHeight * (int)GetDeviceCaps( hDC, LOGPIXELSY ) / 72;

	HFONT	hFont = CreateFontA( iHeight, 0, 0, 0, (m_dwCreateFlags & FCR_BOLD) ? FW_BOLD : FW_NORMAL,
								m_dwCreateFlags & FCR_ITALICS, false, false, DEFAULT_CHARSET, OUT_TT_PRECIS,
								CLIP_DEFAULT_PRECIS, PROOF_QUALITY, VARIABLE_PITCH, m_szFontName );

	if ( hFont == NULL )
	{
		DeleteObject( hbmBitmap );
		DeleteDC( hDC );
		DeleteObject( hFont );
		SAFE_RELEASE( m_pD3Dtex );
		return E_FAIL;
	}

	SelectObject( hDC, hbmBitmap );
	SelectObject( hDC, hFont );

	RECT	all = { 0, 0, m_texWidth - 1, m_texWidth - 1 };
	HBRUSH	green = CreateSolidBrush( RGB(0, 255, 0) );
	FillRect( hDC, &all, green );
	DeleteObject( green );

	SetBkMode( hDC, TRANSPARENT );
	SetTextAlign( hDC, TA_TOP );

	SIZE	size;
	char	ch = ' ';

	GetTextExtentPoint32A( hDC, &ch, 1, &size );

	m_chrSpacing = ( size.cx + 3 ) / 4;
	m_fChrHeight = (float)( size.cy );

	if ( m_dwCreateFlags & FCR_BORDER )
	{
		size.cx += 2;
		size.cy += 2;
	}

	int y = 0, x = ( size.cx + 3 ) / 4;

	for ( int c = 32; c < 256; c++ )
	{
		ch = (char)c;

		GetTextExtentPoint32A( hDC, &ch, 1, &size );
		if ( m_dwCreateFlags & FCR_BORDER )
		{
			size.cx += 3;
			size.cy += 3;
		}

		if ( x + size.cx + m_chrSpacing > m_texWidth )
		{
			x = m_chrSpacing;
			if ( y + size.cy * 2 + 2 < m_texHeight )
				y += size.cy + 1;
		}

		RECT	rect = { x, y, x + size.cx, y + size.cy };

		if ( m_dwCreateFlags & FCR_BORDER )
		{
			// XXX retarded :p use font outline instead
			SetTextColor( hDC, RGB(0, 0, 0) );
			x++;
			y++;
			ExtTextOutA( hDC, x - 1, y - 1, ETO_CLIPPED, &rect, &ch, 1, NULL );
			ExtTextOutA( hDC, x, y - 1, ETO_CLIPPED, &rect, &ch, 1, NULL );
			ExtTextOutA( hDC, x + 1, y - 1, ETO_CLIPPED, &rect, &ch, 1, NULL );
			ExtTextOutA( hDC, x + 1, y, ETO_CLIPPED, &rect, &ch, 1, NULL );
			ExtTextOutA( hDC, x + 1, y + 1, ETO_CLIPPED, &rect, &ch, 1, NULL );
			ExtTextOutA( hDC, x, y + 1, ETO_CLIPPED, &rect, &ch, 1, NULL );
			ExtTextOutA( hDC, x - 1, y + 1, ETO_CLIPPED, &rect, &ch, 1, NULL );
			ExtTextOutA( hDC, x - 1, y, ETO_CLIPPED, &rect, &ch, 1, NULL );
			SetTextColor( hDC, RGB(255, 0, 0) );
			ExtTextOutA( hDC, x, y, ETO_CLIPPED, &rect, &ch, 1, NULL );
			x--;
			y--;
		}
		else
		{
			SetTextColor( hDC, RGB(255, 0, 0) );
			ExtTextOutA( hDC, x, y, ETO_CLIPPED, &rect, &ch, 1, NULL );
		}

		//tu src + dst
		m_fTexCoords[c - 32][0] = (float)( x + 0 - m_chrSpacing ) / (float)m_texWidth;
		m_fTexCoords[c - 32][2] = (float)( x + size.cx + m_chrSpacing ) / (float)m_texWidth;

		//tv src + dst
		m_fTexCoords[c - 32][1] = (float)( y + 0 + 0 ) / (float)m_texHeight;
		m_fTexCoords[c - 32][3] = (float)( y + size.cy + 0 ) / (float)m_texHeight;

		x += size.cx + ( 2 * m_chrSpacing );
	}

	D3DLOCKED_RECT	d3dlr;
	m_pD3Dtex->LockRect( 0, &d3dlr, 0, 0 );

	BYTE	*pDstRow = (BYTE *)d3dlr.pBits;
	WORD	*pDst16;

	for ( y = 0; y < m_texHeight; y++ )
	{
		pDst16 = (WORD *)pDstRow;

		for ( x = 0; x < m_texWidth; x++ )
		{
			DWORD	pixel = pBitmapBits[m_texWidth * y + x];
			BYTE	bAlpha = 15 - ( ((BYTE) (pixel >> 8)) >> 4 );	// green channel
			BYTE	bValue = ( (BYTE) (pixel >> 16) ) >> 4;			// red channel
			*pDst16 = ( WORD ) ( bAlpha << 12 ) | ( bValue << 8 ) | ( bValue << 4 ) | ( bValue );
			pDst16++;
		}

		pDstRow += d3dlr.Pitch;
	}

	m_pD3Dtex->UnlockRect( 0 );

	DeleteObject( hbmBitmap );
	DeleteDC( hDC );
	DeleteObject( hFont );

	m_isReady = true;

	return S_OK;
}

HRESULT CD3DFont::Invalidate ()
{
	m_isReady = false;

	SAFE_RELEASE( m_pD3Dtex );
	SAFE_RELEASE( m_pD3Dbuf );

	//SAFE_RELEASE(m_pD3DstateDraw);
	//SAFE_RELEASE(m_pD3DstateNorm);
	m_pRender->Invalidate();

	CD3DBaseRender::Invalidate();
	return S_OK;
}

static signed char hex_to_dec(signed char ch)
{
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	if (ch >= 'a' && ch <= 'f')
		return ch - 'A' + 10;

	return -1;
}

D3DCOLOR hex_to_color(const char *str, int len)
{
	char buf[12];
	strncpy_s(buf, str, len);
	D3DCOLOR color = 0x00;
	byte *colorByteSet = (byte *) &color;
	int stri = 0;
	for (int i = sizeof(color) -1; i >= 0; i--)
	{
		if (i == 3 && len == 6)
		{
			colorByteSet[3] = 0xFF;
		}
		else
		{
			signed char bh = hex_to_dec(buf[stri++]);
			signed char bl = hex_to_dec(buf[stri++]);
			if (bh != -1 && bl != -1)
			{
				colorByteSet[i] = bl | (bh << 4);
			}
		}
	}
	return color;
}

HRESULT CD3DFont::Print ( float x, float y, DWORD color, const char *szText, bool isShadow )
{
	if ( !m_isReady )
		return E_FAIL;

	x -= (float)m_chrSpacing;

	if ( FAILED(this->BeginRender()) )
		return E_FAIL;

	DWORD	fvf;
	m_pD3Ddev->GetFVF( &fvf );
	m_pD3Ddev->SetFVF( D3DFVF_BITMAPFONT );
	m_pD3Ddev->SetTexture( 0, m_pD3Dtex );
	m_pD3Ddev->SetStreamSource( 0, m_pD3Dbuf, 0, sizeof(d3dvertex_s) );

	if ( szText && *szText )
	{
		UINT		usedTriangles = 0;
		d3dvertex_s *pVertex;

		if ( FAILED(m_pD3Dbuf->Lock(0, 0, (void **) &pVertex, D3DLOCK_DISCARD)) )
		{
			m_pD3Ddev->SetFVF( fvf );
			this->EndRender();
			return E_FAIL;
		}

		size_t len = strlen( szText );
		for ( size_t cpos = 0; cpos < len && szText[cpos] != '\0'; cpos++ )
		{
			int c = * ( unsigned char * ) ( &szText[cpos] ) - 32;
			if ( !(c >= 0 && c < 224) )
				continue;

			if ( szText[cpos] == '{' )
			{
				size_t npos = cpos + 7;
				if ( npos < len && szText[npos] == '}' )
				{
					if ( !isShadow )
					{
						color = hex_to_color( &szText[cpos + 1], 6 );
					}
					cpos = npos;
					continue;
				}
				else 
				{
					npos += 2;
					if ( npos < len && szText[npos] == '}' )
					{
						if ( !isShadow )
						{
							color = hex_to_color( &szText[cpos + 1], 8 );
						}
						cpos = npos;
						continue;
					}
				}
			}

			float	tx1 = m_fTexCoords[c][0];
			float	tx2 = m_fTexCoords[c][2];
			float	ty1 = m_fTexCoords[c][1];
			float	ty2 = m_fTexCoords[c][3];
			float	w = ( tx2 - tx1 ) * m_texWidth;
			float	h = ( ty2 - ty1 ) * m_texHeight;

			*pVertex++ = Init2DVertex( x - 0.5f, y - 0.5f, color, tx1, ty1 );			//topleft
			*pVertex++ = Init2DVertex( x + w - 0.5f, y - 0.5f, color, tx2, ty1 );		//topright
			*pVertex++ = Init2DVertex( x - 0.5f, y + h - 0.5f, color, tx1, ty2 );		//bottomleft
			*pVertex++ = Init2DVertex( x + w - 0.5f, y - 0.5f, color, tx2, ty1 );		//topright
			*pVertex++ = Init2DVertex( x + w - 0.5f, y + h - 0.5f, color, tx2, ty2 );	//bottomright
			*pVertex++ = Init2DVertex( x - 0.5f, y + h - 0.5f, color, tx1, ty2 );		//bottomleft
			if ( m_dwCreateFlags & FCR_BORDER )
				w -= 2.0f;

			x += w - ( m_chrSpacing * 2 );
			usedTriangles += 2;
			if ( usedTriangles >= m_maxTriangles )
				break;
		}

		if ( usedTriangles > 0 )
		{
			m_pD3Dbuf->Unlock();
			m_pD3Ddev->DrawPrimitive( D3DPT_TRIANGLELIST, 0, usedTriangles );
		}
	}

	m_pD3Ddev->SetFVF( fvf );
	this->EndRender();

	return S_OK;
}

HRESULT CD3DFont::PrintShadow ( float x, float y, DWORD color, const char *szText )
{
	DWORD shadow = D3DCOLOR_ARGB((BYTE)HIBYTE(HIWORD(color)), 0, 0, 0);
	Print( x + 1, y + 1, shadow, szText, true );
	Print( x, y, color, szText );

	return S_OK;
}

float CD3DFont::DrawLength ( const char *szText ) const
{
	float	len = 0.0f;
	float	sub = ( m_dwCreateFlags & FCR_BORDER ) ? 2.0f : 0.0f;

	size_t strl = strlen( szText );
	for ( size_t cpos = 0; cpos < strl && szText[cpos] != '\0'; cpos++ )
	{
		int c = * ( unsigned char * ) ( &szText[cpos] ) - 32;
		if ( !(c >= 0 && c < 224) )
			continue;

		if ( szText[cpos] == '{' )
		{
			size_t npos = cpos + 7;
			if ( npos < strl && szText[npos] == '}' )
			{
				cpos = npos;
				continue;
			}
			else 
			{
				npos += 2;
				if ( npos < strl && szText[npos] == '}' )
				{
					cpos = npos;
					continue;
				}
			}
		}

		len += ( (m_fTexCoords[c][2] - m_fTexCoords[c][0]) * m_texWidth - sub ) - m_chrSpacing * 2;
	}
	return len;
}

CD3DRender::CD3DRender ( int numVertices )
{
	m_canRender = false;

	m_pD3Dbuf = NULL;
	m_pVertex = NULL;

	m_color = 0;
	m_tu = 0.0f;
	m_tv = 0.0f;
	m_texture = NULL;

	m_maxVertex = numVertices;
	m_curVertex = 0;
}

CD3DRender::~CD3DRender ()
{
	Invalidate();
}

HRESULT CD3DRender::Initialize ( IDirect3DDevice9 *pD3Ddev )
{
	if ( !m_canRender )
	{
		if ( FAILED(CD3DBaseRender::Initialize(pD3Ddev)) )
			return E_FAIL;
		if ( FAILED(m_pD3Ddev->CreateVertexBuffer(m_maxVertex * sizeof(d3dvertex_s),
					 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_pD3Dbuf, NULL)) )
			return E_FAIL;

		m_canRender = true;
	}

	return S_OK;
}

HRESULT CD3DRender::Invalidate ()
{
	SAFE_RELEASE( m_pD3Dbuf );
	SAFE_RELEASE( m_texture );
	m_pVertex = NULL;

	//SAFE_RELEASE(m_pD3DstateDraw);
	//SAFE_RELEASE(m_pD3DstateNorm);
	CD3DBaseRender::Invalidate();
	m_canRender = false;

	return S_OK;
}

HRESULT CD3DRender::Begin ( D3DPRIMITIVETYPE primType )
{
	if ( !m_canRender )
		return E_FAIL;

	if ( m_pVertex != NULL )
		return E_FAIL;

	if ( FAILED(m_pD3Dbuf->Lock(0, 0, (void **) &m_pVertex, D3DLOCK_DISCARD)) )
		return E_FAIL;

	m_primType = primType;

	return S_OK;
}

HRESULT CD3DRender::End ()
{
	int numPrims;

	m_pVertex = NULL;

	if ( !m_canRender )
	{
		m_curVertex = 0;
		return E_FAIL;
	}

	if ( FAILED(CD3DBaseRender::BeginRender()) )
		return E_FAIL;

	switch ( m_primType )
	{
	case D3DPT_POINTLIST:
		numPrims = m_curVertex;
		break;

	case D3DPT_LINELIST:
		numPrims = m_curVertex / 2;
		break;

	case D3DPT_LINESTRIP:
		numPrims = m_curVertex - 1;
		break;

	case D3DPT_TRIANGLELIST:
		numPrims = m_curVertex / 3;
		break;

	case D3DPT_TRIANGLESTRIP:
	case D3DPT_TRIANGLEFAN:
		numPrims = m_curVertex - 2;
		break;

	default:
		numPrims = 0;
		break;
	}

	m_curVertex = 0;

	if ( numPrims > 0 )
	{
		m_pD3Dbuf->Unlock();

		DWORD	fvf;
		m_pD3Ddev->GetFVF( &fvf );

		if ( m_texture == NULL )
		{
			m_pD3Ddev->SetFVF( D3DFVF_PRIMITIVES );
			m_pD3Ddev->SetTexture( 0, NULL );
		}
		else
		{
			m_pD3Ddev->SetFVF( D3DFVF_BITMAPFONT );
			m_pD3Ddev->SetTexture( 0, m_texture );
		}

		m_pD3Ddev->SetStreamSource( 0, m_pD3Dbuf, 0, sizeof(d3dvertex_s) );

		m_pD3Ddev->DrawPrimitive( m_primType, 0, numPrims );

		m_pD3Ddev->SetFVF( fvf );
	}

	CD3DBaseRender::EndRender();

	return S_OK;
}

HRESULT CD3DRender::D3DColor ( DWORD color )
{
	m_color = color;
	return m_canRender ? S_OK : E_FAIL;
	return S_OK;
}

void CD3DRender::D3DBindTexture ( IDirect3DTexture9 *texture )
{
	m_texture = texture;
}

void CD3DRender::D3DTexCoord2f ( float u, float v )
{
	m_tu = u;
	m_tv = v;
}

HRESULT CD3DRender::D3DVertex2f ( float x, float y )
{
	if ( m_canRender && m_pVertex && ++m_curVertex < m_maxVertex )
		*m_pVertex++ = Init2DVertex( x, y, m_color, m_tu, m_tv );
	else
		return E_FAIL;

	return S_OK;
}

void CD3DRender::D3DTexQuad ( float sx, float sy, float ex, float ey, float su, float sv, float eu, float ev )
{
	if ( SUCCEEDED(Begin(D3DPT_TRIANGLELIST)) )
	{
		D3DColor( D3DCOLOR_XRGB(255, 255, 255) );
		D3DTexCoord2f( su, sv );
		D3DVertex2f( sx, sy );
		D3DTexCoord2f( eu, sv );
		D3DVertex2f( ex, sy );
		D3DTexCoord2f( su, ev );
		D3DVertex2f( sx, ey );

		D3DTexCoord2f( su, ev );
		D3DVertex2f( sx, ey );
		D3DTexCoord2f( eu, sv );
		D3DVertex2f( ex, sy );
		D3DTexCoord2f( eu, ev );
		D3DVertex2f( ex, ey );
		End();
	}
}

void CD3DRender::D3DBox ( float x, float y, float w, float h, D3DCOLOR color )
{
	if ( SUCCEEDED(Begin(D3DPT_TRIANGLELIST)) )
	{
		D3DColor( color );
		D3DVertex2f( x, y );
		D3DVertex2f( x + w, y );
		D3DVertex2f( x, y + h );
		D3DVertex2f( x, y + h );
		D3DVertex2f( x + w, y );
		D3DVertex2f( x + w, y + h );
		End();
	}
}

void CD3DRender::D3DBoxi ( int x, int y, int w, int h, D3DCOLOR color, int maxW )
{
	if ( maxW )
	{
		if ( w >= maxW )
			( w = maxW );
		D3DBox( (float)x, (float)y, (float)w, (float)h, color );
	}
	else
	{
		D3DBox( (float)x, (float)y, (float)w, (float)h, color );
	}
}

void CD3DRender::D3DBoxBorder ( float x, float y, float w, float h, D3DCOLOR border_color, D3DCOLOR color )
{
	D3DBox( x, y, w, 1.0f, border_color );
	D3DBox( x + w, y, 1.0f, h, border_color );
	D3DBox( x, y, 1.0f, h, border_color );
	D3DBox( x, y + h, w, 1.0f, border_color );
	D3DBox( x + 1.0f, y + 1.0f, w - 1.0f, h - 1.0f, color );
}

void CD3DRender::D3DBoxBorderi ( int x, int y, int w, int h, D3DCOLOR border_color, D3DCOLOR color )
{
	D3DBoxBorder( (float)x, (float)y, (float)w, (float)h, border_color, color );
}

void CD3DRender::Line(float x1, float y1, float x2, float y2, DWORD color)
{
	if (SUCCEEDED(Begin(D3DPT_LINELIST)))
	{
		D3DColor(color);
		D3DVertex2f(x1, y1);
		D3DVertex2f(x2, y2);
		End();
	}
}