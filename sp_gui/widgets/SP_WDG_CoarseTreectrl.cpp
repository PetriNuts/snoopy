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

#include "sp_gui/widgets/SP_WDG_CoarseTreectrl.h"
#include "sp_gui/windows/SP_GUI_DevbarContainer.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"

#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"

#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/extensions/SP_DS_Coarse.h"

#include "sp_core/SP_Core.h"

#include "sp_utilities.h"


wxTreeItemId
SP_WDG_CoarseTreectrl::GetNewParentItem(wxTreeItemId p_nId, unsigned int p_nNr, wxTreeItemIdValue  p_nCookie)
{
    wxTreeItemId l_nId = p_nId;

    SP_CoarseTreeItemdata* l_pcData = dynamic_cast<SP_CoarseTreeItemdata*>(GetItemData(l_nId));
    if (l_pcData && l_pcData->GetNetnumber() == p_nNr)
        return l_nId;

    if (! p_nCookie )
        l_nId = GetFirstChild(p_nId, p_nCookie);
    else
        l_nId = GetNextChild(p_nId, p_nCookie);

    if (!l_nId.IsOk())
        return l_nId;

    if (ItemHasChildren(p_nId))
    {
        wxTreeItemIdValue l_nCookie = NULL;
        l_nId = GetNewParentItem(l_nId, p_nNr, l_nCookie);
    }

    if ( ! l_nId )
        l_nId = GetNewParentItem(p_nId, p_nNr, p_nCookie);

    return l_nId;
}

BEGIN_EVENT_TABLE(SP_WDG_CoarseTreectrl, SP_WDG_Treectrl)
    EVT_TREE_ITEM_ACTIVATED(SP_ID_COARSETREE_CTRL_ID,
			    SP_WDG_CoarseTreectrl::OnDoubleClick)
#ifdef __WXMSW__
  EVT_TREE_ITEM_COLLAPSING(SP_ID_COARSETREE_CTRL_ID,
			   SP_WDG_CoarseTreectrl::OnCollapsingExpanding)
  EVT_TREE_ITEM_EXPANDING(SP_ID_COARSETREE_CTRL_ID,
			  SP_WDG_CoarseTreectrl::OnCollapsingExpanding)
#endif
END_EVENT_TABLE()

// SP_WDG_CoarseTreectrl implementation
IMPLEMENT_DYNAMIC_CLASS(SP_WDG_CoarseTreectrl, SP_WDG_Treectrl)

SP_WDG_CoarseTreectrl::SP_WDG_CoarseTreectrl(SP_GUI_DevbarContainer *p_pcParent,
                                 const wxWindowID p_nId,
                                 const wxPoint& p_cPos,
                                 const wxSize& p_cSize,
                                 long p_nStyle)
:SP_WDG_Treectrl(p_pcParent, p_nId, p_cPos, p_cSize, p_nStyle)
{
    AppendFileItem(1, NULL);

    m_pcParent->AddTree(this);

#ifdef __WXMSW__
    m_ActivatedItem = wxTreeItemId();
#endif

}

SP_WDG_CoarseTreectrl::~SP_WDG_CoarseTreectrl()
{
}

wxTreeItemId
SP_WDG_CoarseTreectrl::AppendFileItem(unsigned int p_nNetnumber,
                                      SP_DS_Coarse* p_pcCoarse)
{
    wxTreeItemId l_nId = GetRootItem(), l_nRet;
    wxString l_sLabel = wxT("Top Level");
    wxTreeItemIdValue l_nCookie = NULL;

    if (p_pcCoarse)
    {
        l_nId = GetNewParentItem(l_nId, p_nNetnumber, l_nCookie);
        l_sLabel = p_pcCoarse->GetLabelAttributeLabel();
    }
    else
    {
        l_nRet = AppendItem(l_nId,
            l_sLabel,
            TREE_CTRL_ICON_File,
            TREE_CTRL_ICON_File + 1,
            new SP_CoarseTreeItemdata(l_sLabel, p_pcCoarse));

        Expand(l_nRet);
		m_curTreeItem = l_nRet;
		SelectItem(m_curTreeItem);
        return l_nRet;
    }

    if (l_nId.IsOk() && p_pcCoarse)
    {

			l_nRet = AppendItem(l_nId,
            l_sLabel,
            TREE_CTRL_ICON_File,
            TREE_CTRL_ICON_File + 1,
            new SP_CoarseTreeItemdata(l_sLabel, p_pcCoarse));

        Expand(l_nId);
		SortChildren(l_nId);
        return l_nRet;
    }
    return l_nId;
}

void
SP_WDG_CoarseTreectrl::UpdateEntry(wxTreeItemId& p_nId, SP_DS_Coarse* p_pcCoarse)
{
    if (!p_pcCoarse || !p_nId.IsOk())
        return;

    wxString l_pchVal = p_pcCoarse->GetLabelAttributeLabel();
    SetItemText(p_nId, l_pchVal);
    SortChildren(GetItemParent(p_nId));
}

