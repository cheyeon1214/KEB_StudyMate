#ifndef _DIALOG_DEFS_H_
#define _DIALOG_DEFS_H_

const LOGFONT BTN_FONT = {-14, 0, 0, 0, 700, FALSE, FALSE, 0, 0, 3, 2, 1, 34, _T("MS Shell Dlg 2")};
const LOGFONT COMB_FONT = {-11, 0, 0, 0, 400, FALSE, FALSE, 0, 0, 3, 2, 1, 34, _T("MS Shell Dlg 2")};
const COLORREF bg_crtext = RGB(0, 0, 0);
const COLORREF bg_crback = RGB(162, 172, 181);
const COLORREF btn_crtext = RGB(63, 63, 63);
const COLORREF btn_crtextd = RGB(255, 255, 255);
const COLORREF cmb_crtext = RGB(5, 5, 5);
const COLORREF cmb_crsel = RGB(193, 163, 229);
const COLORREF cmb_crback = RGB(229, 229, 229);

const DWORD dwPushStyle = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_FLAT;
const DWORD dwCheckStyle = WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_FLAT;

inline void SetComboUint(CComboBox& cmb, UINT ui)
{
	for(int i=0; i<cmb.GetCount(); i++)
	{
		if(cmb.GetItemData(i) == ui)
		{
			cmb.SetCurSel(i);
			cmb.Invalidate(FALSE);
			break;
		}
	}
}
inline UINT GetComboUint(CComboBox& cmb)
{
	int nSel = cmb.GetCurSel();
	if(nSel > -1)
	{
		return (UINT)cmb.GetItemData(nSel);
	}
	return 0;
}

inline int GetStringColumnScale(CListCtrl* plist, WCHAR* sz, int cx)
{
	CDC *pDC = plist->GetDC();
	CFont* pOld = pDC->SelectObject(plist->GetFont());
	CSize size = pDC->GetTextExtent(sz, (int)wcslen(sz));
	pDC->SelectObject(pOld);
	plist->ReleaseDC(pDC);

	int iScale = 1;
	if(size.cx > cx) iScale = 1 + (size.cx / cx);

	return iScale;
}

inline CRect GetScreen(CWnd* pWnd, int nID, int dftx = 5, int dfty = 5)
{
	CRect re;
	pWnd->GetDlgItem(nID)->GetWindowRect(&re);
	pWnd->ScreenToClient(&re);
	re.DeflateRect(dftx, dfty);
	return re;
}

#endif // _DIALOG_DEFS_H_
