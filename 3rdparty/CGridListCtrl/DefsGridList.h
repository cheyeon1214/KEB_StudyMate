#ifndef _DEFS_GRID_LIST_H_
#define _DEFS_GRID_LIST_H_

#include "CGridRowTraitXP.h"
#include "CGridColumnTraitEdit.h"
#include "CGridColumnTraitCombo.h"
#include "CGridColumnTraitDateTime.h"
#include "CGridColumnTraitProgress.h"
#include "CGridColumnTraitHyperLink.h"
#include "CGridColumnTraitTextButton.h"

/*****************************************************************************
 *
 *  Common
 *
 ****************************************************************************/

enum
{
	G_CTRL_Text = 0,
	G_CTRL_Edit,
	G_CTRL_Combo,
	G_CTRL_DateTime,
	G_CTRL_Progress,
	G_CTRL_HyperLink,
	G_CTRL_TextButton,
};

struct ToggleParam
{
	int ID_checked;
	int ID_unchecked;
	WCHAR sz_checked[64];
	WCHAR sz_unchecked[64];

	ToggleParam(int ID_image)
	{
		wcscpy_s(this->sz_checked, L"On");
		wcscpy_s(this->sz_unchecked, L"Off");
		this->ID_unchecked = ID_image;
		this->ID_checked = ID_image + 1;
	}
};
inline CGridColumnTrait* G_Column(UINT G_Type, DWORD dwStyle = 0, ToggleParam* pToggle = NULL, BOOL bSingle = FALSE)
{
	CGridColumnTrait* pTrait = NULL;
	switch(G_Type)
	{
	case G_CTRL_Text:
		{
			CGridColumnTraitImage* pTextTrait = new CGridColumnTraitImage;
			if(pToggle)
			{
				pTextTrait->AddImageIndex(pToggle->ID_unchecked);
				pTextTrait->AddImageIndex(pToggle->ID_checked);
				pTextTrait->SetToggleSelection(true);
			}
			pTrait = pTextTrait;
		}
		break;
	case G_CTRL_Edit:
		{
			CGridColumnTraitEdit* pEditTrait = new CGridColumnTraitEdit;
			pEditTrait->SetStyle(pEditTrait->GetStyle() | dwStyle);
			if(pToggle)
			{
				pEditTrait->AddImageIndex(pToggle->ID_unchecked);
				pEditTrait->AddImageIndex(pToggle->ID_checked);
				pEditTrait->SetToggleSelection(true);
			}
			if(bSingle) pEditTrait->SetSingleClickEdit(true);
			pTrait = pEditTrait;
		}
		break;
	case G_CTRL_Combo:
		{
			CGridColumnTraitCombo* pComboTrait = new CGridColumnTraitCombo;
			pComboTrait->SetStyle(pComboTrait->GetStyle() | CBS_DROPDOWNLIST | dwStyle);
			if(pToggle)
			{
				pComboTrait->AddItem(0, pToggle->sz_checked);
				pComboTrait->AddItem(1, pToggle->sz_unchecked);
			}
			if(bSingle) pComboTrait->SetSingleClickEdit(true);
			pTrait = pComboTrait;
		}
		break;
	case G_CTRL_DateTime:
		{
			CGridColumnTraitDateTime* pDateTimeTrait = new CGridColumnTraitDateTime;
			pDateTimeTrait->SetStyle(pDateTimeTrait->GetStyle() | dwStyle);
			if(pToggle)
			{
				pDateTimeTrait->AddImageIndex(pToggle->ID_unchecked);
				pDateTimeTrait->AddImageIndex(pToggle->ID_checked);
				pDateTimeTrait->SetToggleSelection(true);
			}
			if(bSingle) pDateTimeTrait->SetSingleClickEdit(true);
			pTrait = pDateTimeTrait;
		}
		break;
	case G_CTRL_Progress:
		{
			CGridColumnTraitProgress* pProgressTrait = new CGridColumnTraitProgress;
			pProgressTrait->HooverShowValue(true);
			pProgressTrait->UseTheme(false);
			pTrait = pProgressTrait;
		}
		break;
	case G_CTRL_HyperLink:
		{
			CGridColumnTraitHyperLink* pHyperLinkTrait = new CGridColumnTraitHyperLink;
			pHyperLinkTrait->SetShellFilePrefix(_T("http://"));
			if(pToggle)
			{
				pHyperLinkTrait->AddImageIndex(pToggle->ID_unchecked);
				pHyperLinkTrait->AddImageIndex(pToggle->ID_checked);
				pHyperLinkTrait->SetToggleSelection(true);
			}
			if(bSingle) pHyperLinkTrait->SetSingleClickEdit(true);
			pTrait = pHyperLinkTrait;
		}
		break;
	case G_CTRL_TextButton:
		{
			CGridColumnTraitTextButton* pTextButtonTrait = new CGridColumnTraitTextButton;
			if(pToggle)
			{
				pTextButtonTrait->AddImageIndex(pToggle->ID_unchecked);
				pTextButtonTrait->AddImageIndex(pToggle->ID_checked);
				pTextButtonTrait->SetToggleSelection(true);
			}
			pTrait = pTextButtonTrait;
		}
		break;
	}
	return pTrait;
}
inline int G_InsColumn(CGridListCtrlGroups* G_Ctl, int nCol, const CString& strCol, int nWidth, CGridColumnTrait* pTrait, bool bSort = true, int nFormat = LVCFMT_CENTER)
{
	if(G_Ctl)
	{
		if(pTrait)
		{
			pTrait->SetSortable(bSort);
			return G_Ctl->InsertColumnTrait(nCol, strCol, nFormat, nWidth, nCol, pTrait);
		}
	}
	return -1;
}

#endif // _DEFS_GRID_LIST_H_
