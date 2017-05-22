//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include <wx/imaglist.h>
#include <wx/image.h>

#include "sp_core/base/SP_Type.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include "sp_gui/dialogs/dia_CPN/SP_WDG_DeclarationTreectrl.h"
#include "sp_gui/windows/SP_GUI_DevbarContainer.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"

#include "sp_gui/dialogs/dia_CPN/SP_DLG_ColorSetSetting.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_ConstantDefinition.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_VariableDefinition.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_FunctionDefinition.h"

// bysl
#include "sp_gui/dialogs/SP_DLG_NewConstantDefinition.h"
#include "sp_gui/dialogs/SP_DLG_NewObserverDefinition.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"

#include "sp_gui/mdi/SP_MDI_CoarseDoc.h"

#include "sp_ds/SP_DS_Graph.h"


#include "sp_ds/extensions/SP_DS_Coarse.h"
#include "sp_core/SP_Core.h"
#include "sp_utilities.h"

#include "sp_ds/attributes/SP_DS_TextAttribute.h"



BEGIN_EVENT_TABLE(SP_WDG_DeclarationTreectrl, SP_WDG_Treectrl)

    EVT_TREE_ITEM_ACTIVATED(SP_ID_COLORSETTREE_CTRL_ID,  SP_WDG_DeclarationTreectrl::OnDoubleClick)
	EVT_TREE_ITEM_GETTOOLTIP(SP_ID_COLORSETTREE_CTRL_ID,  SP_WDG_DeclarationTreectrl::OnToolTip)

#ifdef __WXMSW__

  EVT_TREE_ITEM_COLLAPSING(SP_ID_COLORSETTREE_CTRL_ID, SP_WDG_DeclarationTreectrl::OnCollapsingExpanding)
  EVT_TREE_ITEM_EXPANDING(SP_ID_COLORSETTREE_CTRL_ID,  SP_WDG_DeclarationTreectrl::OnCollapsingExpanding)

#endif
END_EVENT_TABLE()



// SP_WDG_DeclarationTreectrl implementation
IMPLEMENT_DYNAMIC_CLASS(SP_WDG_DeclarationTreectrl, SP_WDG_Treectrl)

SP_WDG_DeclarationTreectrl::SP_WDG_DeclarationTreectrl(SP_GUI_DevbarContainer *p_pcParent,
                                 const wxWindowID p_nId,
                                 const wxPoint& p_cPos,
                                 const wxSize& p_cSize,
                                 long p_nStyle)
:SP_WDG_Treectrl(p_pcParent, p_nId, p_cPos, p_cSize, p_nStyle)
{	
    m_pcParent->AddTree(this);

#ifdef __WXMSW__
    m_ActivatedItem = wxTreeItemId();
#endif

}

SP_WDG_DeclarationTreectrl::~SP_WDG_DeclarationTreectrl()
{
}

void
SP_WDG_DeclarationTreectrl::OnDoubleClick(wxTreeEvent& p_cEvent)
{

	wxString l_label=GetItemText(p_cEvent.GetItem());
	if(p_cEvent.GetItem() == GetRootItem())
	{
		ExpandAllChildren(GetRootItem());
	}
	if(l_label==wxT("Color Sets"))	
	{
		SP_DLG_ColorSetSetting l_cColorSetDlg(NULL,BASICCOLORSETGRID);
		l_cColorSetDlg.ShowModal();
	}
	//if(l_label==wxT("Basic Colorsets"))
	if(l_label== wxT("Simple Color Sets"))
	{
		SP_DLG_ColorSetSetting l_cColorSetDlg(NULL,BASICCOLORSETGRID);
		l_cColorSetDlg.ShowModal();
	}
	if(l_label== wxT("Compound Color Sets"))
	{
		SP_DLG_ColorSetSetting l_cColorSetDlg(NULL,STRUCTUREDCOLORSETGRID);
		l_cColorSetDlg.ShowModal();
	}

	//if(l_label==wxT("Alias Colorsets"))
	if(l_label== wxT("Alias Color Sets"))
	{
		SP_DLG_ColorSetSetting l_cColorSetDlg(NULL,ALIASCOLORSETGRID);
		l_cColorSetDlg.ShowModal();
	}
	
	if(l_label== wxT("Constants"))
	{
		SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
		if(!l_pcGraph)
			return;

		wxString l_sNetClassName = l_pcGraph->GetNetclass()->GetName();

		if (l_sNetClassName.Contains(wxT("Colored")))
		{
			SP_DLG_ConstantDefinition l_cConstantDlg(NULL);
			l_cConstantDlg.ShowModal();
		}
		else
		{
			SP_DLG_NewConstantDefinition l_cConstantDlg(NULL);
			l_cConstantDlg.ShowModal();
		}

	}

	if(l_label== wxT("Variables"))
	{
		SP_DLG_VariableDefinition l_cVariableDlg(NULL);
		l_cVariableDlg.ShowModal();
	}

	if (l_label == wxT("Functions"))
	{
		SP_DLG_FunctionDefinition l_cFunctionDlg(NULL);
		l_cFunctionDlg.ShowModal();
	}

	if (l_label == wxT("Observers"))
	{
		SP_DLG_NewObserverDefinition l_cFunctionDlg(NULL);
		l_cFunctionDlg.ShowModal();
	}

/*
#ifdef __WXMSW__
  m_ActivatedItem = p_cEvent.GetItem();
#endif
  ShowCoarseDoc(p_cEvent.GetItem());
  */
}



