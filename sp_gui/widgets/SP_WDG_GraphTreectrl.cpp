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

#include "sp_gui/widgets/SP_WDG_GraphTreectrl.h"
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

BEGIN_EVENT_TABLE(SP_WDG_GraphTreectrl, SP_WDG_Treectrl)
    EVT_TREE_SEL_CHANGED(SP_ID_GRAPHTREE_CTRL_ID, SP_WDG_GraphTreectrl::OnSelChanged)
    EVT_TREE_BEGIN_DRAG(SP_ID_GRAPHTREE_CTRL_ID, SP_WDG_Treectrl::OnBeginDrag)
    EVT_TREE_BEGIN_RDRAG(SP_ID_GRAPHTREE_CTRL_ID, SP_WDG_Treectrl::OnBeginRDrag)
    EVT_TREE_END_DRAG(SP_ID_GRAPHTREE_CTRL_ID, SP_WDG_Treectrl::OnEndDrag)
    EVT_TREE_BEGIN_LABEL_EDIT(SP_ID_GRAPHTREE_CTRL_ID, SP_WDG_Treectrl::OnBeginLabelEdit)
    EVT_TREE_END_LABEL_EDIT(SP_ID_GRAPHTREE_CTRL_ID, SP_WDG_Treectrl::OnEndLabelEdit)
    EVT_TREE_DELETE_ITEM(SP_ID_GRAPHTREE_CTRL_ID, SP_WDG_Treectrl::OnDeleteItem)
    EVT_TREE_SET_INFO(SP_ID_GRAPHTREE_CTRL_ID, SP_WDG_Treectrl::OnSetInfo)
    EVT_TREE_SEL_CHANGING(SP_ID_GRAPHTREE_CTRL_ID, SP_WDG_Treectrl::OnSelChanging)
    EVT_TREE_KEY_DOWN(SP_ID_GRAPHTREE_CTRL_ID, SP_WDG_Treectrl::OnTreeKeyDown)
    EVT_TREE_ITEM_ACTIVATED(SP_ID_GRAPHTREE_CTRL_ID, SP_WDG_Treectrl::OnDoubleClick)
    EVT_RIGHT_DOWN(SP_WDG_GraphTreectrl::OnRMouseClick)
	EVT_RIGHT_UP(SP_WDG_GraphTreectrl::OnRMouseClick)
END_EVENT_TABLE()

// SP_WDG_GraphTreectrl implementation
IMPLEMENT_DYNAMIC_CLASS(SP_WDG_GraphTreectrl, SP_WDG_Treectrl)

SP_WDG_GraphTreectrl::SP_WDG_GraphTreectrl(SP_GUI_DevbarContainer *p_pcParent,
                                 const wxWindowID p_nId,
                                 const wxPoint& p_cPos,
                                 const wxSize& p_cSize,
                                 long p_nStyle)
:SP_WDG_Treectrl(p_pcParent, p_nId, p_cPos, p_cSize, p_nStyle),
m_bRightWasDown(FALSE)
{
}

SP_WDG_GraphTreectrl::~SP_WDG_GraphTreectrl()
{
}

bool
SP_WDG_GraphTreectrl::Initialise(SP_DS_Graph* p_pcGraph)
{
    if (!p_pcGraph)
        return FALSE;

    return p_pcGraph->AddToTreectrl(this);
}

wxTreeItemId
SP_WDG_GraphTreectrl::AppendFileItem(wxTreeItemId p_cId, const wxString& p_pchLabel, SP_Type* p_pcElem)
{
    return AppendItem(p_cId, 
        p_pchLabel, 
        TREE_CTRL_ICON_File, 
        TREE_CTRL_ICON_File + 1, 
        new SP_GraphTreeItemdata(p_pchLabel, p_pcElem));
}

void
SP_WDG_GraphTreectrl::OnSelChanged(wxTreeEvent& p_cEvent)
{
    // show some info about this item
    wxTreeItemId l_nId = p_cEvent.GetItem();

    SP_GraphTreeItemdata *l_pcData = dynamic_cast<SP_GraphTreeItemdata*>(GetItemData(l_nId));
    
    if (l_pcData)
    {
        if (m_pcOwner)
        {
            m_pcOwner->SetElement(l_pcData->GetElement());
        }
    }
}

void
SP_WDG_GraphTreectrl::OnRMouseClick(wxMouseEvent& p_cEvent)
{
    if (!m_bRightWasDown)
    {
        m_bRightWasDown = TRUE;
        return;
    }

    m_bRightWasDown = FALSE;

    wxTreeItemId l_nId = HitTest(p_cEvent.GetPosition());
    
    

    SP_GraphTreeItemdata *l_pcData = dynamic_cast<SP_GraphTreeItemdata*>(GetItemData(l_nId));
    // only nodes in the tree, that represent a class
    if (l_pcData && l_pcData->GetElement())
    {
        SelectItem(l_nId);
	// with SelectItem the following lines become not necessary, 
	// OnSelChanged will be called
        //if (m_pcOwner) 
        //    m_pcOwner->SetElement(l_pcData->GetElement());

        // intentionally created on the stack
        wxMenu l_cMenu;
        l_cMenu.Append(SP_MENU_ITEM_TREE_SELECT_ALL, wxT("Select all of this class"));
        l_cMenu.Append(SP_MENU_ITEM_TREE_EDIT_PROPERTIES, wxT("Edit selected"));

        PopupMenu(&l_cMenu, p_cEvent.GetPosition());
    }
}

void
SP_WDG_GraphTreectrl::SelectItemByLabel(const wxString& p_label)
{
  wxTreeItemId id = FindItemRec(GetRootItem(), p_label);
  if (id.IsOk()) {
    SelectItem(id);
  } else {
    SelectItem(GetRootItem());
  }
}