bool
SP_WDG_CoarseTreectrl::InvalidateAllChildren(wxTreeItemId& p_nId)
{
    if (!p_nId.IsOk() || !GetItemData(p_nId))
        return FALSE;

    wxTreeItemId l_nId;
    bool l_bReturn = TRUE;
    SP_DS_Coarse* l_pcCoarse;
    SP_CoarseTreeItemdata* l_pcMyData;
    wxTreeItemIdValue l_nCookie= NULL;
    l_pcMyData = dynamic_cast<SP_CoarseTreeItemdata*>(GetItemData(p_nId));
    l_pcCoarse = l_pcMyData->GetCoarse();

    l_nId = GetFirstChild(p_nId, l_nCookie);
    while (l_nId.IsOk())
    {
        l_bReturn = InvalidateAllChildren(l_nId);
        l_nId = GetNextChild(p_nId, l_nCookie);
    }

    l_nId.m_pItem = 0;
    l_pcCoarse->SetCoarseId(l_nId);
    l_pcCoarse->SetUpdate(TRUE);
    return l_bReturn;
}

wxTreeItemId
SP_WDG_CoarseTreectrl::FindDocRec(const wxTreeItemId& p_Id,
				       const SP_MDI_CoarseDoc* p_pcDoc)
{
  SP_DS_Coarse* l_pcCoarse;
  SP_CoarseTreeItemdata* l_pcMyData;
  wxTreeItemIdValue cookie= NULL;
  wxTreeItemId l_nId = GetFirstChild(p_Id, cookie);
  wxTreeItemId found = wxTreeItemId();

  while (l_nId.IsOk()) {
      l_pcMyData = dynamic_cast<SP_CoarseTreeItemdata*>(GetItemData(l_nId));
      l_pcCoarse = l_pcMyData->GetCoarse();
      if (l_pcCoarse && l_pcCoarse->GetCoarseDoc() == p_pcDoc) {
	found = l_nId;
	break;
      } else {
	found = FindDocRec(l_nId, p_pcDoc);
	if (found.IsOk()) break;
      }
      l_nId = GetNextChild(p_Id, cookie);
  }
  return found;
}

void
SP_WDG_CoarseTreectrl::SetCurrentDoc(const SP_MDI_CoarseDoc* p_pcDoc, bool p_show)
{
  wxTreeItemId tId = GetRootItem();

  SP_LOGDEBUG(wxString::Format(wxT("SetCurrentDoc (%p)"), p_pcDoc));

  if (p_pcDoc) {
    tId = FindDocRec(GetRootItem(), p_pcDoc);
    if (! tId.IsOk()) {
      SP_LOGDEBUG(wxString::Format(wxT("Could not find CoarseDoc (%p)"),
				     p_pcDoc));
    }
  }
  ShowCoarseDoc(tId, p_show);
}


void
SP_WDG_CoarseTreectrl::HierarchyGoUp()
{
  if (m_curTreeItem == GetRootItem()) {
    ShowCoarseDoc(m_curTreeItem);
  } else {
    wxTreeItemId tId = GetItemParent(m_curTreeItem);
    if (tId.IsOk()) ShowCoarseDoc(tId);
  }
}

void
SP_WDG_CoarseTreectrl::HierarchyGoToFirstChild()
{
  wxTreeItemIdValue cookie=NULL;
  wxTreeItemId tId = GetFirstChild(m_curTreeItem, cookie);
  if (tId.IsOk()) ShowCoarseDoc(tId);
}

void
SP_WDG_CoarseTreectrl::HierarchyGoToNextSibling()
{
  wxTreeItemId tId = GetNextSibling(m_curTreeItem);
  if (tId.IsOk()) ShowCoarseDoc(tId);
}

void
SP_WDG_CoarseTreectrl::HierarchyGoToPrevSibling()
{
  wxTreeItemId tId = GetPrevSibling(m_curTreeItem);
  if (tId.IsOk()) ShowCoarseDoc(tId);
}

void
SP_WDG_CoarseTreectrl::ShowCoarseDoc(const wxTreeItemId& p_Id, bool p_show)
{
	if (p_Id != m_curTreeItem)
	{
		if (p_Id.IsOk())
		{
			m_curTreeItem = p_Id;
			if (p_show)
			{
				SP_CoarseTreeItemdata* l_pcMyData =
					dynamic_cast<SP_CoarseTreeItemdata*>(GetItemData(p_Id));

				if (l_pcMyData->GetCoarse())
				{
					l_pcMyData->GetCoarse()->Show();
				}
				else
				{
					SP_Core::Instance()->GetRootDocument()->ShowAll();
				}
			}
		}
	}
	SelectItem(m_curTreeItem);
}

void
SP_WDG_CoarseTreectrl::OnDoubleClick(wxTreeEvent& p_cEvent)
{
#ifdef __WXMSW__
  m_ActivatedItem = p_cEvent.GetItem();
#endif
  ShowCoarseDoc(p_cEvent.GetItem());
}

// on msw dbl click collapses/expands an item, we will check
// the situation and reject it
#ifdef __WXMSW__
void
SP_WDG_CoarseTreectrl::OnCollapsingExpanding(wxTreeEvent& p_cEvent)
{
  if (m_ActivatedItem == p_cEvent.GetItem()) {
    m_ActivatedItem = wxTreeItemId();
    p_cEvent.Veto();
  }
}
#endif

//------------------------------------------------------------------------
SP_CoarseTreeItemdata::SP_CoarseTreeItemdata(const wxString& p_sDesc,
                                             SP_DS_Coarse* p_pcCoarse)
:SP_TreeItemdata(p_sDesc), m_pcCoarse(p_pcCoarse), m_nNetnumber(1)
{
    if (m_pcCoarse)
    {
        m_nNetnumber = m_pcCoarse->GetNetnumber();
    }
}
