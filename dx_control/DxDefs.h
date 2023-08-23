#ifndef _DX_DEFS_H_
#define _DX_DEFS_H_

#ifdef _WIN64
#pragma comment(lib, "../3rdparty/DirectX/x64/lib/d3d9.lib")
#pragma comment(lib, "../3rdparty/DirectX/x64/lib/d3dx9.lib")
#pragma comment(lib, "../3rdparty/DirectX/x64/lib/dsound.lib")
#pragma comment(lib, "../3rdparty/DirectX/x64/lib/dxguid.lib")
#else
#pragma comment(lib, "../3rdparty/DirectX/x86/lib/d3d9.lib")
#pragma comment(lib, "../3rdparty/DirectX/x86/lib/d3dx9.lib")
#pragma comment(lib, "../3rdparty/DirectX/x86/lib/dsound.lib")
#pragma comment(lib, "../3rdparty/DirectX/x86/lib/dxguid.lib")
#endif
#pragma comment(lib, "vfw32.lib")

#include "d3d9.h"
#include "d3dx9.h"
#include "dsound.h"

static WCHAR *szImgName[MAX_IMG] =
{
	L"blk.bmp",
	L"logo.bmp",
	L"recon.bmp",
	L"discon.bmp"
};

inline bool _cmn_col_copy(VID_IMGSRC * pVid_dst, VID_IMGSRC * pVid_src)
{
	if(pVid_dst->f_col != pVid_src->f_col)
		return false;
	switch(pVid_dst->f_col)
	{
	case FOURCC_YV12:
		{
			uint8 * y_dst = pVid_dst->y_src;
			uint8 * u_dst = pVid_dst->u_src;
			uint8 * v_dst = pVid_dst->v_src;
			int y_dst_stride = pVid_dst->y_stride;
			int uv_dst_stride = pVid_dst->uv_stride;

			uint8 * y_src = pVid_src->y_src;
			uint8 * u_src = pVid_src->u_src;
			uint8 * v_src = pVid_src->v_src;
			int y_src_stride = pVid_src->y_stride;
			int uv_src_stride = pVid_src->uv_stride;
			
			int width = pVid_src->hsize;
			int height = pVid_src->vsize;
			int width2 = width / 2;
			int height2 = height / 2;

			for(int y = height; y; y--)
			{
				memcpy(y_dst, y_src, width);
				y_src += y_src_stride;
				y_dst += y_dst_stride;
			}
			for(int y = height2; y; y--)
			{
				memcpy(u_dst, u_src, width2);
				u_src += uv_src_stride;
				u_dst += uv_dst_stride;
			}
			for(int y = height2; y; y--)
			{
				memcpy(v_dst, v_src, width2);
				v_src += uv_src_stride;
				v_dst += uv_dst_stride;
			}
			return true;
		}
		break;
	case FOURCC_BGRA:
		{
			uint8 * x_dst = pVid_dst->x_ptr;
			int x_dst_stride = pVid_dst->x_stride;

			uint8 * x_src = pVid_src->x_ptr;
			int x_src_stride = pVid_src->x_stride;
			
			int width = pVid_src->hsize;
			int height = pVid_src->vsize;
			int width4 = width * 4;

			for(int y = height; y; y--)
			{
				memcpy(x_dst, x_src, width4);
				x_src += x_src_stride;
				x_dst += x_dst_stride;
			}
			return true;
		}
		break;
	}
	return false;
}

/*****************************************************************************
 *
 *  OffSurfCtrl : Offscreen
 *
 ****************************************************************************/

struct OffSurfCtrl
{
	uint32	f_col;
	int		hsize;
	int		vsize;
	IDirect3DSurface9* lpOffSurf;

	OffSurfCtrl()
	{
		this->f_col = 0;
		this->hsize = 0;
		this->vsize = 0;
		this->lpOffSurf = NULL;
	}
	~OffSurfCtrl()
	{
		Clear();
	}
	inline void Clear()
	{
		SAFE_RELEASE(this->lpOffSurf);
		this->f_col = 0;
		this->hsize = 0;
		this->vsize = 0;
	}
	inline BOOL IsSurf(VID_IMGSRC* pVid_img)
	{
		if(!this->lpOffSurf) return FALSE;
		if(this->f_col != pVid_img->f_col) return FALSE;
		if(this->hsize != pVid_img->hsize) return FALSE;
		if(this->vsize != pVid_img->vsize) return FALSE;
		return TRUE;
	}
	inline BOOL Create(IDirect3DDevice9* lpD3DD, VID_IMGSRC* pVid_img)
	{
		SAFE_RELEASE(this->lpOffSurf);

		this->f_col = pVid_img->f_col;
		this->hsize = pVid_img->hsize;
		this->vsize = pVid_img->vsize;

		HRESULT hr = lpD3DD->CreateOffscreenPlainSurface(
			this->hsize, this->vsize,
			(D3DFORMAT)(this->f_col == FOURCC_BGRA ? D3DFMT_X8R8G8B8 : MAKEFOURCC('Y','V','1','2')),
			D3DPOOL_DEFAULT,
			&this->lpOffSurf,
			0);
		if(SUCCEEDED(hr)) return TRUE;

		Clear();

		return FALSE;
	}
	inline BOOL Draw(IDirect3DDevice9* lpD3DD, VID_IMGSRC* pVid_img)
	{
		if(!IsSurf(pVid_img))
		{
			if(!Create(lpD3DD, pVid_img)) return FALSE;
		}

		D3DLOCKED_RECT locked;
		switch(this->f_col)
		{
		case FOURCC_YV12:
			{
				HRESULT hr = this->lpOffSurf->LockRect(&locked, 0, D3DLOCK_NOSYSLOCK);
				if(FAILED(hr)) return FALSE;

				BOOL bRet = FALSE;
				switch(pVid_img->f_col)
				{
				case FOURCC_YV12:
					{
						VID_IMGSRC stVid_dst;
						stVid_dst.f_col = this->f_col;
						stVid_dst.y_src = (uint8*)locked.pBits;
						stVid_dst.v_src = (stVid_dst.y_src + locked.Pitch*this->vsize);
						stVid_dst.u_src = (stVid_dst.v_src + locked.Pitch*this->vsize/4);
						stVid_dst.y_stride = locked.Pitch;
						stVid_dst.uv_stride = locked.Pitch/2;

						_cmn_col_copy(&stVid_dst, pVid_img);
					}
					bRet = TRUE;
					break;
				}

				this->lpOffSurf->UnlockRect();

				return bRet;
			}
			break;
		case FOURCC_BGRA:
			{
				HRESULT hr = this->lpOffSurf->LockRect(&locked, 0, D3DLOCK_NOSYSLOCK);
				if(FAILED(hr)) return FALSE;

				BOOL bRet = FALSE;
				switch(pVid_img->f_col)
				{
				case FOURCC_BGRA:
					{
						VID_IMGSRC stVid_dst;
						stVid_dst.f_col = this->f_col;
						stVid_dst.x_ptr = (uint8*)locked.pBits;
						stVid_dst.x_stride = locked.Pitch;

						_cmn_col_copy(&stVid_dst, pVid_img);
					}
					bRet = TRUE;
					break;
				}

				this->lpOffSurf->UnlockRect();

				return bRet;
			}
			break;
		}
		return FALSE;
	}
};

