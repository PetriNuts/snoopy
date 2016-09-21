//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include <wx/imaglist.h>
#include <wx/image.h>

#include "sp_core/base/SP_Type.h"

#include "sp_gui/widgets/SP_WDG_Treectrl.h"
#include "sp_gui/windows/SP_GUI_DevbarContainer.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Edgeclass.h"

#include "sp_utilities.h"


#ifdef __WIN32__
// this is not supported by native control
#define NO_VARIABLE_HEIGHT
#endif

// under Windows the icons are in the .rc file
// else, we have to include them manually
#ifndef __WXMSW__
#include "bitmaps/file_norm.xpm"
#include "bitmaps/file_sel.xpm"
#include "bitmaps/folder_norm.xpm"
#include "bitmaps/folder_sel.xpm"
#include "bitmaps/folder_open.xpm"
#endif

BEGIN_EVENT_TABLE(SP_WDG_Treectrl, wxTreeCtrl)
END_EVENT_TABLE()

// SP_WDG_Treectrl implementation
IMPLEMENT_DYNAMIC_CLASS(SP_WDG_Treectrl, wxTreeCtrl)

SP_WDG_Treectrl::SP_WDG_Treectrl(SP_GUI_DevbarContainer *p_pcParent,
		const wxWindowID p_nId, const wxPoint& p_cPos, const wxSize& p_cSize,
		long p_nStyle) :
	wxTreeCtrl((wxWindow*)p_pcParent, p_nId, p_cPos, p_cSize, p_nStyle | wxNO_FULL_REPAINT_ON_RESIZE),
m_pcImageList(NULL),
m_pcOwner(NULL),
m_pcParent(p_pcParent)
{
	CreateImageList(16);
}

SP_WDG_Treectrl::~SP_WDG_Treectrl()
{
	if (m_pcImageList)
	{
		wxDELETE(m_pcImageList);
		m_pcImageList = NULL;
	}
}

bool SP_WDG_Treectrl::SetOwner(SP_GUI_Childframe* p_pcVal)
{
	if (!m_pcParent)
		return FALSE;

	m_pcOwner = p_pcVal;

	return m_pcParent->AddTree(this);
}

bool SP_WDG_Treectrl::BringToFront()
{
	if (!m_pcParent)
		return FALSE;

	return m_pcParent->ShowTree(this);
}

bool SP_WDG_Treectrl::Remove()
{
	if (!m_pcParent)
		return FALSE;
	return m_pcParent->RemoveTree(this);
}

void SP_WDG_Treectrl::CreateImageList(int p_nSize)
{
	if (m_pcImageList)
		m_pcImageList->RemoveAll();

	wxDELETE(m_pcImageList);

	if (p_nSize == -1)
	{
		m_pcImageList = NULL;
		SetImageList(m_pcImageList);
		return;
	}

	// Make an l_nImageId list containing small icons
	wxSize l_cSize(p_nSize, p_nSize);
	m_pcImageList = new wxImageList(p_nSize, p_nSize, TRUE);

	// should correspond to TREE_CTRL_ICON_xxx enum
#if defined(__WXMSW__)
	m_pcImageList->Add(wxBitmap(wxT("bitmap_file_norm"), wxBITMAP_TYPE_BMP_RESOURCE));
	m_pcImageList->Add(wxBitmap(wxT("bitmap_file_sel"), wxBITMAP_TYPE_BMP_RESOURCE));
	m_pcImageList->Add(wxBitmap(wxT("bitmap_folder_norm"), wxBITMAP_TYPE_BMP_RESOURCE));
	m_pcImageList->Add(wxBitmap(wxT("bitmap_folder_sel"), wxBITMAP_TYPE_BMP_RESOURCE));
	m_pcImageList->Add(wxBitmap(wxT("bitmap_folder_open"), wxBITMAP_TYPE_BMP_RESOURCE));
#else // !WIN16
	wxImage l_aImages[5];
	l_aImages[0] = wxImage(file_norm_xpm);

	l_aImages[1] = wxImage(file_sel_xpm);

	l_aImages[2] = wxImage(folder_norm_xpm);

	l_aImages[3] = wxImage(folder_sel_xpm);

	l_aImages[4] = wxImage(folder_open_xpm);

	for (size_t i = 0; i < WXSIZEOF(l_aImages); i++)
	{
		l_aImages[i].Rescale(p_nSize, p_nSize);

		m_pcImageList->Add(wxBitmap(l_aImages[i]));

	}
#endif // MSW/!MSW
	SetImageList(m_pcImageList);
}

bool SP_WDG_Treectrl::DeselectAll()
{
	SelectItem(GetRootItem());

	return TRUE;
}

void SP_WDG_Treectrl::DoToggleIcon(const wxTreeItemId& p_cId)
{
	int l_nImageId =
			GetItemImage(p_cId) == TREE_CTRL_ICON_Folder ? TREE_CTRL_ICON_File
					: TREE_CTRL_ICON_Folder;

	SetItemImage(p_cId, l_nImageId);
}

void SP_WDG_Treectrl::OnBeginDrag(wxTreeEvent& p_cEvent)
{
}

void SP_WDG_Treectrl::OnEndDrag(wxTreeEvent& p_cEvent)
{
}

void SP_WDG_Treectrl::OnDoubleClick(wxTreeEvent& p_cEvent)
{
	wxTreeItemId l_nId = p_cEvent.GetItem();
	if (l_nId.IsOk())
	{
		SP_TreeItemdata* l_pcData =
				dynamic_cast<SP_TreeItemdata*>(GetItemData(l_nId));
		if (l_pcData)
			l_pcData->OnDoubleClick(this);
	}
}

void SP_WDG_Treectrl::OnRMouseClick(wxMouseEvent& p_cEvent)
{
}

wxTreeItemId SP_WDG_Treectrl::FindItemRec(const wxTreeItemId& p_Id,
		const wxString& p_label)
{
	wxTreeItemIdValue cookie=NULL;
	wxTreeItemId l_nId = GetFirstChild(p_Id, cookie);
	wxTreeItemId found = wxTreeItemId();
	while (l_nId.IsOk())
	{
		if (GetItemText(l_nId) == p_label)
		{
			found = l_nId;
			break;
		}
		else
		{
			found = FindItemRec(l_nId, p_label);
			if (found.IsOk())
				break;
		}
		l_nId = GetNextChild(p_Id, cookie);
	}
	return found;
}

// avoid repetition
#define TREE_EVENT_HANDLER(name)               \
    void SP_WDG_Treectrl::name(wxTreeEvent& p_cEvent) \
{                                              \
    p_cEvent.Skip();                              \
}

TREE_EVENT_HANDLER(OnBeginRDrag)
TREE_EVENT_HANDLER(OnDeleteItem)
TREE_EVENT_HANDLER(OnGetInfo)
TREE_EVENT_HANDLER(OnSetInfo)
TREE_EVENT_HANDLER(OnSelChanging)
TREE_EVENT_HANDLER(OnTreeKeyDown)

#undef TREE_EVENT_HANDLER

void SP_WDG_Treectrl::OnBeginLabelEdit(wxTreeEvent& p_cEvent)
{
}

void SP_WDG_Treectrl::OnEndLabelEdit(wxTreeEvent& p_cEvent)
{
}

wxTreeItemData* SP_WDG_Treectrl::GetItemData(const wxTreeItemId& p_cItem) const
{
	if (p_cItem.IsOk())
		return wxTreeCtrl::GetItemData(p_cItem);

	return NULL;
}