// on msw dbl click collapses/expands an item, we will check
// the situation and reject it
#ifdef __WXMSW__
void
SP_WDG_DeclarationTreectrl::OnCollapsingExpanding(wxTreeEvent& p_cEvent)
{
  if (m_ActivatedItem == p_cEvent.GetItem()) {
    m_ActivatedItem = wxTreeItemId();
    p_cEvent.Veto();
  }
}
#endif


bool SP_WDG_DeclarationTreectrl::Initialise(SP_DS_Graph* p_pcGraph)
{
	if (!p_pcGraph)
        return FALSE;

    return p_pcGraph->AddToDeclarationTreectrl(this);
}

wxTreeItemId
SP_WDG_DeclarationTreectrl::AppendFileItem(wxTreeItemId p_cId, const wxString& p_pchLabel, SP_Type* p_pcElem)
{
    return AppendItem(p_cId, 
        p_pchLabel, 
        TREE_CTRL_ICON_File, 
        TREE_CTRL_ICON_File + 1, 
        new SP_DeclarationTreeItemdata(p_pchLabel, p_pcElem));
}

//bysl
void
SP_WDG_DeclarationTreectrl::UpdateOtherTree()
{ 
	wxTreeItemId l_cRootId=GetRootItem();
	wxTreeItemIdValue l_nCookie= NULL;

	m_ColorsMap.clear();

	GetFirstChild(l_cRootId,l_nCookie);
	wxTreeItemId l_cSecondChild=GetFirstChild(l_cRootId,l_nCookie);
	DeleteChildren(l_cSecondChild);

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if(!l_pcGraph)
		return;

	SP_DS_Metadataclass* l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);
	if(l_pcMetadataclass)
	{
		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin();
				l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcMetadata = *l_itElem;
			wxString l_sMetadataName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			wxString l_sMetadataGroup = dynamic_cast<SP_DS_TextAttribute*>(l_pcMetadata->GetAttribute(wxT("Group")))->GetValue();
			//wxString l_sMetadataValue = dynamic_cast<SP_DS_TextAttribute*>(l_pcMetadata->GetAttribute(wxT("Value")))->GetValue();
			SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcMetadata->GetAttribute(wxT("ValueList")));
			wxString l_sMetadataValue;
			for(unsigned int i = 0; i < l_pcColList->GetRowCount(); ++i)
			{
				l_sMetadataValue << l_pcColList->GetCell(i,1) << wxT(";");
			}
			m_ColorsMap[l_sMetadataName] = l_sMetadataGroup + wxT(" : ") + l_sMetadataValue;
			AppendFileItem(l_cSecondChild,l_sMetadataName + wxT(" : ") + l_sMetadataValue,NULL);
		}
	}

	wxTreeItemId l_cThirdChild = GetNextChild(l_cRootId,l_nCookie);
	DeleteChildren(l_cThirdChild);

	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_META_FUNCTION);
	if (l_pcMetadataclass)
	{
		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin();
			l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcMetadata = *l_itElem;
			wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();
			wxString l_sParam = l_pcMetadata->GetAttribute(wxT("Parameter"))->GetValueString();
			wxString l_sBody = l_pcMetadata->GetAttribute(wxT("Body"))->GetValueString();
			m_ColorsMap[l_sName] = l_sName + wxT("(") + l_sParam + wxT(") = ") + l_sBody;
			AppendFileItem(l_cThirdChild, l_sName + wxT("(") + l_sParam + wxT(") = ") + l_sBody, NULL);
		}
	}

	wxTreeItemId l_cFourthChild = GetNextChild(l_cRootId, l_nCookie);
	DeleteChildren(l_cFourthChild);

	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_META_OBSERVER);
	if (l_pcMetadataclass)
	{
		SP_ListMetadata::const_iterator l_itElem;
		for (l_itElem = l_pcMetadataclass->GetElements()->begin();
			l_itElem != l_pcMetadataclass->GetElements()->end(); ++l_itElem)
		{
			SP_DS_Metadata* l_pcMetadata = *l_itElem;
			wxString l_sName = l_pcMetadata->GetAttribute(wxT("Name"))->GetValueString();
			wxString l_sBody = l_pcMetadata->GetAttribute(wxT("Body"))->GetValueString();
			m_ColorsMap[l_sName] = l_sName + wxT(": ") + l_sBody;
			AppendFileItem(l_cFourthChild, l_sName + wxT(" = ") + l_sBody, NULL);
		}
	} 
}