/*****************************************************************************
 *
 *  IconObj
 *
 ****************************************************************************/

struct PngObj
{
	IDirect3DTexture9* txIcon[2];
	RECT reIcon[2];

	PngObj()
	{
		for(int i=0; i<2; i++)
		{
			this->txIcon[i] = NULL;
			SetRectEmpty(&this->reIcon[i]);
		}
	}
	~PngObj()
	{
		Clear();
	}

	inline void Clear()
	{
		for(int i=0; i<2; i++)
		{
			SAFE_RELEASE(this->txIcon[i]);
			SetRectEmpty(&this->reIcon[i]);
		}
	}
	inline void Load(IDirect3DDevice9* pDev, int idx, WCHAR* szPath)
	{
		DWORD dColorKey = 0x00FFFFFF;

		D3DXIMAGE_INFO info;
		memset(&info, 0, sizeof(info));

		HRESULT hr = D3DXCreateTextureFromFileEx(
			pDev,
			szPath,
			D3DX_DEFAULT,
			D3DX_DEFAULT,
			1,
			0,
			D3DFMT_UNKNOWN,
			D3DPOOL_DEFAULT,//D3DPOOL_MANAGED,
			D3DX_FILTER_NONE,
			D3DX_FILTER_NONE,
			dColorKey,
			&info,
			NULL,
			&(this->txIcon[idx]));
		if(!FAILED(hr))
		{
			this->reIcon[idx].right = info.Width;
			this->reIcon[idx].bottom = info.Height;
		}
	}
	inline IDirect3DTexture9* TX(int idx)
	{
		return this->txIcon[idx];
	}
	inline LPRECT Re(int idx)
	{
		return &(this->reIcon[idx]);
	}
	inline int Cx(int idx)
	{
		return this->reIcon[idx].right;
	}
	inline int Cy(int idx)
	{
		return this->reIcon[idx].bottom;
	}
};

/*****************************************************************************
 *
 *  Vertex
 *
 ****************************************************************************/

struct Vertex
{
	float x, y, z;
	DWORD cr;

	Vertex()
	{
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
		this->cr = 0;
	}
	Vertex(LPPOINT lp, DWORD color)
	{
		this->x = (float)lp->x;
		this->y = (float)lp->y;
		this->z = 0.0f;// lp->x % 3 ? 1.0f : 0.0f;
		this->cr = color;
	}
	inline Vertex& operator = (Vertex& other)
	{
		memcpy(this, &other, sizeof(Vertex));
		return *this;
	}
};

inline int RECT2Vertex(LPRECT lp, D3DCOLOR color, vector<Vertex>& vtx_array)
{
	CPoint pt1(lp->left, lp->top);
	CPoint pt2(lp->right, lp->top);
	CPoint pt3(lp->left, lp->bottom);
	CPoint pt4(lp->right, lp->bottom);
	vtx_array.push_back(Vertex(&pt1, color));
	vtx_array.push_back(Vertex(&pt2, color));
	vtx_array.push_back(Vertex(&pt3, color));
	vtx_array.push_back(Vertex(&pt4, color));
	vtx_array.push_back(Vertex(&pt3, color));
	vtx_array.push_back(Vertex(&pt2, color));
	return 2;
}

inline D3DCOLOR D3DCOLOR_COLRF(COLORREF cr, uint8 a = 0xff)
{
	uint8 r = GetRValue(cr);
	uint8 g = GetGValue(cr);
	uint8 b = GetBValue(cr);
	return D3DCOLOR_ARGB(a,r,g,b);
}

inline void RECT2D3DXVECTOR(LPRECT lp, vector<D3DXVECTOR2>& vct_array)
{
	vct_array.push_back(D3DXVECTOR2((float)lp->left, (float)lp->bottom));
	vct_array.push_back(D3DXVECTOR2((float)lp->left, (float)lp->top));
	vct_array.push_back(D3DXVECTOR2((float)lp->right, (float)lp->top));
	vct_array.push_back(D3DXVECTOR2((float)lp->right, (float)lp->bottom));
	vct_array.push_back(D3DXVECTOR2((float)lp->left, (float)lp->bottom));
}

#endif // _DX_DEFS_H_