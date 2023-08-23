// Dx3D9.h: interface for the Dx3D9 class.
//
//////////////////////////////////////////////////////////////////////

#ifndef  _DX_3D9_H_
#define  _DX_3D9_H_

class Dx3D9
{
protected:
	PngObj m_ico_Aud;
	PngObj m_ico_Mic;

	ID3DXLine* m_pen;
	ID3DXFont* m_font12;
	ID3DXFont* m_font18;

protected:
	HWND m_hWnd;

	int m_rgb_cx, m_rgb_cy;

protected:
	IDirect3D9* m_lpD3D;
	IDirect3DDevice9* m_lpD3DD;
	IDirect3DSurface9* m_lpRGBback;
	IDirect3DSurface9* m_lpRGBsnap;
	IDirect3DSurface9* m_lpImg[MAX_IMG];

	D3DPRESENT_PARAMETERS m_d3dpp;
	D3DXIMAGE_INFO m_infImg[MAX_IMG];

	ID3DXSprite* m_lpD3DS;

	BOOL InitD3DDevice();
	BOOL InitRGBBackSurface();
	BOOL Init_StsImgSurface();
	BOOL Init_TextureSprite();

	void Draw_3DRect(LPRECT lpDst, DX_SCREEN* pScrn);
	void Draw_3DText(LPRECT lpDst, DX_SCREEN* pScrn);

	void Draw_DxIcon(LPRECT lpDst, DX_SCREEN* pScrn);
	void Draw_Icon(LPRECT lpDst, PngObj* pIco, int& XPOS);

	void Draw_Pt_Rect(LPRECT lpDst, DX_SCREEN* pScrn);
	void DrawRtObj(LPRECT lpSrc, LPRECT lpDst, RtObj* pRt);
	void DrawMosaic(LPRECT lpDst);

protected:
	BOOL Init3D9();
	void Exit3D9();

public:
	Dx3D9::Dx3D9();
	Dx3D9::~Dx3D9();
};

#endif // _DXS3D9_H_