void
SP_WDG_DeclarationTreectrl::UpdateColorSetTree()
{
	wxTreeItemId l_cRootId;
	l_cRootId=GetRootItem();
	wxTreeItemIdValue l_nCookie= NULL;

	m_ColorsMap.clear();

	map<wxString, wxString> l_mCSName2PlaceColor;

	///colorset
	wxTreeItemId l_cFirstChild=GetFirstChild(l_cRootId,l_nCookie);
    wxTreeItemId l_cFirstChild_FirstChild=GetFirstChild(l_cFirstChild,l_nCookie);
	DeleteChildren(l_cFirstChild_FirstChild);
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if(!l_pcGraph)
		return;

	SP_DS_Metadataclass* l_pcMetadataclass = NULL;
	SP_DS_Metadata* l_pcNewMetadata = NULL;

	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return;

	if(l_pcMetadataclass->GetElements()->empty())
		return;

	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
	if(!l_pcColList)
		return;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName=l_pcColList->GetCell(i,0).c_str();
		wxString l_sColors=l_pcColList->GetCell(i,2).c_str();
		m_ColorsMap[l_sName] = l_sColors;

		wxString l_sPlaceColor=l_pcColList->GetCell(i,3).c_str();
		wxString l_sShow=l_pcColList->GetCell(i,4);
		if( l_sShow == wxT("1"))
			l_mCSName2PlaceColor[l_sName] = l_sPlaceColor;

		AppendFileItem(l_cFirstChild_FirstChild,l_sName,NULL);			
	}

    wxTreeItemId l_cFirstChild_SecondChild=GetNextChild(l_cFirstChild,l_nCookie);
	DeleteChildren(l_cFirstChild_SecondChild);
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return;

	if(l_pcMetadataclass->GetElements()->empty())
		return;

	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));
	if(!l_pcColList)
		return;
	
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{		
		wxString l_sName=l_pcColList->GetCell(i,0).c_str();
		wxString l_sColors=l_pcColList->GetCell(i,2).c_str();
		m_ColorsMap[l_sName] = l_sColors;

		wxString l_sPlaceColor=l_pcColList->GetCell(i,4).c_str();
		wxString l_sShow=l_pcColList->GetCell(i,5);
		if( l_sShow == wxT("1"))
			l_mCSName2PlaceColor[l_sName] = l_sPlaceColor;

		AppendFileItem(l_cFirstChild_SecondChild,l_sName,NULL);
	}


	wxTreeItemId l_cFirstChild_FourthChild=GetNextChild(l_cFirstChild,l_nCookie);
	DeleteChildren(l_cFirstChild_FourthChild);
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_ALIASCOLORSETCLASS);
	if(!l_pcMetadataclass)
		return;

	if(l_pcMetadataclass->GetElements()->empty())
		return;

	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AliasColorsetList")));
	if(!l_pcColList)
		return;
	
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName=l_pcColList->GetCell(i,0).c_str();
		wxString l_sColors=l_pcColList->GetCell(i,1).c_str();
		m_ColorsMap[l_sName] = l_sColors;
		AppendFileItem(l_cFirstChild_FourthChild,l_sName,NULL);
	}

    l_cRootId=GetRootItem();
	GetFirstChild(l_cRootId,l_nCookie);
	wxTreeItemId l_cSecondChild=GetNextChild(l_cRootId,l_nCookie);
    DeleteChildren(l_cSecondChild);
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);
	if(!l_pcMetadataclass)
		return;

	if(l_pcMetadataclass->GetElements()->empty())
		return;

	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));
	if(!l_pcColList)
		return;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName=l_pcColList->GetCell(i,0).c_str();
		wxString l_sType=l_pcColList->GetCell(i,1).c_str();
		wxString l_sValue=l_pcColList->GetCell(i,2).c_str();
		m_ColorsMap[l_sName] = l_sType + wxT(":") + l_sValue;
		AppendFileItem(l_cSecondChild,l_sName + wxT(":") + l_sValue,NULL);
	}

    wxTreeItemId l_cThirdChild=GetNextChild(l_cRootId,l_nCookie);
	DeleteChildren(l_cThirdChild);
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
	if(!l_pcMetadataclass)
		return;

	if(l_pcMetadataclass->GetElements()->empty())
		return;

	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
	if(!l_pcColList)
		return;
	
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName=l_pcColList->GetCell(i,1).c_str();
		wxString l_sColors=l_pcColList->GetCell(i,0) + wxT(" ") + l_pcColList->GetCell(i,1) + wxT("( ") +
			l_pcColList->GetCell(i,2) + wxT(") : ") + l_pcColList->GetCell(i,3);
		m_ColorsMap[l_sName] = l_sColors;
		AppendFileItem(l_cThirdChild,l_sName,NULL);
	}

	wxTreeItemId l_cFourthChild=GetNextChild(l_cRootId,l_nCookie);
	DeleteChildren(l_cFourthChild);
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);
	if(!l_pcMetadataclass)
		return;

	if(l_pcMetadataclass->GetElements()->empty())
		return;

	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
	if(!l_pcColList)
		return;
	
	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName=l_pcColList->GetCell(i,0).c_str();
		wxString l_sColors=l_pcColList->GetCell(i,1).c_str();
		m_ColorsMap[l_sName] = l_sColors;
		AppendFileItem(l_cFourthChild,l_sName,NULL);
	}


	//update the real color of a place
	SP_GUI_Canvas *l_cCanvas = NULL;
	wxString l_sNetClassName = l_pcGraph->GetNetclass()->GetName();
	wxString l_sPlaceNodeClassName;

	if(l_sNetClassName == SP_DS_COLCPN_CLASS)
		l_sPlaceNodeClassName = SP_DS_CONTINUOUS_PLACE;	
	else	
		l_sPlaceNodeClassName = SP_DS_DISCRETE_PLACE;

	SP_DS_Nodeclass* l_pcNodeclass = l_pcGraph->GetNodeclass( l_sPlaceNodeClassName );
	SP_ListNode::const_iterator l_itElem;
	bool l_bCanvas = true;
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcPlaceNode = (*l_itElem);
		SP_DS_TextAttribute* l_pcTextAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcPlaceNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
		if( !l_pcTextAttibute )
			return;
		wxString l_sColorSetName = l_pcTextAttibute->GetValue();

		wxString l_sPlaceRColor;
		if(l_mCSName2PlaceColor.find(l_sColorSetName) != l_mCSName2PlaceColor.end() )
		   l_sPlaceRColor = l_mCSName2PlaceColor.find(l_sColorSetName)->second;
		else
			l_sPlaceRColor = wxT("WHITE");
		wxColour l_PlaceRColor = wxColour( l_sPlaceRColor);

		SP_ListGraphic::iterator l_Iter;
		SP_ListGraphic* l_PlaceGraphicsList = l_pcPlaceNode->GetGraphics();
		if( l_bCanvas)
		{
			l_cCanvas = (*(l_PlaceGraphicsList->begin()))->GetCanvas();							
			l_bCanvas = false;
		}
		for (l_Iter = l_PlaceGraphicsList->begin(); l_Iter != l_PlaceGraphicsList->end(); ++l_Iter)
		{
			wxBrush* l_pcBrush= wxTheBrushList->FindOrCreateBrush(l_PlaceRColor, (*l_Iter)->GetBrush()->GetStyle());
			(*l_Iter)->SetBrush(l_pcBrush);
			(*l_Iter)->Update(true);			
		}	
	}

	if (l_cCanvas && l_pcNodeclass->GetElements()->size()>0)
	{
		wxClientDC l_cDC(l_cCanvas);
		l_cCanvas->DoPrepareDC(l_cDC);
		l_cCanvas->Refresh();
	}
	

	
	
}


void SP_WDG_DeclarationTreectrl::OnToolTip(wxTreeEvent& p_cEvent)
{
	wxString l_sColorsetName = this->GetItemText(p_cEvent.GetItem());
	if(m_ColorsMap.find(l_sColorsetName) != m_ColorsMap.end() )
	{
		wxString l_sColors = m_ColorsMap.find(l_sColorsetName)->second;
		p_cEvent.SetToolTip(l_sColors);
	}

}

void SP_WDG_DeclarationTreectrl::DeleteChildren(const wxTreeItemId &item) {
	if(item.IsOk()) {
		SP_WDG_Treectrl::DeleteChildren(item);
	}
}
