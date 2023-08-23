#ifndef _MEMORYDC_H_
#define _MEMORYDC_H_

//////////////////////////////////////////////////
// CMemoryDC - memory DC
//
// Author: Keith Rule
// Email:  keithr@europa.com
// Copyright 1996-1997, Keith Rule
//
// You may freely use or modify this code provided this
// Copyright is included in all derived versions.
//
// This class implements a memory Device Context
//
// CMemoryDC는 소멸자에서 rcBounds만큼 BitBlt를 하게 된다.
// 만일 OnPaint에서 가장 마지막에 direct-draw를 사용할 경우
// 주의하여 사용해야 한다.
// 가장 좋은 방법은 {}를 이용하여 {}안에서 CMemoryDC를 사용하고
// {}밖에서 direct-draw를 사용하면 되겠다.
//

class CMemoryDC : public CDC 
{
private:
	CBitmap*	m_bitmap;
	CBitmap*	m_oldBitmap;
	CDC*		m_pDC;
	CRect		m_rcBounds;
public:
	CMemoryDC(CDC* pDC, const CRect& rcBounds) : CDC()
	{
		CreateCompatibleDC(pDC);
		m_bitmap = new CBitmap;
		m_bitmap->CreateCompatibleBitmap(pDC, rcBounds.Width(), rcBounds.Height());
		m_oldBitmap = SelectObject(m_bitmap);
		m_pDC = pDC;
		m_rcBounds = rcBounds;
	}
	~CMemoryDC() 
	{
		m_pDC->BitBlt(m_rcBounds.left, m_rcBounds.top, m_rcBounds.Width(), m_rcBounds.Height(), 
					this, 0, 0, SRCCOPY);
		SelectObject(m_oldBitmap);
		if (m_bitmap != NULL) 
			delete m_bitmap;
	}
	CMemoryDC* operator->() 
	{
		return this;
	}
	operator CMemoryDC*()
	{
		return this;
	}
};

#endif