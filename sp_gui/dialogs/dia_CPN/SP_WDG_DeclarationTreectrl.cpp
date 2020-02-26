//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// @ Modified by George Assaf mainly for unused declaration 
// Short Description:
//////////////////////////////////////////////////////////////////////
#include <wx/imaglist.h>
#include <wx/image.h>

#include "sp_ds/attributes/SP_DS_TypeAttribute.h"
#include "sp_core/base/SP_Type.h"

#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

#include "sp_gui/dialogs/dia_CPN/SP_WDG_DeclarationTreectrl.h"
#include "sp_gui/windows/SP_GUI_DevbarContainer.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/dialogs/dia_FPN/SP_DLG_FpnConstantDefinition.h"

#include "sp_gui/dialogs/dia_CPN/SP_DLG_ColorSetSetting.h"
#include "sp_gui/dialogs/dia_CPN/SP_DLG_ConstantDefinition.h"
#include "sp_gui/dialogs/dia_FPN/SP_DLG_FpnConstantDefinition.h"
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
#include "sp_ds/extensions/unusedDeclarationsFeature/SP_DS_UnUsedDeclaration.h"
 

BEGIN_EVENT_TABLE(SP_WDG_DeclarationTreectrl, SP_WDG_Treectrl)

EVT_TREE_ITEM_ACTIVATED(SP_ID_COLORSETTREE_CTRL_ID, SP_WDG_DeclarationTreectrl::OnDoubleClick)
EVT_TREE_ITEM_GETTOOLTIP(SP_ID_COLORSETTREE_CTRL_ID, SP_WDG_DeclarationTreectrl::OnToolTip)
EVT_TREE_ITEM_RIGHT_CLICK(SP_ID_COLORSETTREE_CTRL_ID, SP_WDG_DeclarationTreectrl::OnRMouseClick) //Added by G.Assaf

EVT_MENU(SP_MENU_RITEM1_TREE, SP_WDG_DeclarationTreectrl::OnCheckForUnusedItems) //Added by G.Assaf
EVT_MENU(SP_MENU_RITEM2_TREE, SP_WDG_DeclarationTreectrl::OnCleanUnusedItems) //Added by G.Assaf
EVT_MENU(SP_MENU_RITEM1_TREE + 500, SP_WDG_DeclarationTreectrl::OnSelecting) //Added by G.Assaf
EVT_MENU(SP_MENU_RITEM2_TREE + 500, SP_WDG_DeclarationTreectrl::OnUnSelecting) //Added by G.Assaf
EVT_MENU(SP_MENU_RITEM3_TREE, SP_WDG_DeclarationTreectrl::OnUnSelectAll) //Added by G.Assaf
EVT_MENU(SP_MENU_RITEM4_TREE, SP_WDG_DeclarationTreectrl::OnSelectAll) //Added by G.Assaf
#ifdef __WXMSW__

																	   //EVT_TREE_ITEM_MENU(SP_RIGHT_CLICK_ITEM + 54322, SP_WDG_DeclarationTreectrl::OnRMouseClick) //Added by G.Assaf
	EVT_TREE_ITEM_COLLAPSING(SP_ID_COLORSETTREE_CTRL_ID, SP_WDG_DeclarationTreectrl::OnCollapsingExpanding)
	EVT_TREE_ITEM_EXPANDING(SP_ID_COLORSETTREE_CTRL_ID, SP_WDG_DeclarationTreectrl::OnCollapsingExpanding)



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
	l_cMenuOptions.Append(SP_MENU_RITEM1_TREE + 500, wxT("select item"));
	l_cMenuOptions.Append(SP_MENU_RITEM2_TREE + 500, wxT("unselect item"));
	l_pcItem1 = l_cMenuOptions.GetMenuItems();


	l_cMenu.Append(SP_MENU_RITEM1_TREE, wxT("check"));
	l_cMenu.Append(SP_MENU_RITEM3_TREE, wxT("unselect all"));
	l_cMenu.Append(SP_MENU_RITEM4_TREE, wxT("select all"));
	l_cMenu.Append(SP_MENU_RITEM2_TREE, wxT("delete"));
	l_pcItem2 = l_cMenu.GetMenuItems();
	l_pcItem2[0]->Enable(true);
	l_pcItem2[1]->Enable(false);
	l_pcItem2[2]->Enable(false);
	l_pcItem2[3]->Enable(false);

	m_bisIsFirst = false;
	m_bisSecond = false;
	/************/

	m_sSelected = wxT("");
	m_setSelectedItems = {};
	m_setSelectedConstants = {};
	m_setSelectedfunctions = {};
	m_setSelectedObservers = {};
	m_setUnSelectedItems = {};
	m_setUnSelectedContants = {};
	m_setUnSelectedFunctions = {};
	m_setUnSelectedObservers = {};
	m_vUnUsedDecList = {};
	m_vUnUsedConstantsList = {};
	m_setUnUsedFunctionsList = {};
	m_setUnUsedVariablesList = {};
	m_setUnUsedColorSetsList = {};
	m_setUnUsedObserverList = {};
	m_setUnUsedSimpleCS = {};
	m_setUnUsedCompoundCS = {};
	m_setUnUsedAliasCS = {};
	m_mDeclarations2Dependencies = {};
	m_vConstantsInfo = {};
	m_vSimpleCsInfo = {};
	m_vCompoundCsInfo = {};
	m_vFunctionsInfo = {};
	m_vSubCsInfo = {};
	m_vVariablesInfo = {};
	m_vObserversInfo = {};
	m_mGroup2SelectedSet = {};
	m_vtrimeddecConstants = {};
	m_vtrimeddecFunctions = {};
	m_vtrimeddecVariables = {};
	m_vtrimeddecObservers = {};
	m_vtrimeddecSimplCS = {};
	m_vtrimeddecCompoundCS = {};
	m_vtrimeddecAliasCS = {};
	m_vtrimeddecAll = {};
	m_vtrimeddecALLCs = {};

#ifdef __WXMSW__
	m_ActivatedItem = wxTreeItemId();
#endif

}

SP_WDG_DeclarationTreectrl::~SP_WDG_DeclarationTreectrl()
{
}
/*
void SP_WDG_DeclarationTreectrl::OnFirsMentItem(wxMouseEvent& p_cEvent)
{

wxMenu* ContextMenu = new wxMenu();
ContextMenu->Append(new wxMenuItem());
wxPoint Mouse = this->ScreenToClient(wxGetMousePosition());
wxTreeItemId Hit = this->HitTest(Mouse);
int temp_num1 = wxTREE_HITTEST_ONITEMLABEL;
wxTreeItemId id = this->HitTest(wxGetMousePosition(), temp_num1);
if (Hit.IsOk()) {
if (this->ItemHasChildren(id)) {

wxString l_selectedItem = this->GetItemText(id);
PopupMenu(ContextMenu, Mouse);
}
else
{
wxRect Rect;
this->GetBoundingRect(id, Rect, true);
wxPoint Point(Rect.GetLeft() + Rect.GetHeight() / 2, Rect.GetTop() + Rect.GetHeight() / 2);
PopupMenu(ContextMenu, Point);
}
}

delete ContextMenu;


wxPoint mouse_position = p_cEvent.GetPosition();
int temp_num = wxTREE_HITTEST_ONITEMLABEL;
//p_cEvent.Skip();
wxTreeItemId id1 = this->HitTest(mouse_position, temp_num);
if ( this->ItemHasChildren(id1))
{
this->Toggle(id1);
if (!this->IsSelected(id1))
{
this->SelectItem(id1, true);
wxMenu l_cMenu;
l_cMenu.Append(SP_MENU_RITEM1_TREE, wxT("select"));
l_cMenu.Append(SP_MENU_RITEM2_TREE, wxT("unselect"));


PopupMenu(&l_cMenu);// p_cEvent.GetPosition());
}
}
else
{
p_cEvent.Skip();
}

}
*/

void SP_WDG_DeclarationTreectrl::UpdateSetODeclarations(std::map<wxString, wxString> p_mGroup)
{

	for (auto it_map = p_mGroup.begin(); it_map != p_mGroup.end(); ++it_map)
	{
		wxString l_slabel = it_map->first;
		wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), l_slabel, true, true);
		if (foundId != NULL) {
			wxColour l_nColor1(wxT("Black"));
			this->SetItemTextColour(foundId, l_nColor1);

		}
	}


}

map<wxString, wxString> SP_WDG_DeclarationTreectrl::ObtainItemDependenciesForSelect(wxString p_sItem, bool &p_bIsFound)
{

	set<wxString> l_setDependencies;
	set<wxString> l_setDependenciesTreeNaming;
	bool l_bIsFound = false;
	map<wxString, wxString> l_mDep2Type;


	m_vtrimeddecAll.insert(m_vtrimeddecConstants.begin(), m_vtrimeddecConstants.end());
	m_vtrimeddecAll.insert(m_vtrimeddecObservers.begin(), m_vtrimeddecObservers.end());
	m_vtrimeddecAll.insert(m_vtrimeddecFunctions.begin(), m_vtrimeddecFunctions.end());

	m_vtrimeddecAll.insert(m_vtrimeddecVariables.begin(), m_vtrimeddecVariables.end());

	m_vtrimeddecAll.insert(m_vtrimeddecSimplCS.begin(), m_vtrimeddecSimplCS.end());
	m_vtrimeddecAll.insert(m_vtrimeddecCompoundCS.begin(), m_vtrimeddecCompoundCS.end());

	wxString l_sSelected;
	auto found = m_vtrimeddecAll.find(p_sItem);
	if (found != m_vtrimeddecAll.end())
	{
		l_sSelected = found->second;
	}
	else {
		l_sSelected = p_sItem;
	}
	for (int it_vobs = 0; it_vobs != m_vObserversInfo.size(); it_vobs++)
	{
		if (m_vObserversInfo[it_vobs].m_observerName == l_sSelected)
		{
			l_bIsFound = true;
			l_setDependencies.insert(l_sSelected);

			//l_setDependencies.insert(m_vObserversInfo[it_vobs].m_setConstants.begin(), m_vObserversInfo[it_vobs].m_setConstants.end());
			//l_setDependencies.insert(m_vObserversInfo[it_vobs].m_setFuns.begin(), m_vObserversInfo[it_vobs].m_setFuns.end());
			//l_setDependencies.insert(m_vObserversInfo[it_vobs]..begin(), m_vObserversInfo[it_vobs].m_SetSimpleCS.end());
			//l_setDependencies.insert(m_vObserversInfo[it_vobs].m_setObservers.begin(), m_vObserversInfo[it_vobs].m_setObservers.end());
			l_mDep2Type[p_sItem] = wxT("Observer");
			break;
		}
	}
	if (l_bIsFound && l_mDep2Type[p_sItem] == wxT("Observer"))
	{


		for (int it_vConst = 0; it_vConst != m_vObserversInfo.size(); it_vConst++)
		{
			//	if (m_vObserversInfo[it_vConst].m_observerName == l_sSelected)
			//	{
			//for (auto it_con = m_vObserversInfo[it_vConst].m_setConstants.begin(); it_con != m_vObserversInfo[it_vConst].m_setConstants.end(); ++it_con)
			//{
			//l_mDep2Type[*it_con] = wxT("Constant");
			//}
			//}
		}
		//functions
		for (int it_vfun = 0; it_vfun != m_vObserversInfo.size(); it_vfun++)
		{
			if (m_vObserversInfo[it_vfun].m_observerName == l_sSelected)
			{
				for (auto it_fun = m_vObserversInfo[it_vfun].m_setFuns.begin(); it_fun != m_vObserversInfo[it_vfun].m_setFuns.end(); ++it_fun)
				{
					l_mDep2Type[*it_fun] = wxT("Function");
				}
			}
		}
	}

	///////////////////////
	for (int it_vConst = 0; it_vConst != m_vConstantsInfo.size(); it_vConst++)
	{
		if (m_vConstantsInfo[it_vConst].m_sConstName == l_sSelected)
		{
			l_bIsFound = true;
			l_setDependencies.insert(l_sSelected);

			l_setDependencies.insert(m_vConstantsInfo[it_vConst].m_SetCompoundCS.begin(), m_vConstantsInfo[it_vConst].m_SetCompoundCS.end());
			set<wxString> l_sCompCS = m_vConstantsInfo[it_vConst].m_SetCompoundCS;
			for (auto it_setComp = l_sCompCS.begin(); it_setComp != l_sCompCS.end(); ++it_setComp)
			{
				for (auto it_v = m_vCompoundCsInfo.begin(); it_v != m_vCompoundCsInfo.end(); ++it_v)
				{
					if (*it_setComp == it_v->m_sName)
					{
						l_setDependencies.insert(it_v->m_setSimpleCs.begin(), it_v->m_setSimpleCs.end());
						for (auto it1 = it_v->m_setSimpleCs.begin(); it1 != it_v->m_setSimpleCs.end(); ++it1)
						{
							l_mDep2Type[*it1] = wxT("Simple");
						}

						for (auto it2 = it_v->m_setVar.begin(); it2 != it_v->m_setVar.end(); ++it2)
						{
							l_mDep2Type[*it2] = wxT("Variable");
						}
						for (auto it3 = it_v->m_setFun.begin(); it3 != it_v->m_setFun.end(); ++it3)
						{
							l_mDep2Type[*it3] = wxT("Function");
						}

					}
				}
			}


			l_setDependencies.insert(m_vConstantsInfo[it_vConst].m_setFun.begin(), m_vConstantsInfo[it_vConst].m_setFun.end());
			//l_setDependencies.insert(m_vConstantsInfo[it_vConst].m_setForwardFun.begin(), m_vConstantsInfo[it_vConst].m_setForwardFun.end());
			l_setDependencies.insert(m_vConstantsInfo[it_vConst].m_SetSimpleCS.begin(), m_vConstantsInfo[it_vConst].m_SetSimpleCS.end());
			l_setDependencies.insert(m_vConstantsInfo[it_vConst].m_setObservers.begin(), m_vConstantsInfo[it_vConst].m_setObservers.end());
			l_mDep2Type[p_sItem] = wxT("Constant");

			for (auto it_OppChain = m_vConstantsInfo[it_vConst].m_setOppositeDepConstants.begin(); it_OppChain != m_vConstantsInfo[it_vConst].m_setOppositeDepConstants.end(); ++it_OppChain)
			{
				for (int j = 0; j < m_vConstantsInfo.size(); j++)
				{
					if (*it_OppChain == m_vConstantsInfo[j].m_sConstName&&  m_vConstantsInfo[j].m_bIsUsed == false)
					{
						l_setDependencies.insert(*it_OppChain);
						l_mDep2Type[*it_OppChain] = wxT("Constant");
					}
				}
			}
			break;

		}
	}
	if (l_bIsFound && l_mDep2Type[p_sItem] == wxT("Constant"))
	{


		for (int it_vConst = 0; it_vConst != m_vConstantsInfo.size(); it_vConst++)
		{
			if (m_vConstantsInfo[it_vConst].m_sConstName == l_sSelected)
			{
				for (auto it_compCS = m_vConstantsInfo[it_vConst].m_SetCompoundCS.begin(); it_compCS != m_vConstantsInfo[it_vConst].m_SetCompoundCS.end(); ++it_compCS)
				{
					l_mDep2Type[*it_compCS] = wxT("Compound");
				}
			}
		}
		//functions
		for (int it_vConst = 0; it_vConst != m_vConstantsInfo.size(); it_vConst++)
		{
			if (m_vConstantsInfo[it_vConst].m_sConstName == l_sSelected)
			{
				for (auto it_fun = m_vConstantsInfo[it_vConst].m_setFun.begin(); it_fun != m_vConstantsInfo[it_vConst].m_setFun.end(); ++it_fun)
				{
					l_mDep2Type[*it_fun] = wxT("Function");
				}
				//	for (auto it_fun = m_vConstantsInfo[it_vConst].m_setForwardFun.begin(); it_fun != m_vConstantsInfo[it_vConst].m_setForwardFun.end(); ++it_fun)
				//	{
				//		l_mDep2Type[*it_fun] = wxT("Function");
				//	}
			}
		}
		//observers
		for (int it_vConst = 0; it_vConst != m_vConstantsInfo.size(); it_vConst++)
		{
			if (m_vConstantsInfo[it_vConst].m_sConstName == l_sSelected)
			{
				for (auto it_ob = m_vConstantsInfo[it_vConst].m_setObservers.begin(); it_ob != m_vConstantsInfo[it_vConst].m_setObservers.end(); ++it_ob)
				{
					l_mDep2Type[*it_ob] = wxT("Observer");
				}
			}
		}
		//simple cs
		for (int it_vConst = 0; it_vConst != m_vConstantsInfo.size(); it_vConst++)
		{
			if (m_vConstantsInfo[it_vConst].m_sConstName == l_sSelected)
			{
				for (auto it_simple = m_vConstantsInfo[it_vConst].m_SetSimpleCS.begin(); it_simple != m_vConstantsInfo[it_vConst].m_SetSimpleCS.end(); ++it_simple)
				{
					l_mDep2Type[*it_simple] = wxT("Simple");
				}
			}
		}

		for (int it_vConst = 0; it_vConst != m_vConstantsInfo.size(); it_vConst++)
		{
			if (m_vConstantsInfo[it_vConst].m_sConstName == l_sSelected)
			{
				for (auto it_simple = m_vConstantsInfo[it_vConst].m_setVar.begin(); it_simple != m_vConstantsInfo[it_vConst].m_setVar.end(); ++it_simple)
				{
					l_mDep2Type[*it_simple] = wxT("Variable");
				}
			}
		}

	}
	if (!l_bIsFound) {
		for (int it_vsimple = 0; it_vsimple != m_vSimpleCsInfo.size(); it_vsimple++)
		{
			if (m_vSimpleCsInfo[it_vsimple].m_sName == l_sSelected)
			{
				l_bIsFound = true;
				l_setDependencies.insert(l_sSelected);
				l_setDependencies.insert(m_vSimpleCsInfo[it_vsimple].m_setIndirectCs.begin(), m_vSimpleCsInfo[it_vsimple].m_setIndirectCs.end());
				//l_setDependencies.insert(m_vSimpleCsInfo[it_vsimple].m_setConst.begin(), m_vSimpleCsInfo[it_vsimple].m_setConst.end());
				l_setDependencies.insert(m_vSimpleCsInfo[it_vsimple].m_setCompCs.begin(), m_vSimpleCsInfo[it_vsimple].m_setCompCs.end());
				l_setDependencies.insert(m_vSimpleCsInfo[it_vsimple].m_setFun.begin(), m_vSimpleCsInfo[it_vsimple].m_setFun.end());
				//l_setDependencies.insert(m_vSimpleCsInfo[it_vsimple].m_setConst.begin(), m_vSimpleCsInfo[it_vsimple].m_setConst.end());
				l_setDependencies.insert(m_vSimpleCsInfo[it_vsimple].m_setVar.begin(), m_vSimpleCsInfo[it_vsimple].m_setVar.end());
				l_mDep2Type[l_sSelected] = wxT("Simple");
				for (auto it = m_vSimpleCsInfo[it_vsimple].m_setIndirectCs.begin(); it != m_vSimpleCsInfo[it_vsimple].m_setIndirectCs.end(); it++)
				{
					l_mDep2Type[*it] = wxT("Simple");
				}
				break;
			}
		}
	}
	if (l_bIsFound && l_mDep2Type[l_sSelected] == wxT("Simple"))
	{

		for (int it_vsimple = 0; it_vsimple != m_vSimpleCsInfo.size(); it_vsimple++)
		{
			if (m_vSimpleCsInfo[it_vsimple].m_sName == l_sSelected)
			{
				for (auto it_dec = m_vSimpleCsInfo[it_vsimple].m_setConst.begin(); it_dec != m_vSimpleCsInfo[it_vsimple].m_setConst.end(); ++it_dec)
				{
					//l_mDep2Type[*it_dec] = wxT("Constant");
				}
			}
		}
		/*****compound cs***/
		for (int it_vsimple = 0; it_vsimple != m_vSimpleCsInfo.size(); it_vsimple++)
		{
			if ((m_vSimpleCsInfo[it_vsimple].m_sName == l_sSelected))
			{
				set<wxString> l_setComp = m_vSimpleCsInfo[it_vsimple].m_setCompCs;
				//l_setComp.insert(m_vSimpleCsInfo[it_vsimple].m_setIndirectCs.begin(), m_vSimpleCsInfo[it_vsimple].m_setIndirectCs.end());
				for (auto it = l_setComp.begin(); it != l_setComp.end(); it++)
				{
					l_mDep2Type[*it] = wxT("Compound");
				}
			}
			/************var********/
			if ((m_vSimpleCsInfo[it_vsimple].m_sName == l_sSelected))
			{
				set<wxString> l_setVar = m_vSimpleCsInfo[it_vsimple].m_setVar;
				//l_setComp.insert(m_vSimpleCsInfo[it_vsimple].m_setIndirectCs.begin(), m_vSimpleCsInfo[it_vsimple].m_setIndirectCs.end());
				for (auto it = l_setVar.begin(); it != l_setVar.end(); it++)
				{
					l_mDep2Type[*it] = wxT("Variable");
				}
			}
			/**********functions*********/
			if ((m_vSimpleCsInfo[it_vsimple].m_sName == l_sSelected))
			{
				set<wxString> l_setfun = m_vSimpleCsInfo[it_vsimple].m_setFun;
				//l_setComp.insert(m_vSimpleCsInfo[it_vsimple].m_setIndirectCs.begin(), m_vSimpleCsInfo[it_vsimple].m_setIndirectCs.end());
				for (auto it = l_setfun.begin(); it != l_setfun.end(); it++)
				{
					l_mDep2Type[*it] = wxT("Function");
				}
			}
			/**********functions*********/
		}


	}

	if (!l_bIsFound) {
		for (int j = 0; j != m_vFunctionsInfo.size(); j++)
		{
			if (m_vFunctionsInfo[j].m_sFunName == l_sSelected)
			{
				l_bIsFound = true;
				l_setDependencies.insert(l_sSelected);
				l_setDependencies.insert(m_vFunctionsInfo[j].m_setCompoundCs.begin(), m_vFunctionsInfo[j].m_setCompoundCs.end());
				l_setDependencies.insert(m_vFunctionsInfo[j].m_setSimpleCs.begin(), m_vFunctionsInfo[j].m_setSimpleCs.end());
				l_setDependencies.insert(m_vFunctionsInfo[j].m_setConst.begin(), m_vFunctionsInfo[j].m_setConst.end());
				l_setDependencies.insert(m_vFunctionsInfo[j].m_setBackwordConst.begin(), m_vFunctionsInfo[j].m_setBackwordConst.end());
				l_setDependencies.insert(m_vFunctionsInfo[j].m_setBackwordObservers.begin(), m_vFunctionsInfo[j].m_setBackwordObservers.end());
				l_mDep2Type[l_sSelected] = wxT("Function");
				//l_setDependencies.insert(m_vFunctionsInfo[j].m_setVars.begin(), m_vFunctionsInfo[j].m_setVars.end());
				break;
			}
		}
	}
	if (l_bIsFound && l_mDep2Type[l_sSelected] == wxT("Function"))
	{

		for (int j = 0; j != m_vFunctionsInfo.size(); j++)
		{
			if (m_vFunctionsInfo[j].m_sFunName == l_sSelected)
			{

				//l_setDependencies.insert(, );
				for (auto it_dec = m_vFunctionsInfo[j].m_setCompoundCs.begin(); it_dec != m_vFunctionsInfo[j].m_setCompoundCs.end(); ++it_dec)
				{
					l_mDep2Type[*it_dec] = wxT("Compound");
				}
				for (auto it_dec = m_vFunctionsInfo[j].m_setSimpleCs.begin(); it_dec != m_vFunctionsInfo[j].m_setSimpleCs.end(); ++it_dec)
				{
					l_mDep2Type[*it_dec] = wxT("Simple");
				}
				for (auto it_dec = m_vFunctionsInfo[j].m_setConst.begin(); it_dec != m_vFunctionsInfo[j].m_setConst.end(); ++it_dec)
				{
					l_mDep2Type[*it_dec] = wxT("Constant");
				}
				for (auto it_dec = m_vFunctionsInfo[j].m_setBackwordConst.begin(); it_dec != m_vFunctionsInfo[j].m_setBackwordConst.end(); ++it_dec)
				{
					l_mDep2Type[*it_dec] = wxT("Constant");
				}
				for (auto it_dec = m_vFunctionsInfo[j].m_setObservers.begin(); it_dec != m_vFunctionsInfo[j].m_setObservers.end(); ++it_dec)
				{
					l_mDep2Type[*it_dec] = wxT("Observer");
				}
				for (auto it_dec = m_vFunctionsInfo[j].m_setBackwordObservers.begin(); it_dec != m_vFunctionsInfo[j].m_setBackwordObservers.end(); ++it_dec)
				{
					l_mDep2Type[*it_dec] = wxT("Observer");
				}
			}
		}

	}

	if (!l_bIsFound) {
		for (int j = 0; j != m_vVariablesInfo.size(); j++)
		{
			if (m_vVariablesInfo[j].m_sName == l_sSelected)
			{
				l_bIsFound = true;
				l_setDependencies.insert(l_sSelected);
				wxString l_sVarType = m_unUsedDec.ObtainVarType(l_sSelected);
				l_mDep2Type[l_sSelected] = wxT("Variable");
				//to add subcs
				break;

			}
		}
	}
	if (l_bIsFound  && l_mDep2Type[l_sSelected] == wxT("Variable"))
	{

		for (int j = 0; j != m_vVariablesInfo.size(); j++)
		{
			if (m_vVariablesInfo[j].m_sName == l_sSelected)
			{

				//l_setDependencies.insert(l_sSelected);
				wxString l_sVarType = m_unUsedDec.ObtainVarType(l_sSelected);
			}
		}
	}

	if (!l_bIsFound) {
		for (int j = 0; j != m_vCompoundCsInfo.size(); j++)
		{
			if (m_vCompoundCsInfo[j].m_sName == l_sSelected)
			{
				l_bIsFound = true;
				l_setDependencies.insert(l_sSelected);
				l_setDependencies.insert(m_vCompoundCsInfo[j].m_setFun.begin(), m_vCompoundCsInfo[j].m_setFun.end());
				//	l_setDependencies.insert(m_vCompoundCsInfo[j].m_setSimpleCs.begin(), m_vCompoundCsInfo[j].m_setSimpleCs.end());
				//	set<wxString> l_setComp = m_vCompoundCsInfo[j].m_setIndirectCompCs;
				//l_setDependencies.insert(m_vCompoundCsInfo[j].m_setIndirectCompCs.begin(), m_vCompoundCsInfo[j].m_setIndirectCompCs.end());
				//l_setDependencies.insert(m_vCompoundCsInfo[j].m_setConst.begin(), m_vCompoundCsInfo[j].m_setConst.end());
				l_mDep2Type[l_sSelected] = wxT("Compound");
				l_setDependencies.insert(m_vCompoundCsInfo[j].m_setVar.begin(), m_vCompoundCsInfo[j].m_setVar.end());
				break;
			}
		}
	}

	if (l_bIsFound&&l_mDep2Type[l_sSelected] == wxT("Compound"))
	{

		for (int j = 0; j != m_vCompoundCsInfo.size(); j++)
		{
			if (m_vCompoundCsInfo[j].m_sName == l_sSelected)
			{


				//	for (auto it_dec = m_vCompoundCsInfo[j].m_setIndirectCompCs.begin(); it_dec != m_vCompoundCsInfo[j].m_setIndirectCompCs.end(); ++it_dec)
				//	{
				//		l_mDep2Type[*it_dec] = wxT("Compound");
				//	 }
				//	for (auto it_dec = m_vCompoundCsInfo[j].m_setConst.begin(); it_dec != m_vCompoundCsInfo[j].m_setConst.end(); ++it_dec)
				//	{
				//		l_mDep2Type[*it_dec] = wxT("Constant");
				//}
				for (auto it_dec = m_vCompoundCsInfo[j].m_setFun.begin(); it_dec != m_vCompoundCsInfo[j].m_setFun.end(); ++it_dec)
				{
					l_mDep2Type[*it_dec] = wxT("Function");
				}
				for (auto it_dec = m_vCompoundCsInfo[j].m_setVar.begin(); it_dec != m_vCompoundCsInfo[j].m_setVar.end(); ++it_dec)
				{
					l_mDep2Type[*it_dec] = wxT("Variable");
				}
			}
		}


	}

	if (!l_bIsFound) {
		for (int j = 0; j != m_vSubCsInfo.size(); j++)
		{
			if (m_vSubCsInfo[j].m_sName == l_sSelected)
			{
				l_bIsFound = true;
				l_setDependencies.insert(l_sSelected);
				//l_setDependencies.insert(m_vSubCsInfo[j].m_setFun.begin(), m_vSubCsInfo[j].m_setFun.end());
				l_setDependencies.insert(m_vSubCsInfo[j].m_setSimpleCs.begin(), m_vSubCsInfo[j].m_setSimpleCs.end());
				l_setDependencies.insert(m_vSubCsInfo[j].m_setConst.begin(), m_vSubCsInfo[j].m_setConst.end());
				l_setDependencies.insert(m_vSubCsInfo[j].m_CompoundCS.begin(), m_vSubCsInfo[j].m_CompoundCS.end());
				l_mDep2Type[l_sSelected] = wxT("Compound");
				break;
			}
		}
	}
	if (l_bIsFound && l_mDep2Type[l_sSelected] == wxT("Compound"))
	{
		//l_mDep2Type[m_sSelected] = wxT("Compound");
		for (int j = 0; j != m_vSubCsInfo.size(); j++)
		{
			if (m_vSubCsInfo[j].m_sName == l_sSelected)
			{


				l_setDependencies.insert(m_vSubCsInfo[j].m_setSimpleCs.begin(), m_vSubCsInfo[j].m_setSimpleCs.end());
				for (auto it_dec = m_vSubCsInfo[j].m_setSimpleCs.begin(); it_dec != m_vSubCsInfo[j].m_setSimpleCs.end(); it_dec++)
				{
					l_mDep2Type[*it_dec] = wxT("Simple");
				}
				for (auto it_dec = m_vSubCsInfo[j].m_setConst.begin(); it_dec != m_vSubCsInfo[j].m_setConst.end(); it_dec++)
				{
					l_mDep2Type[*it_dec] = wxT("Constant");
				}
				for (auto it_dec = m_vSubCsInfo[j].m_CompoundCS.begin(); it_dec != m_vSubCsInfo[j].m_CompoundCS.begin(); it_dec++)
				{
					l_mDep2Type[*it_dec] = wxT("Compound");
				}


			}
		}

	}




	/*************return tree naming of unselected item(s)**********/

	map<wxString, wxString> l_mRes;

	for (auto it_mapRes = l_mDep2Type.begin(); it_mapRes != l_mDep2Type.end(); ++it_mapRes)
	{
		for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
		{
			if (it_mapRes->first == itmap->second)
			{
				l_mRes[itmap->first] = wxT("Constant");

			}
			else {
				if (it_mapRes->second != wxT("Constant") && it_mapRes->second != wxT(""))
					l_mRes[it_mapRes->first] = it_mapRes->second;
			}

		}
	}

	for (auto itset = l_setDependencies.begin(); itset != l_setDependencies.end(); ++itset)
	{
		for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
		{
			if (*itset == itmap->second)
			{
				l_setDependenciesTreeNaming.insert(itmap->first);
			}
		}
	}

	for (auto itset = l_setDependencies.begin(); itset != l_setDependencies.end(); ++itset)
	{
		for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
		{
			if (*itset == itmap->second)
			{
				l_setDependenciesTreeNaming.insert(itmap->first);
			}
		}
	}


	for (auto itset = l_setDependencies.begin(); itset != l_setDependencies.end(); ++itset)
	{
		for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
		{
			if (*itset == itmap->second)
			{
				l_setDependenciesTreeNaming.insert(itmap->first);
			}
		}
	}

	for (auto itset = l_setDependencies.begin(); itset != l_setDependencies.end(); ++itset)
	{
		for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
		{
			if (*itset == itmap->second)
			{
				l_setDependenciesTreeNaming.insert(itmap->first);
			}
		}
	}

	p_bIsFound = l_bIsFound;
	return l_mRes;
	//return l_setDependenciesTreeNaming;


}

set<wxString> SP_WDG_DeclarationTreectrl::ObtainItemDependenciesForUnSelect(wxString p_sItem)
{

	set<wxString> l_setDependencies;
	set<wxString> l_setDependenciesTreeNaming;
	bool l_bIsFound = false;



	m_vtrimeddecAll.insert(m_vtrimeddecConstants.begin(), m_vtrimeddecConstants.end());

	m_vtrimeddecAll.insert(m_vtrimeddecFunctions.begin(), m_vtrimeddecFunctions.end());

	m_vtrimeddecAll.insert(m_vtrimeddecVariables.begin(), m_vtrimeddecVariables.end());
	m_vtrimeddecAll.insert(m_vtrimeddecObservers.begin(), m_vtrimeddecObservers.end());
	m_vtrimeddecAll.insert(m_vtrimeddecSimplCS.begin(), m_vtrimeddecSimplCS.end());
	m_vtrimeddecAll.insert(m_vtrimeddecCompoundCS.begin(), m_vtrimeddecCompoundCS.end());

	wxString l_sSelected;
	auto found = m_vtrimeddecAll.find(p_sItem);
	if (found != m_vtrimeddecAll.end())
	{
		l_sSelected = found->second;
	}
	else {
		l_sSelected = p_sItem;
	}
	for (int it_vConst = 0; it_vConst != m_vConstantsInfo.size(); it_vConst++)
	{
		if (m_vConstantsInfo[it_vConst].m_sConstName == l_sSelected)
		{
			l_bIsFound = true;
			l_setDependencies.insert(l_sSelected);
			set<wxString> l_setConDep = m_vConstantsInfo[it_vConst].m_setConstants;
			for (auto it_con = l_setConDep.begin(); it_con != l_setConDep.end(); ++it_con)
			{
				for (auto it_unusedConsts = m_vConstantsInfo.begin(); it_unusedConsts != m_vConstantsInfo.end(); it_unusedConsts++)
				{
					if (*it_con == it_unusedConsts->m_sConstName && it_unusedConsts->m_bIsUsed == false)
					{
						l_setDependencies.insert(*it_con);
						l_setDependencies.insert(it_unusedConsts->m_setConstants.begin(), it_unusedConsts->m_setConstants.end());
						l_setDependencies.insert(it_unusedConsts->m_setForwardFun.begin(), it_unusedConsts->m_setForwardFun.end());
					}
				}

			}


			l_setDependencies.insert(m_vConstantsInfo[it_vConst].m_setForwardFun.begin(), m_vConstantsInfo[it_vConst].m_setForwardFun.end());
			//	l_setDependencies.insert(m_vConstantsInfo[it_vConst].m_setFun.begin(), m_vConstantsInfo[it_vConst].m_setFun.end());
			//	l_setDependencies.insert(m_vConstantsInfo[it_vConst].m_SetSimpleCS.begin(), m_vConstantsInfo[it_vConst].m_SetSimpleCS.end());
			//	l_setDependencies.insert(m_vConstantsInfo[it_vConst].m_setVar.begin(), m_vConstantsInfo[it_vConst].m_setVar.end());
			break;
		}
	}
	if (!l_bIsFound) {
		for (int it_vsimple = 0; it_vsimple != m_vSimpleCsInfo.size(); it_vsimple++)
		{
			if (m_vSimpleCsInfo[it_vsimple].m_sName == l_sSelected)
			{
				l_bIsFound = true;
				l_setDependencies.insert(l_sSelected);
				l_setDependencies.insert(m_vSimpleCsInfo[it_vsimple].m_setConst.begin(), m_vSimpleCsInfo[it_vsimple].m_setConst.end());
				//l_setDependencies.insert(m_vSimpleCsInfo[it_vsimple].m_setCompCs.begin(), m_vSimpleCsInfo[it_vsimple].m_setCompCs.end());
				//l_setDependencies.insert(m_vSimpleCsInfo[it_vsimple].m_setFun.begin(), m_vSimpleCsInfo[it_vsimple].m_setFun.end());
				l_setDependencies.insert(m_vSimpleCsInfo[it_vsimple].m_setConst.begin(), m_vSimpleCsInfo[it_vsimple].m_setConst.end());
				//l_setDependencies.insert(m_vSimpleCsInfo[it_vsimple].m_setVar.begin(), m_vSimpleCsInfo[it_vsimple].m_setVar.end());
				break;
			}
		}
	}


	if (!l_bIsFound) {
		for (int j = 0; j != m_vFunctionsInfo.size(); j++)
		{
			if (m_vFunctionsInfo[j].m_sFunName == l_sSelected)
			{
				l_bIsFound = true;
				l_setDependencies.insert(l_sSelected);
				l_setDependencies.insert(m_vFunctionsInfo[j].m_setCompoundCs.begin(), m_vFunctionsInfo[j].m_setCompoundCs.end());
				l_setDependencies.insert(m_vFunctionsInfo[j].m_setSimpleCs.begin(), m_vFunctionsInfo[j].m_setSimpleCs.end());
				l_setDependencies.insert(m_vFunctionsInfo[j].m_setConst.begin(), m_vFunctionsInfo[j].m_setConst.end());
				//l_setDependencies.insert(m_vFunctionsInfo[j].m_setConst.begin(), m_vFunctionsInfo[j].m_setConst.end());
				///l_setDependencies.insert(m_vFunctionsInfo[j].m_setObservers.begin(), m_vFunctionsInfo[j].m_setObservers.end());
				break;
			}
		}
	}

	if (!l_bIsFound) {
		for (int j = 0; j != m_vObserversInfo.size(); j++)
		{
			if (m_vObserversInfo[j].m_observerName == l_sSelected)
			{
				l_bIsFound = true;
				l_setDependencies.insert(l_sSelected);
				//l_setDependencies.insert(m_vFunctionsInfo[j].m_setCompoundCs.begin(), m_vFunctionsInfo[j].m_setCompoundCs.end());
				//	l_setDependencies.insert(m_vFunctionsInfo[j].m_setSimpleCs.begin(), m_vFunctionsInfo[j].m_setSimpleCs.end());
				l_setDependencies.insert(m_vObserversInfo[j].m_setConstants.begin(), m_vObserversInfo[j].m_setConstants.end());
				l_setDependencies.insert(m_vObserversInfo[j].m_setForwardFuns.begin(), m_vObserversInfo[j].m_setForwardFuns.end());
				//	l_setDependencies.insert(m_vFunctionsInfo[j].m_setConst.begin(), m_vFunctionsInfo[j].m_setConst.end());
				///l_setDependencies.insert(m_vFunctionsInfo[j].m_setObservers.begin(), m_vFunctionsInfo[j].m_setObservers.end());
				break;
			}
		}
	}
	if (!l_bIsFound) {
		for (int j = 0; j != m_vVariablesInfo.size(); j++)
		{
			if (m_vVariablesInfo[j].m_sName == l_sSelected)
			{
				l_bIsFound = true;
				l_setDependencies.insert(l_sSelected);
				wxString l_sVarType = m_unUsedDec.ObtainVarType(l_sSelected);
				//if (m_vVariablesInfo[j].m_setSimpleCs.size() > 0)
				//{
				auto findType = m_vVariablesInfo[j].m_setSimpleCs.find(l_sVarType);
				if (findType != m_vVariablesInfo[j].m_setSimpleCs.end())
				{
					l_setDependencies.insert(m_vVariablesInfo[j].m_setSimpleCs.begin(), m_vVariablesInfo[j].m_setSimpleCs.end());
				}
				auto findType1 = m_vVariablesInfo[j].m_CompoundCS.find(l_sVarType);
				if (findType1 != m_vVariablesInfo[j].m_CompoundCS.end())
				{
					l_setDependencies.insert(m_vVariablesInfo[j].m_CompoundCS.begin(), m_vVariablesInfo[j].m_CompoundCS.end());

					l_setDependencies.insert(m_vVariablesInfo[j].m_setSimpleCs.begin(), m_vVariablesInfo[j].m_setSimpleCs.end());
				}


				l_setDependencies.insert(m_vVariablesInfo[j].m_setConst.begin(), m_vVariablesInfo[j].m_setConst.end());

				//l_setDependencies.insert(m_vVariablesInfo[j].m_CompoundCS.begin(), m_vVariablesInfo[j].m_CompoundCS.end());
				break;
				//}
			}
		}
	}

	if (!l_bIsFound) {
		for (int j = 0; j != m_vCompoundCsInfo.size(); j++)
		{
			if (m_vCompoundCsInfo[j].m_sName == l_sSelected)
			{
				l_bIsFound = true;
				l_setDependencies.insert(l_sSelected);
				//l_setDependencies.insert(m_vCompoundCsInfo[j].m_setFun.begin(), m_vCompoundCsInfo[j].m_setFun.end());
				l_setDependencies.insert(m_vCompoundCsInfo[j].m_setSimpleCs.begin(), m_vCompoundCsInfo[j].m_setSimpleCs.end());
				l_setDependencies.insert(m_vCompoundCsInfo[j].m_setConst.begin(), m_vCompoundCsInfo[j].m_setConst.end());
				//l_setDependencies.insert(m_vCompoundCsInfo[j].m_setVar.begin(), m_vCompoundCsInfo[j].m_setVar.end());
				break;
			}
		}
	}

	if (!l_bIsFound) {
		for (int j = 0; j != m_vSubCsInfo.size(); j++)
		{
			if (m_vSubCsInfo[j].m_sName == l_sSelected)
			{
				l_bIsFound = true;
				l_setDependencies.insert(l_sSelected);
				//l_setDependencies.insert(m_vSubCsInfo[j].m_setFun.begin(), m_vSubCsInfo[j].m_setFun.end());
				l_setDependencies.insert(m_vSubCsInfo[j].m_setSimpleCs.begin(), m_vSubCsInfo[j].m_setSimpleCs.end());
				l_setDependencies.insert(m_vSubCsInfo[j].m_setConst.begin(), m_vSubCsInfo[j].m_setConst.end());
				l_setDependencies.insert(m_vSubCsInfo[j].m_CompoundCS.begin(), m_vSubCsInfo[j].m_CompoundCS.end());
				break;
			}
		}
	}
	/*************return tree naming of unselected item(s)**********/
	for (auto itset = l_setDependencies.begin(); itset != l_setDependencies.end(); ++itset)
	{
		for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
		{
			if (*itset == itmap->second)
			{
				l_setDependenciesTreeNaming.insert(itmap->first);
			}
		}
	}

	for (auto itset = l_setDependencies.begin(); itset != l_setDependencies.end(); ++itset)
	{
		for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
		{
			if (*itset == itmap->second)
			{
				l_setDependenciesTreeNaming.insert(itmap->first);
			}
		}
	}
	for (auto itset = l_setDependencies.begin(); itset != l_setDependencies.end(); ++itset)
	{
		for (auto itmap = m_vtrimeddecObservers.begin(); itmap != m_vtrimeddecObservers.end(); ++itmap)
		{
			if (*itset == itmap->second)
			{
				l_setDependenciesTreeNaming.insert(itmap->first);
			}
		}
	}
	for (auto itset = l_setDependencies.begin(); itset != l_setDependencies.end(); ++itset)
	{
		for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
		{
			if (*itset == itmap->second)
			{
				l_setDependenciesTreeNaming.insert(itmap->first);
			}
		}
	}


	for (auto itset = l_setDependencies.begin(); itset != l_setDependencies.end(); ++itset)
	{
		for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
		{
			if (*itset == itmap->second)
			{
				l_setDependenciesTreeNaming.insert(itmap->first);
			}
		}
	}

	for (auto itset = l_setDependencies.begin(); itset != l_setDependencies.end(); ++itset)
	{
		for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
		{
			if (*itset == itmap->second)
			{
				l_setDependenciesTreeNaming.insert(itmap->first);
			}
		}
	}

	return l_setDependenciesTreeNaming;


}

set<wxString> SP_WDG_DeclarationTreectrl::ObtainItemDependencies(wxString p_sItem)
{
	set<wxString> l_setDependencies;
	set<wxString> l_setDependenciesTreeNaming;
	bool l_bIsFound = false;



	m_vtrimeddecAll.insert(m_vtrimeddecConstants.begin(), m_vtrimeddecConstants.end());

	m_vtrimeddecAll.insert(m_vtrimeddecFunctions.begin(), m_vtrimeddecFunctions.end());

	m_vtrimeddecAll.insert(m_vtrimeddecVariables.begin(), m_vtrimeddecVariables.end());

	m_vtrimeddecAll.insert(m_vtrimeddecSimplCS.begin(), m_vtrimeddecSimplCS.end());
	m_vtrimeddecAll.insert(m_vtrimeddecCompoundCS.begin(), m_vtrimeddecCompoundCS.end());

	wxString l_sSelected;
	auto found = m_vtrimeddecAll.find(p_sItem);
	if (found != m_vtrimeddecAll.end())
	{
		l_sSelected = found->second;
	}
	else {
		l_sSelected = p_sItem;
	}

	for (int it_vConst = 0; it_vConst != m_vConstantsInfo.size(); it_vConst++)
	{
		if (m_vConstantsInfo[it_vConst].m_sConstName == l_sSelected)
		{
			l_bIsFound = true;
			l_setDependencies.insert(l_sSelected);
			l_setDependencies.insert(m_vConstantsInfo[it_vConst].m_SetCompoundCS.begin(), m_vConstantsInfo[it_vConst].m_SetCompoundCS.end());
			l_setDependencies.insert(m_vConstantsInfo[it_vConst].m_setFun.begin(), m_vConstantsInfo[it_vConst].m_setFun.end());
			l_setDependencies.insert(m_vConstantsInfo[it_vConst].m_SetSimpleCS.begin(), m_vConstantsInfo[it_vConst].m_SetSimpleCS.end());
			l_setDependencies.insert(m_vConstantsInfo[it_vConst].m_setVar.begin(), m_vConstantsInfo[it_vConst].m_setVar.end());
			break;
		}
	}
	if (!l_bIsFound) {
		for (int it_vsimple = 0; it_vsimple != m_vSimpleCsInfo.size(); it_vsimple++)
		{
			if (m_vSimpleCsInfo[it_vsimple].m_sName == l_sSelected)
			{
				l_bIsFound = true;
				l_setDependencies.insert(l_sSelected);
				l_setDependencies.insert(m_vSimpleCsInfo[it_vsimple].m_setCompCs.begin(), m_vSimpleCsInfo[it_vsimple].m_setCompCs.end());
				l_setDependencies.insert(m_vSimpleCsInfo[it_vsimple].m_setFun.begin(), m_vSimpleCsInfo[it_vsimple].m_setFun.end());
				l_setDependencies.insert(m_vSimpleCsInfo[it_vsimple].m_setConst.begin(), m_vSimpleCsInfo[it_vsimple].m_setConst.end());
				l_setDependencies.insert(m_vSimpleCsInfo[it_vsimple].m_setVar.begin(), m_vSimpleCsInfo[it_vsimple].m_setVar.end());
				break;
			}
		}
	}
	if (!l_bIsFound) {
		for (int j = 0; j != m_vFunctionsInfo.size(); j++)
		{
			if (m_vFunctionsInfo[j].m_sFunName == l_sSelected)
			{
				l_bIsFound = true;
				l_setDependencies.insert(l_sSelected);
				l_setDependencies.insert(m_vFunctionsInfo[j].m_setCompoundCs.begin(), m_vFunctionsInfo[j].m_setCompoundCs.end());
				l_setDependencies.insert(m_vFunctionsInfo[j].m_setSimpleCs.begin(), m_vFunctionsInfo[j].m_setSimpleCs.end());
				l_setDependencies.insert(m_vFunctionsInfo[j].m_setConst.begin(), m_vFunctionsInfo[j].m_setConst.end());
				l_setDependencies.insert(m_vFunctionsInfo[j].m_setVars.begin(), m_vFunctionsInfo[j].m_setVars.end());
				break;
			}
		}
	}

	if (!l_bIsFound) {
		for (int j = 0; j != m_vCompoundCsInfo.size(); j++)
		{
			if (m_vCompoundCsInfo[j].m_sName == l_sSelected)
			{
				l_bIsFound = true;
				l_setDependencies.insert(l_sSelected);
				l_setDependencies.insert(m_vCompoundCsInfo[j].m_setFun.begin(), m_vCompoundCsInfo[j].m_setFun.end());
				l_setDependencies.insert(m_vCompoundCsInfo[j].m_setSimpleCs.begin(), m_vCompoundCsInfo[j].m_setSimpleCs.end());
				l_setDependencies.insert(m_vCompoundCsInfo[j].m_setConst.begin(), m_vCompoundCsInfo[j].m_setConst.end());
				l_setDependencies.insert(m_vCompoundCsInfo[j].m_setSimpleCs.begin(), m_vCompoundCsInfo[j].m_setSimpleCs.end());
				l_setDependencies.insert(m_vCompoundCsInfo[j].m_setVar.begin(), m_vCompoundCsInfo[j].m_setVar.end());
				break;
			}
		}
	}

	if (!l_bIsFound) {
		for (int j = 0; j != m_vSubCsInfo.size(); j++)
		{
			if (m_vSubCsInfo[j].m_sName == l_sSelected)
			{
				l_bIsFound = true;
				l_setDependencies.insert(l_sSelected);
				l_setDependencies.insert(m_vSubCsInfo[j].m_setFun.begin(), m_vSubCsInfo[j].m_setFun.end());
				l_setDependencies.insert(m_vSubCsInfo[j].m_setSimpleCs.begin(), m_vSubCsInfo[j].m_setSimpleCs.end());
				l_setDependencies.insert(m_vSubCsInfo[j].m_setConst.begin(), m_vSubCsInfo[j].m_setConst.end());
				l_setDependencies.insert(m_vSubCsInfo[j].m_setSimpleCs.begin(), m_vSubCsInfo[j].m_setSimpleCs.end());
				l_setDependencies.insert(m_vSubCsInfo[j].m_CompoundCS.begin(), m_vSubCsInfo[j].m_CompoundCS.end());
				break;
			}
		}
	}

	if (!l_bIsFound) {
		for (int j = 0; j != m_vVariablesInfo.size(); j++)
		{
			if (m_vVariablesInfo[j].m_sName == l_sSelected)
			{
				l_bIsFound = true;
				l_setDependencies.insert(l_sSelected);
				l_setDependencies.insert(m_vVariablesInfo[j].m_setFun.begin(), m_vVariablesInfo[j].m_setFun.end());
				l_setDependencies.insert(m_vVariablesInfo[j].m_setSimpleCs.begin(), m_vVariablesInfo[j].m_setSimpleCs.end());
				l_setDependencies.insert(m_vVariablesInfo[j].m_setConst.begin(), m_vVariablesInfo[j].m_setConst.end());
				l_setDependencies.insert(m_vVariablesInfo[j].m_setSimpleCs.begin(), m_vVariablesInfo[j].m_setSimpleCs.end());
				l_setDependencies.insert(m_vVariablesInfo[j].m_CompoundCS.begin(), m_vVariablesInfo[j].m_CompoundCS.end());
				break;
			}
		}
	}
	/**************compute tree naming of all dependencies**************/
	for (auto itset = l_setDependencies.begin(); itset != l_setDependencies.end(); ++itset)
	{
		for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
		{
			if (*itset == itmap->second)
			{
				l_setDependenciesTreeNaming.insert(itmap->first);
			}
		}
	}

	for (auto itset = l_setDependencies.begin(); itset != l_setDependencies.end(); ++itset)
	{
		for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
		{
			if (*itset == itmap->second)
			{
				l_setDependenciesTreeNaming.insert(itmap->first);
			}
		}
	}

	for (auto itset = l_setDependencies.begin(); itset != l_setDependencies.end(); ++itset)
	{
		for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
		{
			if (*itset == itmap->second)
			{
				l_setDependenciesTreeNaming.insert(itmap->first);
			}
		}
	}


	for (auto itset = l_setDependencies.begin(); itset != l_setDependencies.end(); ++itset)
	{
		for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
		{
			if (*itset == itmap->second)
			{
				l_setDependenciesTreeNaming.insert(itmap->first);
			}
		}
	}

	for (auto itset = l_setDependencies.begin(); itset != l_setDependencies.end(); ++itset)
	{
		for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
		{
			if (*itset == itmap->second)
			{
				l_setDependenciesTreeNaming.insert(itmap->first);
			}
		}
	}

	return l_setDependenciesTreeNaming;

}
set<wxString>  SP_WDG_DeclarationTreectrl::SetSelectItems(std::map<wxString, wxString> &p_mdec, set<wxString> &p_setUnused)
{
	set<wxString> l_setSelected;

	auto it_set = p_setUnused.begin();
	while (it_set != p_setUnused.end())
	{
		auto it = p_mdec.begin();
		// Iterate through the map
		while (it != p_mdec.end())
		{
			// Check if value of this entry matches with given value
			if (it->second.Trim() == (*it_set))
			{

				l_setSelected.insert(it->first);
				break;
			}
			// Go to next entry in map
			it++;
		}
		++it_set;
	}
	return l_setSelected;
}
void SP_WDG_DeclarationTreectrl::CheckAllDeclarations()
{

	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();
	bool l_bIsColoured = false;
	if (l_netType.Contains(wxT("Colored")))
	{
		l_bIsColoured = true;
	}
	if (l_bIsColoured)
	{
		m_mDeclarations2Dependencies.clear();
		set<wxString> l_setAllVariables;
		set<wxString> l_setAllFuns;
		set<wxString> l_setAllConst;
		set<wxString> l_setAllSimpleCs;
		set<wxString> l_setAllCompoundCs;

		/*compute color sets*/
		wxString l_sGroupSimple = wxT("Simple Color Sets");
		wxString l_sGroupCompound = wxT("Compound Color Sets");
		wxString l_sGroupAlias = wxT("Alias Color Sets");
		m_vtrimeddecSimplCS = m_unUsedDec.TrimUserDec(l_sGroupSimple);
		m_vtrimeddecCompoundCS = m_unUsedDec.TrimUserDec(l_sGroupCompound);
		m_vtrimeddecAliasCS = m_unUsedDec.TrimAliasColorSets(l_sGroupAlias);
		m_vtrimeddecALLCs.insert(m_vtrimeddecSimplCS.begin(), m_vtrimeddecSimplCS.end());
		m_vtrimeddecALLCs.insert(m_vtrimeddecCompoundCS.begin(), m_vtrimeddecCompoundCS.end());
		m_vtrimeddecALLCs.insert(m_vtrimeddecAliasCS.begin(), m_vtrimeddecAliasCS.end());
		m_setUnUsedColorSetsList.erase(m_setUnUsedColorSetsList.begin(), m_setUnUsedColorSetsList.end());//reset all color sets

																										 //compute for simple cs
		m_vSimpleCsInfo.clear();


		set<wxString> l_setUnusedSimpleCs;
		wxString l_sgroupVar = wxT("Variables");
		wxString l_sgroupConstants = wxT("Constants");
		wxString l_sgroupSimple = wxT("Simple Color Sets");
		wxString l_sgroupCompound = wxT("Compound Color Sets");
		wxString l_sgroupFun = wxT("Functions");

		m_vtrimeddecVariables = m_unUsedDec.TrimUserDec(l_sgroupVar);
		m_vtrimeddecConstants = m_unUsedDec.TrimUserDec(l_sgroupConstants);
		m_vtrimeddecSimplCS = m_unUsedDec.TrimUserDec(l_sgroupSimple);
		m_vtrimeddecCompoundCS = m_unUsedDec.TrimUserDec(l_sgroupCompound);
		m_vtrimeddecFunctions = m_unUsedDec.TrimUserDec(l_sgroupFun);



		set<wxString> l_setConstants;
		set<wxString> l_setVar;
		set<wxString> l_setFun;
		set<wxString> l_setComp;

		map<wxString, wxString> l_mCs2Type = m_unUsedDec.ObtainAllCs2Type();
		for (auto it_map = l_mCs2Type.begin(); it_map != l_mCs2Type.end(); ++it_map)
		{
			map<wxString, wxString> l_mDep2Type;
			if (it_map->second == wxT("Simple"))
			{
				SimpleCsDependencyInfo l_SimplecsInfo = m_unUsedDec.FindSimpleCSDependencies(it_map->first);
				if (l_SimplecsInfo.m_bIsUSed == false && it_map->first != wxT("Dot"))
				{
					l_setUnusedSimpleCs.insert(l_SimplecsInfo.m_sName);
					l_setUnusedSimpleCs.insert(l_SimplecsInfo.m_setIndirectCs.begin(), l_SimplecsInfo.m_setIndirectCs.end());
					m_vSimpleCsInfo.push_back(l_SimplecsInfo);
					l_setConstants.insert(l_SimplecsInfo.m_setConst.begin(), l_SimplecsInfo.m_setConst.end());
					l_setVar.insert(l_SimplecsInfo.m_setVar.begin(), l_SimplecsInfo.m_setVar.end());
					l_setFun.insert(l_SimplecsInfo.m_setFun.begin(), l_SimplecsInfo.m_setFun.end());
					l_setComp.insert(l_SimplecsInfo.m_setCompCs.begin(), l_SimplecsInfo.m_setCompCs.end());


					l_mDep2Type[l_SimplecsInfo.m_sName] = wxT("Simple");
					for (auto it_setFun = l_SimplecsInfo.m_setFun.begin(); it_setFun != l_SimplecsInfo.m_setFun.end(); ++it_setFun)
					{
						l_mDep2Type[*it_setFun] = wxT("Function");
					}
					for (auto it_Simp = l_SimplecsInfo.m_setVar.begin(); it_Simp != l_SimplecsInfo.m_setVar.end(); ++it_Simp)
					{
						l_mDep2Type[*it_Simp] = wxT("Variable");
					}
					for (auto it_Con = l_SimplecsInfo.m_setConst.begin(); it_Con != l_SimplecsInfo.m_setConst.end(); ++it_Con)
					{
						l_mDep2Type[*it_Con] = wxT("Constant");
					}
					for (auto it_Compound = l_SimplecsInfo.m_setCompCs.begin(); it_Compound != l_SimplecsInfo.m_setCompCs.end(); ++it_Compound)
					{
						l_mDep2Type[*it_Compound] = wxT("Compound");
					}
					m_mDeclarations2Dependencies[l_SimplecsInfo.m_sName] = l_mDep2Type;
				}

			}
		}//end for
		 /*compute for compound*/
		m_vCompoundCsInfo.clear();
		m_vSubCsInfo.clear();
		set<wxString> l_setSimplcs;
		set<wxString> l_setUnusedStructuredCs;
		//map<wxString, wxString> l_mCs2Type = l_UnDec.ObtainAllCs2Type();
		for (auto it_map = l_mCs2Type.begin(); it_map != l_mCs2Type.end(); ++it_map)
		{
			map<wxString, wxString> l_mDep2Type;
			if (it_map->second == wxT("Compound"))
			{
				CompoundCsDependencyInfo l_CompoundcsInfo = m_unUsedDec.FindCompoundCSDependencies(it_map->first);
				if (l_CompoundcsInfo.m_bIsUSed == false)
				{
					m_vCompoundCsInfo.push_back(l_CompoundcsInfo);
					l_setUnusedStructuredCs.insert(l_CompoundcsInfo.m_sName);
					l_setUnusedStructuredCs.insert(l_CompoundcsInfo.m_setIndirectCompCs.begin(), l_CompoundcsInfo.m_setIndirectCompCs.end());
					l_setConstants.insert(l_CompoundcsInfo.m_setConst.begin(), l_CompoundcsInfo.m_setConst.end());
					l_setVar.insert(l_CompoundcsInfo.m_setVar.begin(), l_CompoundcsInfo.m_setVar.end());
					l_setFun.insert(l_CompoundcsInfo.m_setFun.begin(), l_CompoundcsInfo.m_setFun.end());
					set<wxString> l_setExcludeUsedSimple;
					for (auto it_set = l_CompoundcsInfo.m_setSimpleCs.begin(); it_set != l_CompoundcsInfo.m_setSimpleCs.end(); ++it_set)
					{
						bool b = false;
						for (auto it_v = m_vSimpleCsInfo.begin(); it_v != m_vSimpleCsInfo.end(); ++it_v)

						{

							if (*it_set == it_v->m_sName)
							{
								l_setSimplcs.insert(*it_set);
								b = true;
							}


						}
						if (!b)
						{
							//		l_setExcludeUsedSimple.insert(*it_set);

						}
						b = false;
					}
					for (auto it_set1 = l_setExcludeUsedSimple.begin(); it_set1 != l_setExcludeUsedSimple.end(); ++it_set1)
					{
						//	l_CompoundcsInfo.m_setSimpleCs.erase(*it_set1);
					}
					l_mDep2Type[l_CompoundcsInfo.m_sName] = wxT("Compound");
					for (auto it_setFun = l_CompoundcsInfo.m_setFun.begin(); it_setFun != l_CompoundcsInfo.m_setFun.end(); ++it_setFun)
					{
						l_mDep2Type[*it_setFun] = wxT("Function");
					}
					for (auto it_Simp = l_CompoundcsInfo.m_setVar.begin(); it_Simp != l_CompoundcsInfo.m_setVar.end(); ++it_Simp)
					{
						l_mDep2Type[*it_Simp] = wxT("Variable");
					}
					for (auto it_Con = l_CompoundcsInfo.m_setConst.begin(); it_Con != l_CompoundcsInfo.m_setConst.end(); ++it_Con)
					{
						l_mDep2Type[*it_Con] = wxT("Constant");
					}
					for (auto it_Compound = l_CompoundcsInfo.m_setSimpleCs.begin(); it_Compound != l_CompoundcsInfo.m_setSimpleCs.end(); ++it_Compound)
					{
						l_mDep2Type[*it_Compound] = wxT("Simple");
					}
					m_mDeclarations2Dependencies[l_CompoundcsInfo.m_sName] = l_mDep2Type;

				}


			}
			else {
				if (!it_map->second.Contains(wxT("Simple")) && !it_map->second.Contains(wxT("Alias")))
				{
					SubSetCsDependencyInfo l_subInfo = m_unUsedDec.FindSubCSDependency(it_map->first);
					if (l_subInfo.m_bIsUSed == false)
					{
						m_vSubCsInfo.push_back(l_subInfo);
						l_setUnusedStructuredCs.insert(l_subInfo.m_sName);
						l_setConstants.insert(l_subInfo.m_setConst.begin(), l_subInfo.m_setConst.end());
						l_setVar.insert(l_subInfo.m_setVar.begin(), l_subInfo.m_setVar.end());
						l_setFun.insert(l_subInfo.m_setFun.begin(), l_subInfo.m_setFun.end());
						l_setSimplcs.insert(l_subInfo.m_setSimpleCs.begin(), l_subInfo.m_setSimpleCs.end());

						l_mDep2Type[l_subInfo.m_sName] = wxT("Compound");
						for (auto it_setFun = l_subInfo.m_setFun.begin(); it_setFun != l_subInfo.m_setFun.end(); ++it_setFun)
						{
							l_mDep2Type[*it_setFun] = wxT("Function");
						}
						for (auto it_Simp = l_subInfo.m_setVar.begin(); it_Simp != l_subInfo.m_setVar.end(); ++it_Simp)
						{
							l_mDep2Type[*it_Simp] = wxT("Variable");
						}
						for (auto it_Con = l_subInfo.m_setConst.begin(); it_Con != l_subInfo.m_setConst.end(); ++it_Con)
						{
							l_mDep2Type[*it_Con] = wxT("Constant");
						}
						for (auto it_Compound = l_subInfo.m_setSimpleCs.begin(); it_Compound != l_subInfo.m_setSimpleCs.end(); ++it_Compound)
						{
							l_mDep2Type[*it_Compound] = wxT("Simple");
						}
						m_mDeclarations2Dependencies[l_subInfo.m_sName] = l_mDep2Type;
					}

				}
			}
		}//end for
		 /**********Alias Cs**********/
		m_vAliasCsInfo.clear();
		map<wxString, wxString> l_mCs2Type1;
		l_mCs2Type1 = m_unUsedDec.ObtainAllCs2Type();

		set<wxString> l_setConstantsForAlias;
		set<wxString> l_setVarForAlias;
		set<wxString> l_setFunForAlias;
		set<wxString> l_setSimplcsForAlias;
		set<wxString> l_setCompcsForAlias;
		set<wxString> l_setAlias;
		for (auto it_m = l_mCs2Type1.begin(); it_m != l_mCs2Type1.end(); ++it_m)
		{
			map<wxString, wxString> l_mDep2Type;
			if (it_m->second == wxT("Alias"))
			{
				AliasCsDependencyInfo l_aliasInf = m_unUsedDec.FindAliasCSDependency(it_m->first);
				if (l_aliasInf.m_bIsUSed == false)
				{


					m_vAliasCsInfo.push_back(l_aliasInf);
					l_setAlias.insert(l_aliasInf.m_sName);
					l_setConstantsForAlias.insert(l_aliasInf.m_setConst.begin(), l_aliasInf.m_setConst.end());
					l_setVarForAlias.insert(l_aliasInf.m_setVar.begin(), l_aliasInf.m_setVar.end());
					l_setFunForAlias.insert(l_aliasInf.m_setFun.begin(), l_aliasInf.m_setFun.end());
					l_setSimplcsForAlias.insert(l_aliasInf.m_setSimpleCs.begin(), l_aliasInf.m_setSimpleCs.end());
					l_setCompcsForAlias.insert(l_aliasInf.m_setCompCs.begin(), l_aliasInf.m_setCompCs.end());



					l_mDep2Type[l_aliasInf.m_sName] = wxT("Alias");
					for (auto it_setFun = l_aliasInf.m_setFun.begin(); it_setFun != l_aliasInf.m_setFun.end(); ++it_setFun)
					{
						l_mDep2Type[*it_setFun] = wxT("Function");
					}
					for (auto it_Simp = l_aliasInf.m_setVar.begin(); it_Simp != l_aliasInf.m_setVar.end(); ++it_Simp)
					{
						l_mDep2Type[*it_Simp] = wxT("Variable");
					}
					for (auto it_Con = l_aliasInf.m_setConst.begin(); it_Con != l_aliasInf.m_setConst.end(); ++it_Con)
					{
						l_mDep2Type[*it_Con] = wxT("Constant");
					}
					for (auto it_Compound = l_aliasInf.m_setSimpleCs.begin(); it_Compound != l_aliasInf.m_setSimpleCs.end(); ++it_Compound)
					{
						l_mDep2Type[*it_Compound] = wxT("Simple");
					}

					for (auto it_Compound = l_aliasInf.m_setCompCs.begin(); it_Compound != l_aliasInf.m_setCompCs.end(); ++it_Compound)
					{
						l_mDep2Type[*it_Compound] = wxT("Compound");
					}
					m_mDeclarations2Dependencies[l_aliasInf.m_sName] = l_mDep2Type;


				}

			}
		}



		/************************/
		//merge unused simple cs and merge compound
		l_setSimplcs.insert(l_setUnusedSimpleCs.begin(), l_setUnusedSimpleCs.end());
		l_setSimplcs.insert(l_setSimplcsForAlias.begin(), l_setSimplcsForAlias.end());

		l_setUnusedStructuredCs.insert(l_setComp.begin(), l_setComp.end());
		l_setUnusedStructuredCs.insert(l_setCompcsForAlias.begin(), l_setCompcsForAlias.end());


		l_setVar.insert(l_setVarForAlias.begin(), l_setVarForAlias.end());
		l_setFun.insert(l_setFunForAlias.begin(), l_setFunForAlias.end());
		l_setConstants.insert(l_setConstantsForAlias.begin(), l_setConstantsForAlias.end());


		l_setAllSimpleCs.insert(m_setUnUsedSimpleCS.begin(), m_setUnUsedSimpleCS.end());
		l_setAllVariables.insert(l_setVar.begin(), l_setVar.end());
		l_setAllFuns.insert(l_setFun.begin(), l_setFun.end());
		l_setAllConst.insert(l_setConstants.begin(), l_setConstants.end());
		l_setAllCompoundCs.insert(l_setUnusedStructuredCs.begin(), l_setUnusedStructuredCs.end());
		/*constants*/
		m_vConstantsInfo.clear();
		set<wxString> l_setAllselectedConst;


		set<wxString> l_setFun1;
		set<wxString> l_setVar1;
		set<wxString> l_setSimpl1;
		set<wxString> l_setComp1;


		set<wxString> l_setAllconstant = m_unUsedDec.ObtainAllDefinedConstants();
		for (auto it_ConstantSet = l_setAllconstant.begin(); it_ConstantSet != l_setAllconstant.end(); ++it_ConstantSet)
		{
			map<wxString, wxString> l_mDep2Type;
			ConstantDependencyInfo l_Cinfo = m_unUsedDec.FindConstantsDependencies(*it_ConstantSet);

			if (l_Cinfo.m_bIsUsed == false) {
				m_vConstantsInfo.push_back(l_Cinfo);
				l_setAllselectedConst.insert(l_Cinfo.m_sConstName);
				l_setFun1.insert(l_Cinfo.m_setFun.begin(), l_Cinfo.m_setFun.end());
				l_setVar1.insert(l_Cinfo.m_setVar.begin(), l_Cinfo.m_setVar.end());
				l_setSimpl1.insert(l_Cinfo.m_SetSimpleCS.begin(), l_Cinfo.m_SetSimpleCS.end());
				l_setComp1.insert(l_Cinfo.m_SetCompoundCS.begin(), l_Cinfo.m_SetCompoundCS.end());


				l_mDep2Type[l_Cinfo.m_sConstName] = wxT("Constant");
				for (auto it_setFun = l_Cinfo.m_setFun.begin(); it_setFun != l_Cinfo.m_setFun.end(); ++it_setFun)
				{
					l_mDep2Type[*it_setFun] = wxT("Function");
				}
				for (auto it_Simp = l_Cinfo.m_SetSimpleCS.begin(); it_Simp != l_Cinfo.m_SetSimpleCS.end(); ++it_Simp)
				{
					l_mDep2Type[*it_Simp] = wxT("Simple");
				}

				for (auto it_Compound = l_Cinfo.m_SetCompoundCS.begin(); it_Compound != l_Cinfo.m_SetCompoundCS.end(); ++it_Compound)
				{
					l_mDep2Type[*it_Compound] = wxT("Compound");
				}
				for (auto it_Var = l_Cinfo.m_setVar.begin(); it_Var != l_Cinfo.m_setVar.end(); ++it_Var)
				{
					l_mDep2Type[*it_Var] = wxT("Variable");
				}
				m_mDeclarations2Dependencies[l_Cinfo.m_sConstName] = l_mDep2Type;
			}


		}



		l_setAllSimpleCs.insert(l_setSimpl1.begin(), l_setSimpl1.end());
		l_setAllVariables.insert(l_setVar1.begin(), l_setVar1.end());
		l_setAllFuns.insert(l_setFun1.begin(), l_setFun1.end());
		l_setAllConst.insert(l_setAllselectedConst.begin(), l_setAllselectedConst.end());
		l_setAllCompoundCs.insert(l_setComp1.begin(), l_setComp1.end());
		/*functions*/
		m_vFunctionsInfo.clear();
		set<wxString> l_setUnusedFuns;

		set<wxString> l_setConstants2;
		set<wxString> l_setVar2;
		set<wxString> l_setSimpl2;
		set<wxString> l_setComp2;

		set<wxString> l_setAlldefFuns = m_unUsedDec.ReadAllDefinedFunctions();
		for (auto it_set = l_setAlldefFuns.begin(); it_set != l_setAlldefFuns.end(); ++it_set)
		{
			map<wxString, wxString> l_mDep2Type;
			FunctionDependencyInfo l_FunDepInf = m_unUsedDec.FindFunctionDependencies(*it_set);
			if (l_FunDepInf.m_bIsUSed == false)
			{
				l_setUnusedFuns.insert(l_FunDepInf.m_sFunName);
				m_vFunctionsInfo.push_back(l_FunDepInf);

				l_setConstants2.insert(l_FunDepInf.m_setConst.begin(), l_FunDepInf.m_setConst.end());
				l_setVar2.insert(l_FunDepInf.m_setVars.begin(), l_FunDepInf.m_setVars.end());
				l_setSimpl2.insert(l_FunDepInf.m_setSimpleCs.begin(), l_FunDepInf.m_setSimpleCs.end());
				l_setComp2.insert(l_FunDepInf.m_setCompoundCs.begin(), l_FunDepInf.m_setCompoundCs.end());

				l_mDep2Type[l_FunDepInf.m_sFunName] = wxT("Function");

				for (auto it_var = l_FunDepInf.m_setVars.begin(); it_var != l_FunDepInf.m_setVars.end(); ++it_var)
				{
					l_mDep2Type[*it_var] = wxT("Variable");
				}


				for (auto it_Simp = l_FunDepInf.m_setSimpleCs.begin(); it_Simp != l_FunDepInf.m_setSimpleCs.end(); ++it_Simp)
				{
					l_mDep2Type[*it_Simp] = wxT("Simple");
				}

				for (auto it_Compound = l_FunDepInf.m_setCompoundCs.begin(); it_Compound != l_FunDepInf.m_setCompoundCs.end(); ++it_Compound)
				{
					l_mDep2Type[*it_Compound] = wxT("Compound");
				}
				m_mDeclarations2Dependencies[l_FunDepInf.m_sFunName] = l_mDep2Type;
			}
		}

		l_setAllSimpleCs.insert(l_setSimpl2.begin(), l_setSimpl2.end());
		l_setAllVariables.insert(l_setVar2.begin(), l_setVar2.end());
		l_setAllFuns.insert(l_setUnusedFuns.begin(), l_setUnusedFuns.end());
		l_setAllConst.insert(l_setConstants2.begin(), l_setConstants2.end());
		l_setAllCompoundCs.insert(l_setComp2.begin(), l_setComp2.end());


		/*******var*****/
		m_vVariablesInfo.clear();



		set<wxString> l_setUnusedVar;


		set<wxString> l_setFunforvar;
		set<wxString> l_setConforvar;
		set<wxString> l_setSimplforvar;
		set<wxString> l_setCompforvar;


		map<wxString, wxString> l_mAlldefinedVar = m_unUsedDec.ObtainAllVar2Cs();
		for (auto itmap = l_mAlldefinedVar.begin(); itmap != l_mAlldefinedVar.end(); ++itmap)
		{
			map<wxString, wxString> l_mDep2Type;
			VariableDependencyInfo l_varDependencyInfo = m_unUsedDec.FindVariableDependencies(itmap->first);
			if (l_varDependencyInfo.m_bIsUSed == false)
			{
				m_vVariablesInfo.push_back(l_varDependencyInfo);
				l_setUnusedVar.insert(l_varDependencyInfo.m_sName);
				l_setFunforvar.insert(l_varDependencyInfo.m_setFun.begin(), l_varDependencyInfo.m_setFun.end());
				l_setSimplforvar.insert(l_varDependencyInfo.m_setSimpleCs.begin(), l_varDependencyInfo.m_setSimpleCs.end());
				l_setConforvar.insert(l_varDependencyInfo.m_setConst.begin(), l_varDependencyInfo.m_setConst.end());
				l_setCompforvar.insert(l_varDependencyInfo.m_CompoundCS.begin(), l_varDependencyInfo.m_CompoundCS.end());

				l_mDep2Type[l_varDependencyInfo.m_sName] = wxT("Variable");
				for (auto it_setFun = l_varDependencyInfo.m_setFun.begin(); it_setFun != l_varDependencyInfo.m_setFun.end(); ++it_setFun)
				{
					l_mDep2Type[*it_setFun] = wxT("Function");
				}
				for (auto it_Simp = l_varDependencyInfo.m_setSimpleCs.begin(); it_Simp != l_varDependencyInfo.m_setSimpleCs.end(); ++it_Simp)
				{
					l_mDep2Type[*it_Simp] = wxT("Simple");
				}
				for (auto it_Con = l_varDependencyInfo.m_setConst.begin(); it_Con != l_varDependencyInfo.m_setConst.end(); ++it_Con)
				{
					l_mDep2Type[*it_Con] = wxT("Constant");
				}
				for (auto it_Compound = l_varDependencyInfo.m_CompoundCS.begin(); it_Compound != l_varDependencyInfo.m_CompoundCS.end(); ++it_Compound)
				{
					l_mDep2Type[*it_Compound] = wxT("Compound");
				}
				m_mDeclarations2Dependencies[l_varDependencyInfo.m_sName] = l_mDep2Type;
			}
		}

		l_setAllSimpleCs.insert(l_setSimplforvar.begin(), l_setSimplforvar.end());
		l_setAllVariables.insert(l_setUnusedVar.begin(), l_setUnusedVar.end());
		l_setAllFuns.insert(l_setFunforvar.begin(), l_setFunforvar.end());
		l_setAllConst.insert(l_setConforvar.begin(), l_setConforvar.end());
		l_setAllCompoundCs.insert(l_setCompforvar.begin(), l_setCompforvar.end());


		/****************/
		m_mGroup2SelectedSet[l_sgroupConstants] = SetSelectItems(m_vtrimeddecConstants, l_setAllConst);
		m_mGroup2SelectedSet[l_sgroupVar] = SetSelectItems(m_vtrimeddecVariables, l_setAllVariables);
		m_mGroup2SelectedSet[l_sgroupFun] = SetSelectItems(m_vtrimeddecFunctions, l_setAllFuns);
		m_mGroup2SelectedSet[l_sgroupSimple] = SetSelectItems(m_vtrimeddecSimplCS, l_setAllSimpleCs);
		m_mGroup2SelectedSet[l_sgroupCompound] = SetSelectItems(m_vtrimeddecCompoundCS, l_setAllCompoundCs);
		//reset selected list and assign curren items of all dec
		m_vUnUsedConstantsList.erase(m_vUnUsedConstantsList.begin(), m_vUnUsedConstantsList.end());
		m_vUnUsedConstantsList.insert(l_setAllConst.begin(), l_setAllConst.end());
		m_setUnUsedFunctionsList.erase(m_setUnUsedFunctionsList.begin(), m_setUnUsedFunctionsList.end());
		m_setUnUsedFunctionsList.insert(l_setAllFuns.begin(), l_setAllFuns.end());
		m_setUnUsedVariablesList.erase(m_setUnUsedVariablesList.begin(), m_setUnUsedVariablesList.end());
		m_setUnUsedVariablesList.insert(l_setAllVariables.begin(), l_setAllVariables.end());
		m_setUnUsedSimpleCS.erase(m_setUnUsedSimpleCS.begin(), m_setUnUsedSimpleCS.end());
		m_setUnUsedSimpleCS.insert(l_setAllSimpleCs.begin(), l_setAllSimpleCs.end());
		m_setUnUsedCompoundCS.erase(m_setUnUsedCompoundCS.begin(), m_setUnUsedCompoundCS.end());
		m_setUnUsedCompoundCS.insert(l_setAllCompoundCs.begin(), l_setAllCompoundCs.end());

		m_setUnUsedAliasCS.clear();
		m_setUnUsedAliasCS.insert(l_setAlias.begin(), l_setAlias.end());
		/*if there is no unused dec, toggel log window with this info*/

		//if (m_vUnUsedConstantsList.size() == 0 && m_setUnUsedFunctionsList.size() == 0 && m_setUnUsedVariablesList.size() == 0 && m_setUnUsedSimpleCS.size() == 0 && m_setUnUsedCompoundCS.size() == 0)
		//{
		//SP_LOGDEBUG(wxT("no unused declarations"));
		//}

		//if (m_mDeclarations2Dependencies.size() == 0)
		//{
		//	wxString s_warningMsg = wxT("no unused declarations");
		//	SP_LOGWARNING(s_warningMsg);// add the message to log window
		//}

	}
	else {
		//check all un-colored declarations
		m_mDeclarations2Dependencies.clear();
		wxString l_sgroupCon = wxT("Constants");
		m_vtrimeddecConstants = m_unUsedDec.TrimUserDec(l_sgroupCon);

		m_vUnUsedConstantsList = m_unUsedDec.FindUnusedConstants();


		m_vConstantsInfo.clear();
		set<wxString> l_setAllselectedConst;

		wxString l_sgroupFun = wxT("Functions");
		wxString l_sgroupObservers = wxT("Observers");




		m_vtrimeddecFunctions = m_unUsedDec.TrimUserDec(l_sgroupFun);
		m_vtrimeddecObservers = m_unUsedDec.TrimUserDec(l_sgroupObservers);


		UpdateSetODeclarations(m_vtrimeddecConstants);
		UpdateSetODeclarations(m_vtrimeddecFunctions);
		UpdateSetODeclarations(m_vtrimeddecObservers);

		set<wxString> l_setFun;
		set<wxString> l_setObservers;



		set<wxString> l_setAllconstant = m_unUsedDec.ObtainAllDefinedConstants();
		for (auto it_ConstantSet = l_setAllconstant.begin(); it_ConstantSet != l_setAllconstant.end(); ++it_ConstantSet)
		{
			map<wxString, wxString> l_mDep2Type;
			ConstantDependencyInfo l_Cinfo = m_unUsedDec.FindConstantsDependencies(*it_ConstantSet);

			if (l_Cinfo.m_bIsUsed == false) {

				m_vConstantsInfo.push_back(l_Cinfo);
				l_setAllselectedConst.insert(l_Cinfo.m_sConstName);

				l_setFun.insert(l_Cinfo.m_setFun.begin(), l_Cinfo.m_setFun.end());


				l_mDep2Type[l_Cinfo.m_sConstName] = wxT("Constant");
				for (auto it_setFun = l_Cinfo.m_setFun.begin(); it_setFun != l_Cinfo.m_setFun.end(); ++it_setFun)
				{
					l_mDep2Type[*it_setFun] = wxT("Function");
				}


				for (auto it_Var = l_Cinfo.m_setObservers.begin(); it_Var != l_Cinfo.m_setObservers.end(); ++it_Var)
				{
					l_mDep2Type[*it_Var] = wxT("Observer");
				}
				m_mDeclarations2Dependencies[l_Cinfo.m_sConstName] = l_mDep2Type;
			}


		}
		m_vUnUsedConstantsList.clear();
		m_vUnUsedConstantsList.insert(l_setAllselectedConst.begin(), l_setAllselectedConst.end());
		m_setUnUsedFunctionsList.insert(l_setFun.begin(), l_setFun.end());
		//funs


		m_setUnUsedFunctionsList = m_unUsedDec.FindUnusedFunctions();

		UpdateSetODeclarations(m_vtrimeddecConstants);
		UpdateSetODeclarations(m_vtrimeddecFunctions);
		UpdateSetODeclarations(m_vtrimeddecObservers);


		m_vFunctionsInfo.clear();
		set<wxString> l_setUnusedFuns;

		wxString l_sgroupConstants = wxT("Constants");
		set<wxString> l_setConstants;
		set<wxString> l_setObservers1;

		set<wxString> l_setAlldefFuns = m_unUsedDec.ReadAllDefinedFunctions();
		for (auto it_set = l_setAlldefFuns.begin(); it_set != l_setAlldefFuns.end(); ++it_set)
		{
			map<wxString, wxString> l_mDep2Type;
			FunctionDependencyInfo l_FunDepInf = m_unUsedDec.FindFunctionDependencies(*it_set);
			if (l_FunDepInf.m_bIsUSed == false)
			{
				l_setUnusedFuns.insert(l_FunDepInf.m_sFunName);
				m_vFunctionsInfo.push_back(l_FunDepInf);

				l_setConstants.insert(l_FunDepInf.m_setConst.begin(), l_FunDepInf.m_setConst.end());
				l_setObservers1.insert(l_FunDepInf.m_setObservers.begin(), l_FunDepInf.m_setObservers.end());

				l_mDep2Type[l_FunDepInf.m_sFunName] = wxT("Function");

				for (auto it_Con = l_setConstants.begin(); it_Con != l_setConstants.end(); ++it_Con)
				{
					l_mDep2Type[*it_Con] = wxT("Constant");
				}
				for (auto it_Con = l_setObservers1.begin(); it_Con != l_setObservers1.end(); ++it_Con)
				{
					l_mDep2Type[*it_Con] = wxT("Observer");
				}
				m_mDeclarations2Dependencies[l_FunDepInf.m_sFunName] = l_mDep2Type;


			}



		}
		m_setUnUsedFunctionsList.clear();
		m_setUnUsedFunctionsList.insert(l_setUnusedFuns.begin(), l_setUnusedFuns.end());

		//observers
		wxString l_sobserv = wxT("Observers");
		m_vtrimeddecObservers = m_unUsedDec.TrimUserDec(l_sobserv);

		m_setUnUsedObserverList = m_unUsedDec.FindUnusedObservers();


		UpdateSetODeclarations(m_vtrimeddecConstants);
		UpdateSetODeclarations(m_vtrimeddecFunctions);
		UpdateSetODeclarations(m_vtrimeddecObservers);

		m_vObserversInfo.clear();
		set<wxString> l_setUnusedObservers;
		set<wxString> l_setConstants1;
		set<wxString> l_setFun2;


		map<wxString, set<wxString>> l_mapAlldefObservers = m_unUsedDec.ObtainObserversConstantsDependent();
		for (auto it_map = l_mapAlldefObservers.begin(); it_map != l_mapAlldefObservers.end(); ++it_map)
		{
			map<wxString, wxString> l_mDep2Type;
			ObserverDependencyInfo l_ObserverInf = m_unUsedDec.FindObserverDependency(it_map->first);
			if (l_ObserverInf.m_bIsUsed == false)
			{
				l_setUnusedObservers.insert(l_ObserverInf.m_observerName);
				m_vObserversInfo.push_back(l_ObserverInf);

				l_setConstants1.insert(l_ObserverInf.m_setConstants.begin(), l_ObserverInf.m_setConstants.end());
				//
				l_setFun2.insert(l_ObserverInf.m_setFuns.begin(), l_ObserverInf.m_setFuns.end());
				l_setFun.insert(l_ObserverInf.m_setForwardFuns.begin(), l_ObserverInf.m_setForwardFuns.end());

				l_mDep2Type[l_ObserverInf.m_observerName] = wxT("Observer");

				for (auto it_Con = l_setFun2.begin(); it_Con != l_setFun2.end(); ++it_Con)
				{
					l_mDep2Type[*it_Con] = wxT("Function");
				}

				for (auto it_Con = l_setConstants1.begin(); it_Con != l_setConstants1.end(); ++it_Con)
				{
					l_mDep2Type[*it_Con] = wxT("Constant");
				}
				m_mDeclarations2Dependencies[l_ObserverInf.m_observerName] = l_mDep2Type;


			}



		}



	}

}
void SP_WDG_DeclarationTreectrl::OnCheckForUnusedItems(wxCommandEvent& p_cEvent)//Added by G.Assaf
{
	m_bisSecond = true;
	m_bisIsFirst = true;
	wxTreeCtrl* pTreeCtrl = this;// m_pcGraph->GetDeclarationTree();
	wxTreeItemId l_root = pTreeCtrl->GetRootItem();
	wxTreeItemIdValue l_nCookie = NULL;
	wxTreeItemId first = pTreeCtrl->GetFirstChild(l_root, l_nCookie);
	SP_VectorString v_declTree;
	ReadTreeItems(pTreeCtrl, l_root, v_declTree);
	SP_DS_UnUsedDeclaration  l_UnDec(m_pcGraph, v_declTree);
	m_unUsedDec = l_UnDec;
	std::map<wxString, wxString> l_vtrimeddec;

	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();
	bool l_bIsColoured = false;
	if (l_netType.Contains(wxT("Colored")))
	{
		l_bIsColoured = true;
	}

	if (!l_bIsColoured)
	{
		//uncolored pn
		wxString l_sDec = m_sSelected;
		int l_iUnuusedConstantsCount = 0;
		int l_iUnusedFunctionsCount = 0;
		int l_iUnusedObserversCount = 0;

		if (m_sSelected == wxT("Constants"))
		{

			CheckAllDeclarations();

			m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sDec);

			m_vUnUsedConstantsList = l_UnDec.FindUnusedConstants();


			m_vConstantsInfo.clear();
			set<wxString> l_setAllselectedConst;

			wxString l_sgroupFun = wxT("Functions");
			wxString l_sgroupObservers = wxT("Observers");
			m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupFun);
			m_vtrimeddecObservers = l_UnDec.TrimUserDec(l_sgroupObservers);

			//reset the tree view 
			UpdateSetODeclarations(m_vtrimeddecConstants);
			UpdateSetODeclarations(m_vtrimeddecFunctions);
			UpdateSetODeclarations(m_vtrimeddecObservers);

			set<wxString> l_setFun;
			set<wxString> l_setObservers;
			bool l_bIsUnused = false;

			set<wxString> l_setAllconstant = l_UnDec.ObtainAllDefinedConstants();
			for (auto it_ConstantSet = l_setAllconstant.begin(); it_ConstantSet != l_setAllconstant.end(); ++it_ConstantSet)
			{
				map<wxString, wxString> l_mDep2Type;
				ConstantDependencyInfo l_Cinfo = l_UnDec.FindConstantsDependencies(*it_ConstantSet);

				if (l_Cinfo.m_bIsUsed == false) {

					l_bIsUnused = true;
					l_iUnuusedConstantsCount++;
					m_vConstantsInfo.push_back(l_Cinfo);
					l_setAllselectedConst.insert(l_Cinfo.m_sConstName);
					l_setFun.insert(l_Cinfo.m_setFun.begin(), l_Cinfo.m_setFun.end());


					l_mDep2Type[l_Cinfo.m_sConstName] = wxT("Constant");
					for (auto it_setFun = l_Cinfo.m_setFun.begin(); it_setFun != l_Cinfo.m_setFun.end(); ++it_setFun)
					{
						l_mDep2Type[*it_setFun] = wxT("Function");
					}


					for (auto it_Var = l_Cinfo.m_setObservers.begin(); it_Var != l_Cinfo.m_setObservers.end(); ++it_Var)
					{
						l_mDep2Type[*it_Var] = wxT("Observer");
					}
					m_mDeclarations2Dependencies[l_Cinfo.m_sConstName] = l_mDep2Type;
				}


			}
			m_vUnUsedConstantsList.clear();
			m_vUnUsedConstantsList.insert(l_setAllselectedConst.begin(), l_setAllselectedConst.end());
			m_setUnUsedFunctionsList.insert(l_setFun.begin(), l_setFun.end());

			if (!l_bIsUnused)//report when no unused  consts found
			{
				wxString s_warningMsg = wxT("no unused declarations (constants)");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else {
				wxString l_sFoundReport = wxT("the number of unused Constants is:");
				l_sFoundReport << l_iUnuusedConstantsCount;
				SP_LOGMESSAGE(l_sFoundReport);
			}

		}
		else if (m_sSelected == wxT("Functions"))
		{
			CheckAllDeclarations();

			m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sDec);

			m_setUnUsedFunctionsList = l_UnDec.FindUnusedFunctions();

			UpdateSetODeclarations(m_vtrimeddecConstants);
			UpdateSetODeclarations(m_vtrimeddecFunctions);
			UpdateSetODeclarations(m_vtrimeddecObservers);


			m_vFunctionsInfo.clear();
			set<wxString> l_setUnusedFuns;
			wxString l_sgroupConstants = wxT("Constants");
			wxString l_sgroupFun = wxT("Functions");
			wxString l_sgroupObservers = wxT("Observers");


			m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupConstants);
			m_vtrimeddecObservers = l_UnDec.TrimUserDec(l_sgroupObservers);
			m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupFun);

			set<wxString> l_setConstants;
			set<wxString> l_setObservers;
			bool l_bIsUnused = false;
			set<wxString> l_setAlldefFuns = l_UnDec.ReadAllDefinedFunctions();
			for (auto it_set = l_setAlldefFuns.begin(); it_set != l_setAlldefFuns.end(); ++it_set)
			{
				map<wxString, wxString> l_mDep2Type;
				FunctionDependencyInfo l_FunDepInf = l_UnDec.FindFunctionDependencies(*it_set);
				if (l_FunDepInf.m_bIsUSed == false)
				{
					l_iUnusedFunctionsCount++;
					l_bIsUnused = true;
					l_setUnusedFuns.insert(l_FunDepInf.m_sFunName);
					m_vFunctionsInfo.push_back(l_FunDepInf);

					l_setConstants.insert(l_FunDepInf.m_setConst.begin(), l_FunDepInf.m_setConst.end());
					l_setObservers.insert(l_FunDepInf.m_setObservers.begin(), l_FunDepInf.m_setObservers.end());

					l_mDep2Type[l_FunDepInf.m_sFunName] = wxT("Function");

					for (auto it_Con = l_setConstants.begin(); it_Con != l_setConstants.end(); ++it_Con)
					{
						l_mDep2Type[*it_Con] = wxT("Constant");
					}
					for (auto it_Con = l_setObservers.begin(); it_Con != l_setObservers.end(); ++it_Con)
					{
						l_mDep2Type[*it_Con] = wxT("Observer");
					}
					m_mDeclarations2Dependencies[l_FunDepInf.m_sFunName] = l_mDep2Type;


				}



			}
			m_setUnUsedFunctionsList.clear();
			m_setUnUsedFunctionsList.insert(l_setUnusedFuns.begin(), l_setUnusedFuns.end());


			if (!l_bIsUnused)
			{
				wxString s_warningMsg = wxT("no unused declarations (Functions)");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else {
				wxString l_sFoundReport = wxT("the number of unused Functions is:");
				l_sFoundReport << l_iUnusedFunctionsCount;
				SP_LOGMESSAGE(l_sFoundReport);
			}

		}
		else if (m_sSelected == wxT("Observers"))
		{
			CheckAllDeclarations();
			m_vtrimeddecObservers = l_UnDec.TrimUserDec(m_sSelected);

			m_setUnUsedObserverList = l_UnDec.FindUnusedObservers();


			UpdateSetODeclarations(m_vtrimeddecConstants);
			UpdateSetODeclarations(m_vtrimeddecFunctions);
			UpdateSetODeclarations(m_vtrimeddecObservers);


			m_vObserversInfo.clear();

			set<wxString> l_setUnusedObservers;
			set<wxString> l_setConstants;
			set<wxString> l_setFun;
			bool l_bIsUnused = false;

			map<wxString, set<wxString>> l_mapAlldefObservers = l_UnDec.ObtainObserversConstantsDependent();
			for (auto it_map = l_mapAlldefObservers.begin(); it_map != l_mapAlldefObservers.end(); ++it_map)
			{
				map<wxString, wxString> l_mDep2Type;
				ObserverDependencyInfo l_ObserverInf = l_UnDec.FindObserverDependency(it_map->first);
				if (l_ObserverInf.m_bIsUsed == false)
				{
					l_iUnusedObserversCount++;
					l_bIsUnused = true;
					l_setUnusedObservers.insert(l_ObserverInf.m_observerName);
					m_vObserversInfo.push_back(l_ObserverInf);

					l_setConstants.insert(l_ObserverInf.m_setConstants.begin(), l_ObserverInf.m_setConstants.end());
					//
					l_setFun.insert(l_ObserverInf.m_setFuns.begin(), l_ObserverInf.m_setFuns.end());
					l_setFun.insert(l_ObserverInf.m_setForwardFuns.begin(), l_ObserverInf.m_setForwardFuns.end());

					l_mDep2Type[l_ObserverInf.m_observerName] = wxT("Observer");

					for (auto it_Con = l_setFun.begin(); it_Con != l_setFun.end(); ++it_Con)
					{
						l_mDep2Type[*it_Con] = wxT("Function");
					}


					for (auto it_Con = l_setConstants.begin(); it_Con != l_setConstants.end(); ++it_Con)
					{
						l_mDep2Type[*it_Con] = wxT("Constant");
					}
					m_mDeclarations2Dependencies[l_ObserverInf.m_observerName] = l_mDep2Type;


				}



			}

			m_setUnUsedObserverList = l_setUnusedObservers;
			if (!l_bIsUnused)//no unused observers
			{
				wxString s_warningMsg = wxT("no unused declarations (Observers)");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else {
				wxString l_sFoundReport = wxT("the number of unused Functions is:");
				l_sFoundReport << l_iUnusedObserversCount;
				SP_LOGMESSAGE(l_sFoundReport);
			}
		}
		else {

			CheckAllDeclarations();

			m_vFunctionsInfo.clear();
			m_vConstantsInfo.clear();
			m_vObserversInfo.clear();
			m_mDeclarations2Dependencies.clear();
			set<wxString> l_setAllselectedConst;

			wxString l_sgroupConstants = wxT("Constants");
			wxString l_sgroupFun = wxT("Functions");
			wxString l_sgroupObservers = wxT("Observers");


			m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupFun);
			m_vtrimeddecObservers = l_UnDec.TrimUserDec(l_sgroupObservers);
			m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupConstants);


			UpdateSetODeclarations(m_vtrimeddecConstants);
			UpdateSetODeclarations(m_vtrimeddecFunctions);
			UpdateSetODeclarations(m_vtrimeddecObservers);



			set<wxString> l_setFun;
			set<wxString> l_setObservers;

			set<wxString> l_setAllconstant = l_UnDec.ObtainAllDefinedConstants();
			for (auto it_ConstantSet = l_setAllconstant.begin(); it_ConstantSet != l_setAllconstant.end(); ++it_ConstantSet)
			{
				map<wxString, wxString> l_mDep2Type;
				ConstantDependencyInfo l_Cinfo = l_UnDec.FindConstantsDependencies(*it_ConstantSet);

				if (l_Cinfo.m_bIsUsed == false) {
					l_iUnuusedConstantsCount++;
					m_vConstantsInfo.push_back(l_Cinfo);
					l_setAllselectedConst.insert(l_Cinfo.m_sConstName);
					l_setFun.insert(l_Cinfo.m_setFun.begin(), l_Cinfo.m_setFun.end());


					l_mDep2Type[l_Cinfo.m_sConstName] = wxT("Constant");
					for (auto it_setFun = l_Cinfo.m_setFun.begin(); it_setFun != l_Cinfo.m_setFun.end(); ++it_setFun)
					{
						l_mDep2Type[*it_setFun] = wxT("Function");
					}

					for (auto it_setFun = l_Cinfo.m_setForwardFun.begin(); it_setFun != l_Cinfo.m_setForwardFun.end(); ++it_setFun)
					{
						l_mDep2Type[*it_setFun] = wxT("Function");
					}

					for (auto it_Var = l_Cinfo.m_setObservers.begin(); it_Var != l_Cinfo.m_setObservers.end(); ++it_Var)
					{
						l_mDep2Type[*it_Var] = wxT("Observer");
					}
					m_mDeclarations2Dependencies[l_Cinfo.m_sConstName] = l_mDep2Type;
				}


			}

			//functions


			set<wxString> l_setConstants;
			set<wxString> l_setObservers1;
			set<wxString> l_setUnusedFuns;

			set<wxString> l_setAlldefFuns = l_UnDec.ReadAllDefinedFunctions();
			for (auto it_set = l_setAlldefFuns.begin(); it_set != l_setAlldefFuns.end(); ++it_set)
			{
				map<wxString, wxString> l_mDep2Type;
				FunctionDependencyInfo l_FunDepInf = l_UnDec.FindFunctionDependencies(*it_set);
				if (l_FunDepInf.m_bIsUSed == false)
				{
					l_iUnusedFunctionsCount++;

					l_setUnusedFuns.insert(l_FunDepInf.m_sFunName);
					m_vFunctionsInfo.push_back(l_FunDepInf);

					l_setConstants.insert(l_FunDepInf.m_setConst.begin(), l_FunDepInf.m_setConst.end());
					l_setConstants.insert(l_FunDepInf.m_setBackwordConst.begin(), l_FunDepInf.m_setBackwordConst.end());
					l_setObservers1.insert(l_FunDepInf.m_setObservers.begin(), l_FunDepInf.m_setObservers.end());


					l_mDep2Type[l_FunDepInf.m_sFunName] = wxT("Function");

					for (auto it_Con = l_setConstants.begin(); it_Con != l_setConstants.end(); ++it_Con)
					{
						l_mDep2Type[*it_Con] = wxT("Constant");
					}

					for (auto it_Con = l_setObservers1.begin(); it_Con != l_setObservers1.end(); ++it_Con)
					{
						l_mDep2Type[*it_Con] = wxT("Observer");
					}
					m_mDeclarations2Dependencies[l_FunDepInf.m_sFunName] = l_mDep2Type;


				}
			}
			//observers


			set<wxString> l_setUnusedObservers2;
			set<wxString> l_setConstants2;
			set<wxString> l_setFun2;


			map<wxString, set<wxString>> l_mapAlldefObservers = l_UnDec.ObtainObserversConstantsDependent();
			for (auto it_map = l_mapAlldefObservers.begin(); it_map != l_mapAlldefObservers.end(); ++it_map)
			{
				map<wxString, wxString> l_mDep2Type;
				ObserverDependencyInfo l_ObserverInf = l_UnDec.FindObserverDependency(it_map->first);
				if (l_ObserverInf.m_bIsUsed == false)
				{
					l_iUnusedObserversCount++;
					l_setUnusedObservers2.insert(l_ObserverInf.m_observerName);
					m_vObserversInfo.push_back(l_ObserverInf);

					l_setConstants2.insert(l_ObserverInf.m_setConstants.begin(), l_ObserverInf.m_setConstants.end());

					//
					l_setFun2.insert(l_ObserverInf.m_setFuns.begin(), l_ObserverInf.m_setFuns.end());
					l_setFun2.insert(l_ObserverInf.m_setForwardFuns.begin(), l_ObserverInf.m_setForwardFuns.end());

					l_mDep2Type[l_ObserverInf.m_observerName] = wxT("Observer");

					for (auto it_Con = l_setFun2.begin(); it_Con != l_setFun2.end(); ++it_Con)
					{
						l_mDep2Type[*it_Con] = wxT("Function");
					}

					for (auto it_Con = l_setConstants2.begin(); it_Con != l_setConstants2.end(); ++it_Con)
					{
						l_mDep2Type[*it_Con] = wxT("Constant");
					}
					m_mDeclarations2Dependencies[l_ObserverInf.m_observerName] = l_mDep2Type;


				}



			}





			m_vUnUsedDecList.erase(m_vUnUsedDecList.begin(), m_vUnUsedDecList.end());
			m_vUnUsedConstantsList.clear();
			m_setUnUsedFunctionsList.clear();
			m_setUnUsedObserverList.clear();
			m_vUnUsedConstantsList = l_setAllselectedConst;
			m_setUnUsedFunctionsList = l_setUnusedFuns;
			m_setUnUsedObserverList = l_setUnusedObservers2;
			m_vUnUsedDecList.insert(m_vUnUsedConstantsList.begin(), m_vUnUsedConstantsList.end());//merging
			m_vUnUsedDecList.insert(m_setUnUsedFunctionsList.begin(), m_setUnUsedFunctionsList.end());//merging
			m_vUnUsedDecList.insert(m_setUnUsedObserverList.begin(), m_setUnUsedObserverList.end());//merging


			if (m_mDeclarations2Dependencies.size() == 0)
			{
				wxString s_warningMsg = wxT("no unused declarations");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else {
				int l_iTotal = l_iUnusedObserversCount + l_iUnusedFunctionsCount + l_iUnuusedConstantsCount;
				wxString l_sReport = wxT("the total number of unused declarations: ");
				l_sReport << l_iTotal << wxT("\n");
				l_sReport << wxT("Details:");
				l_sReport << wxT("\n");
				l_sReport << wxT("Constants: ") << l_iUnuusedConstantsCount << wxT("\n");
				l_sReport << wxT("Functions: ") << l_iUnusedFunctionsCount << wxT("\n");
				l_sReport << wxT("Observers: ") << l_iUnusedObserversCount << wxT("\n");
				SP_LOGMESSAGE(l_sReport);
			}

		}


	}

	else {
		//colored pn
		wxString l_sDec = m_sSelected;

		//for final report
		int l_iUnusedSimpleCs = 0;
		int l_iUnusedCompCs = 0;
		int l_iUnusedAlias = 0;
		int l_iUnusedFuns = 0;
		int l_iUnusedconstants = 0;
		int l_iUnusedVariables = 0;

		if (m_sSelected == wxT("Variables"))
		{
			CheckAllDeclarations();
			m_vVariablesInfo.clear();



			set<wxString> l_setUnusedVar;
			wxString l_sgroupConst = wxT("Constants");
			wxString l_sgroupFun = wxT("Functions");
			wxString l_sgroupSimple = wxT("Simple Color Sets");
			wxString l_sgroupCompound = wxT("Compound Color Sets");

			m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupConst);
			m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupFun);
			m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroupSimple);
			m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroupCompound);
			m_vtrimeddecVariables = l_UnDec.TrimUserDec(m_sSelected);
			UpdateSetODeclarations(m_vtrimeddecConstants);
			UpdateSetODeclarations(m_vtrimeddecVariables);
			UpdateSetODeclarations(m_vtrimeddecSimplCS);
			UpdateSetODeclarations(m_vtrimeddecCompoundCS);
			UpdateSetODeclarations(m_vtrimeddecFunctions);

			set<wxString> l_setFun;
			set<wxString> l_setCon;
			set<wxString> l_setSimpl;
			set<wxString> l_setComp;
			bool l_bIsUnused = false;

			map<wxString, wxString> l_mAlldefinedVar = l_UnDec.ObtainAllVar2Cs();
			for (auto itmap = l_mAlldefinedVar.begin(); itmap != l_mAlldefinedVar.end(); ++itmap)
			{
				map<wxString, wxString> l_mDep2Type;
				VariableDependencyInfo l_varDependencyInfo = l_UnDec.FindVariableDependencies(itmap->first);
				if (l_varDependencyInfo.m_bIsUSed == false)
				{
					l_iUnusedVariables++;
					l_bIsUnused = true;
					m_vVariablesInfo.push_back(l_varDependencyInfo);
					l_setUnusedVar.insert(l_varDependencyInfo.m_sName);
					l_setFun.insert(l_varDependencyInfo.m_setFun.begin(), l_varDependencyInfo.m_setFun.end());
					l_setSimpl.insert(l_varDependencyInfo.m_setSimpleCs.begin(), l_varDependencyInfo.m_setSimpleCs.end());
					l_setCon.insert(l_varDependencyInfo.m_setConst.begin(), l_varDependencyInfo.m_setConst.end());
					l_setComp.insert(l_varDependencyInfo.m_CompoundCS.begin(), l_varDependencyInfo.m_CompoundCS.end());
					l_mDep2Type[l_varDependencyInfo.m_sName] = wxT("Variable");
					for (auto it_setFun = l_varDependencyInfo.m_setFun.begin(); it_setFun != l_varDependencyInfo.m_setFun.end(); ++it_setFun)
					{
						l_mDep2Type[*it_setFun] = wxT("Function");
					}
					for (auto it_Simp = l_varDependencyInfo.m_setSimpleCs.begin(); it_Simp != l_varDependencyInfo.m_setSimpleCs.end(); ++it_Simp)
					{
						l_mDep2Type[*it_Simp] = wxT("Simple");
					}
					for (auto it_Con = l_varDependencyInfo.m_setConst.begin(); it_Con != l_varDependencyInfo.m_setConst.end(); ++it_Con)
					{
						l_mDep2Type[*it_Con] = wxT("Constant");
					}
					for (auto it_Compound = l_varDependencyInfo.m_CompoundCS.begin(); it_Compound != l_varDependencyInfo.m_CompoundCS.end(); ++it_Compound)
					{
						l_mDep2Type[*it_Compound] = wxT("Compound");
					}
					m_mDeclarations2Dependencies[l_varDependencyInfo.m_sName] = l_mDep2Type;

				}
			}


			wxString l_sgroup = wxT("Variables");
			//m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroup);
			m_mGroup2SelectedSet[l_sgroupConst] = SetSelectItems(m_vtrimeddecConstants, l_setCon);
			m_mGroup2SelectedSet[l_sgroup] = SetSelectItems(m_vtrimeddecVariables, l_setUnusedVar);
			m_mGroup2SelectedSet[l_sgroupFun] = SetSelectItems(m_vtrimeddecFunctions, l_setFun);
			m_mGroup2SelectedSet[l_sgroupSimple] = SetSelectItems(m_vtrimeddecSimplCS, l_setSimpl);
			m_mGroup2SelectedSet[l_sgroupCompound] = SetSelectItems(m_vtrimeddecCompoundCS, l_setComp);

			m_vUnUsedConstantsList.erase(m_vUnUsedConstantsList.begin(), m_vUnUsedConstantsList.end());
			m_vUnUsedConstantsList.insert(l_setCon.begin(), l_setCon.end());
			m_setUnUsedFunctionsList.erase(m_setUnUsedFunctionsList.begin(), m_setUnUsedFunctionsList.end());
			m_setUnUsedFunctionsList.insert(l_setFun.begin(), l_setFun.end());
			m_setUnUsedVariablesList.erase(m_setUnUsedVariablesList.begin(), m_setUnUsedVariablesList.end());
			m_setUnUsedVariablesList.insert(l_setUnusedVar.begin(), l_setUnusedVar.end());
			m_setUnUsedSimpleCS.erase(m_setUnUsedSimpleCS.begin(), m_setUnUsedSimpleCS.end());
			m_setUnUsedSimpleCS.insert(l_setSimpl.begin(), l_setSimpl.end());
			m_setUnUsedCompoundCS.erase(m_setUnUsedCompoundCS.begin(), m_setUnUsedCompoundCS.end());
			m_setUnUsedCompoundCS.insert(l_setComp.begin(), l_setComp.end());


			if (!l_bIsUnused)
			{
				wxString s_warningMsg = wxT("no unused declarations (Variables)");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else {
				wxString l_sReport = wxT("the number of unused Variables is: ");
				l_sReport << l_iUnusedVariables << wxT("\n");
				SP_LOGMESSAGE(l_sReport);
			}

		}


		if (l_sDec == wxT("Constants"))
		{
			CheckAllDeclarations();
			m_vConstantsInfo.clear();
			set<wxString> l_setAllselectedConst;


			wxString l_sgroupVar = wxT("Variables");
			wxString l_sgroupFun = wxT("Functions");
			wxString l_sgroupSimple = wxT("Simple Color Sets");
			wxString l_sgroupCompound = wxT("Compound Color Sets");

			m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroupVar);
			m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupFun);
			m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroupSimple);
			m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroupCompound);

			UpdateSetODeclarations(m_vtrimeddecConstants);
			UpdateSetODeclarations(m_vtrimeddecVariables);
			UpdateSetODeclarations(m_vtrimeddecSimplCS);
			UpdateSetODeclarations(m_vtrimeddecCompoundCS);
			UpdateSetODeclarations(m_vtrimeddecFunctions);

			set<wxString> l_setFun;
			set<wxString> l_setVar;
			set<wxString> l_setSimpl;
			set<wxString> l_setComp;
			bool l_bIsUnused = false;

			set<wxString> l_setAllconstant = l_UnDec.ObtainAllDefinedConstants();
			for (auto it_ConstantSet = l_setAllconstant.begin(); it_ConstantSet != l_setAllconstant.end(); ++it_ConstantSet)
			{
				map<wxString, wxString> l_mDep2Type;
				ConstantDependencyInfo l_Cinfo = l_UnDec.FindConstantsDependencies(*it_ConstantSet);

				if (l_Cinfo.m_bIsUsed == false) {

					l_iUnusedconstants++;
					m_vConstantsInfo.push_back(l_Cinfo);
					l_setAllselectedConst.insert(l_Cinfo.m_sConstName);
					l_bIsUnused = true;
					l_setFun.insert(l_Cinfo.m_setFun.begin(), l_Cinfo.m_setFun.end());
					l_setVar.insert(l_Cinfo.m_setVar.begin(), l_Cinfo.m_setVar.end());
					l_setSimpl.insert(l_Cinfo.m_SetSimpleCS.begin(), l_Cinfo.m_SetSimpleCS.end());
					l_setComp.insert(l_Cinfo.m_SetCompoundCS.begin(), l_Cinfo.m_SetCompoundCS.end());

					l_mDep2Type[l_Cinfo.m_sConstName] = wxT("Constant");
					for (auto it_setFun = l_Cinfo.m_setFun.begin(); it_setFun != l_Cinfo.m_setFun.end(); ++it_setFun)
					{
						l_mDep2Type[*it_setFun] = wxT("Function");
					}
					for (auto it_Simp = l_Cinfo.m_SetSimpleCS.begin(); it_Simp != l_Cinfo.m_SetSimpleCS.end(); ++it_Simp)
					{
						l_mDep2Type[*it_Simp] = wxT("Simple");
					}

					for (auto it_Compound = l_Cinfo.m_SetCompoundCS.begin(); it_Compound != l_Cinfo.m_SetCompoundCS.end(); ++it_Compound)
					{
						l_mDep2Type[*it_Compound] = wxT("Compound");
					}
					for (auto it_Var = l_Cinfo.m_setVar.begin(); it_Var != l_Cinfo.m_setVar.end(); ++it_Var)
					{
						l_mDep2Type[*it_Var] = wxT("Variable");
					}
					m_mDeclarations2Dependencies[l_Cinfo.m_sConstName] = l_mDep2Type;
				}


			}
			wxString l_sgroup = wxT("Constants");
			m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroup);
			m_mGroup2SelectedSet[l_sgroup] = SetSelectItems(m_vtrimeddecConstants, l_setAllselectedConst);
			m_mGroup2SelectedSet[l_sgroupVar] = SetSelectItems(m_vtrimeddecVariables, l_setVar);
			m_mGroup2SelectedSet[l_sgroupFun] = SetSelectItems(m_vtrimeddecFunctions, l_setFun);
			m_mGroup2SelectedSet[l_sgroupSimple] = SetSelectItems(m_vtrimeddecSimplCS, l_setSimpl);
			m_mGroup2SelectedSet[l_sgroupCompound] = SetSelectItems(m_vtrimeddecCompoundCS, l_setComp);

			m_vUnUsedConstantsList.erase(m_vUnUsedConstantsList.begin(), m_vUnUsedConstantsList.end());
			m_vUnUsedConstantsList.insert(l_setAllselectedConst.begin(), l_setAllselectedConst.end());
			m_setUnUsedFunctionsList.erase(m_setUnUsedFunctionsList.begin(), m_setUnUsedFunctionsList.end());
			m_setUnUsedFunctionsList.insert(l_setFun.begin(), l_setFun.end());
			m_setUnUsedVariablesList.erase(m_setUnUsedVariablesList.begin(), m_setUnUsedVariablesList.end());
			m_setUnUsedVariablesList.insert(l_setVar.begin(), l_setVar.end());
			m_setUnUsedSimpleCS.erase(m_setUnUsedSimpleCS.begin(), m_setUnUsedSimpleCS.end());
			m_setUnUsedSimpleCS.insert(l_setSimpl.begin(), l_setSimpl.end());
			m_setUnUsedCompoundCS.erase(m_setUnUsedCompoundCS.begin(), m_setUnUsedCompoundCS.end());
			m_setUnUsedCompoundCS.insert(l_setComp.begin(), l_setComp.end());

			if (!l_bIsUnused)
			{
				wxString s_warningMsg = wxT("no unused declarations (Constants)");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else {
				wxString l_sReport = wxT("the number of unused Constants is: ");
				l_sReport << l_iUnusedconstants << wxT("\n");
				SP_LOGMESSAGE(l_sReport);
			}
		}
		else if (l_sDec == wxT("Functions"))
		{
			CheckAllDeclarations();
			m_vFunctionsInfo.clear();
			set<wxString> l_setUnusedFuns;
			wxString l_sgroupVar = wxT("Variables");
			wxString l_sgroupConstants = wxT("Constants");
			wxString l_sgroupSimple = wxT("Simple Color Sets");
			wxString l_sgroupCompound = wxT("Compound Color Sets");
			wxString l_sgroupFun = wxT("Functions");


			m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroupVar);
			m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupConstants);
			m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroupSimple);
			m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroupCompound);
			m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupFun);

			UpdateSetODeclarations(m_vtrimeddecConstants);
			UpdateSetODeclarations(m_vtrimeddecVariables);
			UpdateSetODeclarations(m_vtrimeddecSimplCS);
			UpdateSetODeclarations(m_vtrimeddecCompoundCS);
			UpdateSetODeclarations(m_vtrimeddecFunctions);


			set<wxString> l_setConstants;
			set<wxString> l_setVar;
			set<wxString> l_setSimpl;
			set<wxString> l_setComp;
			bool l_bIsUnused = false;
			set<wxString> l_setAlldefFuns = l_UnDec.ReadAllDefinedFunctions();
			for (auto it_set = l_setAlldefFuns.begin(); it_set != l_setAlldefFuns.end(); ++it_set)
			{
				map<wxString, wxString> l_mDep2Type;
				FunctionDependencyInfo l_FunDepInf = l_UnDec.FindFunctionDependencies(*it_set);
				if (l_FunDepInf.m_bIsUSed == false)
				{

					l_iUnusedFuns++;
					l_bIsUnused = true;
					l_setUnusedFuns.insert(l_FunDepInf.m_sFunName);
					m_vFunctionsInfo.push_back(l_FunDepInf);

					l_setConstants.insert(l_FunDepInf.m_setConst.begin(), l_FunDepInf.m_setConst.end());
					l_setVar.insert(l_FunDepInf.m_setVars.begin(), l_FunDepInf.m_setVars.end());
					l_setSimpl.insert(l_FunDepInf.m_setSimpleCs.begin(), l_FunDepInf.m_setSimpleCs.end());
					l_setComp.insert(l_FunDepInf.m_setCompoundCs.begin(), l_FunDepInf.m_setCompoundCs.end());

					l_mDep2Type[l_FunDepInf.m_sFunName] = wxT("Function");

					for (auto it_var = l_FunDepInf.m_setVars.begin(); it_var != l_FunDepInf.m_setVars.end(); ++it_var)
					{
						l_mDep2Type[*it_var] = wxT("Variable");
					}


					for (auto it_Simp = l_FunDepInf.m_setSimpleCs.begin(); it_Simp != l_FunDepInf.m_setSimpleCs.end(); ++it_Simp)
					{
						l_mDep2Type[*it_Simp] = wxT("Simple");
					}

					for (auto it_Compound = l_FunDepInf.m_setCompoundCs.begin(); it_Compound != l_FunDepInf.m_setCompoundCs.end(); ++it_Compound)
					{
						l_mDep2Type[*it_Compound] = wxT("Compound");
					}
					m_mDeclarations2Dependencies[l_FunDepInf.m_sFunName] = l_mDep2Type;


				}
			}



			m_mGroup2SelectedSet[l_sgroupConstants] = SetSelectItems(m_vtrimeddecConstants, l_setConstants);
			m_mGroup2SelectedSet[l_sgroupVar] = SetSelectItems(m_vtrimeddecVariables, l_setVar);
			m_mGroup2SelectedSet[l_sgroupFun] = SetSelectItems(m_vtrimeddecFunctions, l_setUnusedFuns);
			m_mGroup2SelectedSet[l_sgroupSimple] = SetSelectItems(m_vtrimeddecSimplCS, l_setSimpl);
			m_mGroup2SelectedSet[l_sgroupCompound] = SetSelectItems(m_vtrimeddecCompoundCS, l_setComp);

			m_vUnUsedConstantsList.erase(m_vUnUsedConstantsList.begin(), m_vUnUsedConstantsList.end());
			m_vUnUsedConstantsList.insert(l_setConstants.begin(), l_setConstants.end());
			m_setUnUsedFunctionsList.erase(m_setUnUsedFunctionsList.begin(), m_setUnUsedFunctionsList.end());
			m_setUnUsedFunctionsList.insert(l_setUnusedFuns.begin(), l_setUnusedFuns.end());
			m_setUnUsedVariablesList.erase(m_setUnUsedVariablesList.begin(), m_setUnUsedVariablesList.end());
			m_setUnUsedVariablesList.insert(l_setVar.begin(), l_setVar.end());
			m_setUnUsedSimpleCS.erase(m_setUnUsedSimpleCS.begin(), m_setUnUsedSimpleCS.end());
			m_setUnUsedSimpleCS.insert(l_setSimpl.begin(), l_setSimpl.end());
			m_setUnUsedCompoundCS.erase(m_setUnUsedCompoundCS.begin(), m_setUnUsedCompoundCS.end());
			m_setUnUsedCompoundCS.insert(l_setComp.begin(), l_setComp.end());


			if (!l_bIsUnused)
			{
				wxString s_warningMsg = wxT("no unused declarations (Functions)");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else {
				wxString l_sReport = wxT("the number of unused Functions is: ");
				l_sReport << l_iUnusedFuns << wxT("\n");
				SP_LOGMESSAGE(l_sReport);
			}

		}
		else if (l_sDec == wxT("Simple Color Sets"))
		{
			CheckAllDeclarations();
			m_vSimpleCsInfo.clear();


			set<wxString> l_setUnusedSimpleCs;
			wxString l_sgroupVar = wxT("Variables");
			wxString l_sgroupConstants = wxT("Constants");
			wxString l_sgroupSimple = wxT("Simple Color Sets");
			wxString l_sgroupCompound = wxT("Compound Color Sets");
			wxString l_sgroupFun = wxT("Functions");

			m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroupVar);
			m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupConstants);
			m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroupSimple);
			m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroupCompound);
			m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupFun);

			UpdateSetODeclarations(m_vtrimeddecConstants);
			UpdateSetODeclarations(m_vtrimeddecVariables);
			UpdateSetODeclarations(m_vtrimeddecSimplCS);
			UpdateSetODeclarations(m_vtrimeddecCompoundCS);
			UpdateSetODeclarations(m_vtrimeddecFunctions);

			set<wxString> l_setConstants;
			set<wxString> l_setVar;
			set<wxString> l_setFun;
			set<wxString> l_setComp;
			bool l_bIsUnused = false;
			map<wxString, wxString> l_mCs2Type = l_UnDec.ObtainAllCs2Type();
			for (auto it_map = l_mCs2Type.begin(); it_map != l_mCs2Type.end(); ++it_map)
			{
				map<wxString, wxString> l_mDep2Type;
				if (it_map->second == wxT("Simple"))
				{
					SimpleCsDependencyInfo l_SimplecsInfo = l_UnDec.FindSimpleCSDependencies(it_map->first);
					if (l_SimplecsInfo.m_bIsUSed == false && it_map->first != wxT("Dot"))
					{
						l_iUnusedSimpleCs++;

						l_bIsUnused = true;
						l_setUnusedSimpleCs.insert(l_SimplecsInfo.m_sName);
						l_setUnusedSimpleCs.insert(l_SimplecsInfo.m_setIndirectCs.begin(), l_SimplecsInfo.m_setIndirectCs.end());
						m_vSimpleCsInfo.push_back(l_SimplecsInfo);
						l_setConstants.insert(l_SimplecsInfo.m_setConst.begin(), l_SimplecsInfo.m_setConst.end());
						l_setVar.insert(l_SimplecsInfo.m_setVar.begin(), l_SimplecsInfo.m_setVar.end());
						l_setFun.insert(l_SimplecsInfo.m_setFun.begin(), l_SimplecsInfo.m_setFun.end());
						l_setComp.insert(l_SimplecsInfo.m_setCompCs.begin(), l_SimplecsInfo.m_setCompCs.end());

						l_mDep2Type[l_SimplecsInfo.m_sName] = wxT("Simple");
						for (auto it_setFun = l_SimplecsInfo.m_setFun.begin(); it_setFun != l_SimplecsInfo.m_setFun.end(); ++it_setFun)
						{
							l_mDep2Type[*it_setFun] = wxT("Function");
						}
						for (auto it_Simp = l_SimplecsInfo.m_setVar.begin(); it_Simp != l_SimplecsInfo.m_setVar.end(); ++it_Simp)
						{
							l_mDep2Type[*it_Simp] = wxT("Variable");
						}
						for (auto it_Con = l_SimplecsInfo.m_setConst.begin(); it_Con != l_SimplecsInfo.m_setConst.end(); ++it_Con)
						{
							l_mDep2Type[*it_Con] = wxT("Constant");
						}
						for (auto it_Compound = l_SimplecsInfo.m_setCompCs.begin(); it_Compound != l_SimplecsInfo.m_setCompCs.end(); ++it_Compound)
						{
							l_mDep2Type[*it_Compound] = wxT("Compound");
						}
						m_mDeclarations2Dependencies[l_SimplecsInfo.m_sName] = l_mDep2Type;

					}

				}
			}//end for

			m_mGroup2SelectedSet[l_sgroupConstants] = SetSelectItems(m_vtrimeddecConstants, l_setConstants);
			m_mGroup2SelectedSet[l_sgroupVar] = SetSelectItems(m_vtrimeddecVariables, l_setVar);
			m_mGroup2SelectedSet[l_sgroupFun] = SetSelectItems(m_vtrimeddecFunctions, l_setFun);
			m_mGroup2SelectedSet[l_sgroupSimple] = SetSelectItems(m_vtrimeddecSimplCS, l_setUnusedSimpleCs);
			m_mGroup2SelectedSet[l_sgroupCompound] = SetSelectItems(m_vtrimeddecCompoundCS, l_setComp);

			//reset selected list and assign new unused items
			m_vUnUsedConstantsList.erase(m_vUnUsedConstantsList.begin(), m_vUnUsedConstantsList.end());
			m_vUnUsedConstantsList.insert(l_setConstants.begin(), l_setConstants.end());
			m_setUnUsedFunctionsList.erase(m_setUnUsedFunctionsList.begin(), m_setUnUsedFunctionsList.end());
			m_setUnUsedFunctionsList.insert(l_setFun.begin(), l_setFun.end());
			m_setUnUsedVariablesList.erase(m_setUnUsedVariablesList.begin(), m_setUnUsedVariablesList.end());
			m_setUnUsedVariablesList.insert(l_setVar.begin(), l_setVar.end());
			m_setUnUsedSimpleCS.erase(m_setUnUsedSimpleCS.begin(), m_setUnUsedSimpleCS.end());
			m_setUnUsedSimpleCS.insert(l_setUnusedSimpleCs.begin(), l_setUnusedSimpleCs.end());
			m_setUnUsedCompoundCS.erase(m_setUnUsedCompoundCS.begin(), m_setUnUsedCompoundCS.end());
			m_setUnUsedCompoundCS.insert(l_setComp.begin(), l_setComp.end());


			if (!l_bIsUnused)
			{
				wxString s_warningMsg = wxT("no unused declarations (Simple Color Sets)");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else {
				wxString l_sReport = wxT("the number of unused Simple Color Sets is: ");
				l_sReport << l_iUnusedSimpleCs << wxT("\n");
				SP_LOGMESSAGE(l_sReport);
			}

		}
		else if (l_sDec == wxT("Compound Color Sets"))
		{
			CheckAllDeclarations();
			m_vCompoundCsInfo.clear();
			m_vSubCsInfo.clear();

			set<wxString> l_setUnusedStructuredCs;
			wxString l_sgroupVar = wxT("Variables");
			wxString l_sgroupConstants = wxT("Constants");
			wxString l_sgroupSimple = wxT("Simple Color Sets");
			wxString l_sgroupCompound = wxT("Compound Color Sets");
			wxString l_sgroupFun = wxT("Functions");

			m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroupVar);
			m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupConstants);
			m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroupSimple);
			m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroupCompound);
			m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupFun);

			UpdateSetODeclarations(m_vtrimeddecConstants);
			UpdateSetODeclarations(m_vtrimeddecVariables);
			UpdateSetODeclarations(m_vtrimeddecSimplCS);
			UpdateSetODeclarations(m_vtrimeddecCompoundCS);
			UpdateSetODeclarations(m_vtrimeddecFunctions);

			set<wxString> l_setConstants;
			set<wxString> l_setVar;
			set<wxString> l_setFun;
			set<wxString> l_setSimplcs;
			bool l_bIsUnused = false;
			map<wxString, wxString> l_mCs2Type = l_UnDec.ObtainAllCs2Type();
			for (auto it_map = l_mCs2Type.begin(); it_map != l_mCs2Type.end(); ++it_map)
			{
				map<wxString, wxString> l_mDep2Type;
				if (it_map->second == wxT("Compound"))
				{
					CompoundCsDependencyInfo l_CompoundcsInfo = l_UnDec.FindCompoundCSDependencies(it_map->first);
					if (l_CompoundcsInfo.m_bIsUSed == false)
					{
						l_iUnusedCompCs++;
						l_bIsUnused = true;
						m_vCompoundCsInfo.push_back(l_CompoundcsInfo);
						l_setUnusedStructuredCs.insert(l_CompoundcsInfo.m_sName);
						l_setUnusedStructuredCs.insert(l_CompoundcsInfo.m_setIndirectCompCs.begin(), l_CompoundcsInfo.m_setIndirectCompCs.end());
						l_setConstants.insert(l_CompoundcsInfo.m_setConst.begin(), l_CompoundcsInfo.m_setConst.end());
						l_setVar.insert(l_CompoundcsInfo.m_setVar.begin(), l_CompoundcsInfo.m_setVar.end());
						l_setFun.insert(l_CompoundcsInfo.m_setFun.begin(), l_CompoundcsInfo.m_setFun.end());
						l_setSimplcs.insert(l_CompoundcsInfo.m_setSimpleCs.begin(), l_CompoundcsInfo.m_setSimpleCs.end());



						l_mDep2Type[l_CompoundcsInfo.m_sName] = wxT("Compound");
						for (auto it_setFun = l_CompoundcsInfo.m_setFun.begin(); it_setFun != l_CompoundcsInfo.m_setFun.end(); ++it_setFun)
						{
							l_mDep2Type[*it_setFun] = wxT("Function");
						}
						for (auto it_Simp = l_CompoundcsInfo.m_setVar.begin(); it_Simp != l_CompoundcsInfo.m_setVar.end(); ++it_Simp)
						{
							l_mDep2Type[*it_Simp] = wxT("Variable");
						}
						for (auto it_Con = l_CompoundcsInfo.m_setConst.begin(); it_Con != l_CompoundcsInfo.m_setConst.end(); ++it_Con)
						{
							l_mDep2Type[*it_Con] = wxT("Constant");
						}
						for (auto it_Compound = l_CompoundcsInfo.m_setSimpleCs.begin(); it_Compound != l_CompoundcsInfo.m_setSimpleCs.end(); ++it_Compound)
						{
							l_mDep2Type[*it_Compound] = wxT("Simple");
						}
						m_mDeclarations2Dependencies[l_CompoundcsInfo.m_sName] = l_mDep2Type;

					}


				}
				else {
					if (!it_map->second.Contains(wxT("Simple")) && !it_map->second.Contains(wxT("Alias")))
					{
						SubSetCsDependencyInfo l_subInfo = l_UnDec.FindSubCSDependency(it_map->first);
						if (l_subInfo.m_bIsUSed == false)
						{
							l_iUnusedCompCs++;
							l_bIsUnused = true;
							m_vSubCsInfo.push_back(l_subInfo);
							l_setUnusedStructuredCs.insert(l_subInfo.m_sName);
							l_setConstants.insert(l_subInfo.m_setConst.begin(), l_subInfo.m_setConst.end());
							l_setVar.insert(l_subInfo.m_setVar.begin(), l_subInfo.m_setVar.end());
							l_setFun.insert(l_subInfo.m_setFun.begin(), l_subInfo.m_setFun.end());
							l_setSimplcs.insert(l_subInfo.m_setSimpleCs.begin(), l_subInfo.m_setSimpleCs.end());


							l_mDep2Type[l_subInfo.m_sName] = wxT("Compound");
							for (auto it_setFun = l_subInfo.m_setFun.begin(); it_setFun != l_subInfo.m_setFun.end(); ++it_setFun)
							{
								l_mDep2Type[*it_setFun] = wxT("Function");
							}
							for (auto it_Simp = l_subInfo.m_setVar.begin(); it_Simp != l_subInfo.m_setVar.end(); ++it_Simp)
							{
								l_mDep2Type[*it_Simp] = wxT("Variable");
							}
							for (auto it_Con = l_subInfo.m_setConst.begin(); it_Con != l_subInfo.m_setConst.end(); ++it_Con)
							{
								l_mDep2Type[*it_Con] = wxT("Constant");
							}
							for (auto it_Compound = l_subInfo.m_setSimpleCs.begin(); it_Compound != l_subInfo.m_setSimpleCs.end(); ++it_Compound)
							{
								l_mDep2Type[*it_Compound] = wxT("Simple");
							}
							m_mDeclarations2Dependencies[l_subInfo.m_sName] = l_mDep2Type;

						}

					}
				}
			}

			m_mGroup2SelectedSet[l_sgroupConstants] = SetSelectItems(m_vtrimeddecConstants, l_setConstants);
			m_mGroup2SelectedSet[l_sgroupVar] = SetSelectItems(m_vtrimeddecVariables, l_setVar);
			m_mGroup2SelectedSet[l_sgroupFun] = SetSelectItems(m_vtrimeddecFunctions, l_setFun);
			m_mGroup2SelectedSet[l_sgroupSimple] = SetSelectItems(m_vtrimeddecSimplCS, l_setSimplcs);
			m_mGroup2SelectedSet[l_sgroupCompound] = SetSelectItems(m_vtrimeddecCompoundCS, l_setUnusedStructuredCs);

			//reset selected list and assign new unused items
			m_vUnUsedConstantsList.erase(m_vUnUsedConstantsList.begin(), m_vUnUsedConstantsList.end());
			m_vUnUsedConstantsList.insert(l_setConstants.begin(), l_setConstants.end());
			m_setUnUsedFunctionsList.erase(m_setUnUsedFunctionsList.begin(), m_setUnUsedFunctionsList.end());
			m_setUnUsedFunctionsList.insert(l_setFun.begin(), l_setFun.end());
			m_setUnUsedVariablesList.erase(m_setUnUsedVariablesList.begin(), m_setUnUsedVariablesList.end());
			m_setUnUsedVariablesList.insert(l_setVar.begin(), l_setVar.end());
			m_setUnUsedSimpleCS.erase(m_setUnUsedSimpleCS.begin(), m_setUnUsedSimpleCS.end());
			m_setUnUsedSimpleCS.insert(l_setSimplcs.begin(), l_setSimplcs.end());
			m_setUnUsedCompoundCS.erase(m_setUnUsedCompoundCS.begin(), m_setUnUsedCompoundCS.end());
			m_setUnUsedCompoundCS.insert(l_setUnusedStructuredCs.begin(), l_setUnusedStructuredCs.end());

			if (!l_bIsUnused)
			{
				wxString s_warningMsg = wxT("no unused declarations (Compound Color Sets)");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else {
				wxString l_sReport = wxT("the number of unused Compound Color Sets is: ");
				l_sReport << l_iUnusedCompCs << wxT("\n");
				SP_LOGMESSAGE(l_sReport);
			}
		}
		else if (l_sDec == wxT("Alias Color Sets"))
		{
			CheckAllDeclarations();
			bool l_bIsUnused = false;
			m_vAliasCsInfo.clear();
			map<wxString, wxString> l_mCs2Type = l_UnDec.ObtainAllCs2Type();

			set<wxString> l_setConstants;
			set<wxString> l_setVar;
			set<wxString> l_setFun;
			set<wxString> l_setSimplcs;
			set<wxString> l_setCompcs;
			set<wxString> l_setAlias;
			for (auto it_m = l_mCs2Type.begin(); it_m != l_mCs2Type.end(); ++it_m)
			{
				map<wxString, wxString> l_mDep2Type;
				if (it_m->second == wxT("Alias"))
				{
					AliasCsDependencyInfo l_aliasInf = l_UnDec.FindAliasCSDependency(it_m->first);
					if (l_aliasInf.m_bIsUSed == false)
					{
						l_iUnusedAlias++;
						l_bIsUnused = true;
						m_vAliasCsInfo.push_back(l_aliasInf);
						l_setAlias.insert(l_aliasInf.m_sName);
						l_setConstants.insert(l_aliasInf.m_setConst.begin(), l_aliasInf.m_setConst.end());
						l_setVar.insert(l_aliasInf.m_setVar.begin(), l_aliasInf.m_setVar.end());
						l_setFun.insert(l_aliasInf.m_setFun.begin(), l_aliasInf.m_setFun.end());
						l_setSimplcs.insert(l_aliasInf.m_setSimpleCs.begin(), l_aliasInf.m_setSimpleCs.end());
						l_setCompcs.insert(l_aliasInf.m_setCompCs.begin(), l_aliasInf.m_setCompCs.end());



						l_mDep2Type[l_aliasInf.m_sName] = wxT("Alias");
						for (auto it_setFun = l_aliasInf.m_setFun.begin(); it_setFun != l_aliasInf.m_setFun.end(); ++it_setFun)
						{
							l_mDep2Type[*it_setFun] = wxT("Function");
						}
						for (auto it_Simp = l_aliasInf.m_setVar.begin(); it_Simp != l_aliasInf.m_setVar.end(); ++it_Simp)
						{
							l_mDep2Type[*it_Simp] = wxT("Variable");
						}
						for (auto it_Con = l_aliasInf.m_setConst.begin(); it_Con != l_aliasInf.m_setConst.end(); ++it_Con)
						{
							l_mDep2Type[*it_Con] = wxT("Constant");
						}
						for (auto it_Compound = l_aliasInf.m_setSimpleCs.begin(); it_Compound != l_aliasInf.m_setSimpleCs.end(); ++it_Compound)
						{
							l_mDep2Type[*it_Compound] = wxT("Simple");
						}

						for (auto it_Compound = l_aliasInf.m_setCompCs.begin(); it_Compound != l_aliasInf.m_setCompCs.end(); ++it_Compound)
						{
							l_mDep2Type[*it_Compound] = wxT("Compound");
						}
						m_mDeclarations2Dependencies[l_aliasInf.m_sName] = l_mDep2Type;


					}

				}
			}

			//reset selected list and assign new unused items
			m_vUnUsedConstantsList.erase(m_vUnUsedConstantsList.begin(), m_vUnUsedConstantsList.end());
			m_vUnUsedConstantsList.insert(l_setConstants.begin(), l_setConstants.end());
			m_setUnUsedFunctionsList.erase(m_setUnUsedFunctionsList.begin(), m_setUnUsedFunctionsList.end());
			m_setUnUsedFunctionsList.insert(l_setFun.begin(), l_setFun.end());
			m_setUnUsedVariablesList.erase(m_setUnUsedVariablesList.begin(), m_setUnUsedVariablesList.end());
			m_setUnUsedVariablesList.insert(l_setVar.begin(), l_setVar.end());
			m_setUnUsedSimpleCS.erase(m_setUnUsedSimpleCS.begin(), m_setUnUsedSimpleCS.end());
			m_setUnUsedSimpleCS.insert(l_setSimplcs.begin(), l_setSimplcs.end());
			m_setUnUsedCompoundCS.erase(m_setUnUsedCompoundCS.begin(), m_setUnUsedCompoundCS.end());
			m_setUnUsedCompoundCS.insert(l_setCompcs.begin(), l_setCompcs.end());
			m_setUnUsedAliasCS.clear();
			m_setUnUsedAliasCS.insert(l_setAlias.begin(), l_setAlias.end());

			if (!l_bIsUnused)
			{
				wxString s_warningMsg = wxT("no unused declarations (Alias Color Sets)");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else {
				wxString l_sReport = wxT("the number of unused Alias Color Sets is: ");
				l_sReport << l_iUnusedAlias << wxT("\n");
				SP_LOGMESSAGE(l_sReport);
			}
		}
		else if (l_sDec == wxT("Color Sets"))
		{
			CheckAllDeclarations();
			bool l_bIsUnused = false;
			wxString l_sGroupSimple = wxT("Simple Color Sets");
			wxString l_sGroupCompound = wxT("Compound Color Sets");
			wxString l_sGroupAlias = wxT("Alias Color Sets");
			m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sGroupSimple);
			m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sGroupCompound);
			m_vtrimeddecAliasCS = l_UnDec.TrimAliasColorSets(l_sGroupAlias);
			m_vtrimeddecALLCs.insert(m_vtrimeddecSimplCS.begin(), m_vtrimeddecSimplCS.end());
			m_vtrimeddecALLCs.insert(m_vtrimeddecCompoundCS.begin(), m_vtrimeddecCompoundCS.end());
			m_vtrimeddecALLCs.insert(m_vtrimeddecAliasCS.begin(), m_vtrimeddecAliasCS.end());

			m_setUnUsedColorSetsList.erase(m_setUnUsedColorSetsList.begin(), m_setUnUsedColorSetsList.end());//reset all color sets

																											 //compute for simple cs
			m_vSimpleCsInfo.clear();


			set<wxString> l_setUnusedSimpleCs;
			wxString l_sgroupVar = wxT("Variables");
			wxString l_sgroupConstants = wxT("Constants");
			wxString l_sgroupSimple = wxT("Simple Color Sets");
			wxString l_sgroupCompound = wxT("Compound Color Sets");
			wxString l_sgroupFun = wxT("Functions");

			m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroupVar);
			m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupConstants);
			m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroupSimple);
			m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroupCompound);
			m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupFun);

			UpdateSetODeclarations(m_vtrimeddecConstants);
			UpdateSetODeclarations(m_vtrimeddecVariables);
			UpdateSetODeclarations(m_vtrimeddecSimplCS);
			UpdateSetODeclarations(m_vtrimeddecCompoundCS);
			UpdateSetODeclarations(m_vtrimeddecAliasCS);
			UpdateSetODeclarations(m_vtrimeddecFunctions);


			set<wxString> l_setConstants;
			set<wxString> l_setVar;
			set<wxString> l_setFun;
			set<wxString> l_setComp;

			map<wxString, wxString> l_mCs2Type = l_UnDec.ObtainAllCs2Type();
			for (auto it_map = l_mCs2Type.begin(); it_map != l_mCs2Type.end(); ++it_map)
			{
				map<wxString, wxString> l_mDep2Type;
				if (it_map->second == wxT("Simple"))
				{
					SimpleCsDependencyInfo l_SimplecsInfo = l_UnDec.FindSimpleCSDependencies(it_map->first);
					if (l_SimplecsInfo.m_bIsUSed == false && it_map->first != wxT("Dot"))
					{
						l_iUnusedSimpleCs++;
						l_bIsUnused = true;
						l_setUnusedSimpleCs.insert(l_SimplecsInfo.m_sName);
						l_setUnusedSimpleCs.insert(l_SimplecsInfo.m_setIndirectCs.begin(), l_SimplecsInfo.m_setIndirectCs.end());
						m_vSimpleCsInfo.push_back(l_SimplecsInfo);
						l_setConstants.insert(l_SimplecsInfo.m_setConst.begin(), l_SimplecsInfo.m_setConst.end());
						l_setVar.insert(l_SimplecsInfo.m_setVar.begin(), l_SimplecsInfo.m_setVar.end());
						l_setFun.insert(l_SimplecsInfo.m_setFun.begin(), l_SimplecsInfo.m_setFun.end());
						l_setComp.insert(l_SimplecsInfo.m_setCompCs.begin(), l_SimplecsInfo.m_setCompCs.end());

						l_mDep2Type[l_SimplecsInfo.m_sName] = wxT("Simple");
						for (auto it_setFun = l_SimplecsInfo.m_setFun.begin(); it_setFun != l_SimplecsInfo.m_setFun.end(); ++it_setFun)
						{
							l_mDep2Type[*it_setFun] = wxT("Function");
						}
						for (auto it_Simp = l_SimplecsInfo.m_setVar.begin(); it_Simp != l_SimplecsInfo.m_setVar.end(); ++it_Simp)
						{
							l_mDep2Type[*it_Simp] = wxT("Variable");
						}
						for (auto it_Con = l_SimplecsInfo.m_setConst.begin(); it_Con != l_SimplecsInfo.m_setConst.end(); ++it_Con)
						{
							l_mDep2Type[*it_Con] = wxT("Constant");
						}
						for (auto it_Compound = l_SimplecsInfo.m_setCompCs.begin(); it_Compound != l_SimplecsInfo.m_setCompCs.end(); ++it_Compound)
						{
							l_mDep2Type[*it_Compound] = wxT("Compound");
						}
						m_mDeclarations2Dependencies[l_SimplecsInfo.m_sName] = l_mDep2Type;
					}

				}
			}//end for
			 /*compute for compound*/
			m_vCompoundCsInfo.clear();
			m_vSubCsInfo.clear();
			set<wxString> l_setSimplcs;
			set<wxString> l_setUnusedStructuredCs;
			//map<wxString, wxString> l_mCs2Type = l_UnDec.ObtainAllCs2Type();
			for (auto it_map = l_mCs2Type.begin(); it_map != l_mCs2Type.end(); ++it_map)
			{
				map<wxString, wxString> l_mDep2Type;
				if (it_map->second == wxT("Compound"))
				{
					CompoundCsDependencyInfo l_CompoundcsInfo = l_UnDec.FindCompoundCSDependencies(it_map->first);
					if (l_CompoundcsInfo.m_bIsUSed == false)
					{
						l_iUnusedCompCs++;
						l_bIsUnused = true;
						m_vCompoundCsInfo.push_back(l_CompoundcsInfo);
						l_setUnusedStructuredCs.insert(l_CompoundcsInfo.m_sName);
						l_setUnusedStructuredCs.insert(l_CompoundcsInfo.m_setIndirectCompCs.begin(), l_CompoundcsInfo.m_setIndirectCompCs.end());
						l_setConstants.insert(l_CompoundcsInfo.m_setConst.begin(), l_CompoundcsInfo.m_setConst.end());
						l_setVar.insert(l_CompoundcsInfo.m_setVar.begin(), l_CompoundcsInfo.m_setVar.end());
						l_setFun.insert(l_CompoundcsInfo.m_setFun.begin(), l_CompoundcsInfo.m_setFun.end());
						l_setSimplcs.insert(l_CompoundcsInfo.m_setSimpleCs.begin(), l_CompoundcsInfo.m_setSimpleCs.end());


						l_mDep2Type[l_CompoundcsInfo.m_sName] = wxT("Compound");
						for (auto it_setFun = l_CompoundcsInfo.m_setFun.begin(); it_setFun != l_CompoundcsInfo.m_setFun.end(); ++it_setFun)
						{
							l_mDep2Type[*it_setFun] = wxT("Function");
						}
						for (auto it_Simp = l_CompoundcsInfo.m_setVar.begin(); it_Simp != l_CompoundcsInfo.m_setVar.end(); ++it_Simp)
						{
							l_mDep2Type[*it_Simp] = wxT("Variable");
						}
						for (auto it_Con = l_CompoundcsInfo.m_setConst.begin(); it_Con != l_CompoundcsInfo.m_setConst.end(); ++it_Con)
						{
							l_mDep2Type[*it_Con] = wxT("Constant");
						}
						for (auto it_Compound = l_CompoundcsInfo.m_setSimpleCs.begin(); it_Compound != l_CompoundcsInfo.m_setSimpleCs.end(); ++it_Compound)
						{
							l_mDep2Type[*it_Compound] = wxT("Simple");
						}
						m_mDeclarations2Dependencies[l_CompoundcsInfo.m_sName] = l_mDep2Type;
					}


				}
				else {
					if (!it_map->second.Contains(wxT("Simple")) && !it_map->second.Contains(wxT("Alias")))
					{
						SubSetCsDependencyInfo l_subInfo = l_UnDec.FindSubCSDependency(it_map->first);
						if (l_subInfo.m_bIsUSed == false)
						{
							l_iUnusedCompCs++;
							l_bIsUnused = true;
							m_vSubCsInfo.push_back(l_subInfo);
							l_setUnusedStructuredCs.insert(l_subInfo.m_sName);
							l_setConstants.insert(l_subInfo.m_setConst.begin(), l_subInfo.m_setConst.end());
							l_setVar.insert(l_subInfo.m_setVar.begin(), l_subInfo.m_setVar.end());
							l_setFun.insert(l_subInfo.m_setFun.begin(), l_subInfo.m_setFun.end());
							l_setSimplcs.insert(l_subInfo.m_setSimpleCs.begin(), l_subInfo.m_setSimpleCs.end());

							l_mDep2Type[l_subInfo.m_sName] = wxT("Compound");
							for (auto it_setFun = l_subInfo.m_setFun.begin(); it_setFun != l_subInfo.m_setFun.end(); ++it_setFun)
							{
								l_mDep2Type[*it_setFun] = wxT("Function");
							}
							for (auto it_Simp = l_subInfo.m_setVar.begin(); it_Simp != l_subInfo.m_setVar.end(); ++it_Simp)
							{
								l_mDep2Type[*it_Simp] = wxT("Variable");
							}
							for (auto it_Con = l_subInfo.m_setConst.begin(); it_Con != l_subInfo.m_setConst.end(); ++it_Con)
							{
								l_mDep2Type[*it_Con] = wxT("Constant");
							}
							for (auto it_Compound = l_subInfo.m_setSimpleCs.begin(); it_Compound != l_subInfo.m_setSimpleCs.end(); ++it_Compound)
							{
								l_mDep2Type[*it_Compound] = wxT("Simple");
							}
							m_mDeclarations2Dependencies[l_subInfo.m_sName] = l_mDep2Type;
						}

					}
				}
			}//end for
			 /********************/
			m_vAliasCsInfo.clear();
			map<wxString, wxString> l_mCs2Type1 = l_UnDec.ObtainAllCs2Type();

			set<wxString> l_setConstantsforAlias;
			set<wxString> l_setVarforAlias;
			set<wxString> l_setFunforAlias;
			set<wxString> l_setSimplcsforAlias;
			set<wxString> l_setCompcsforAlias;
			set<wxString> l_setAlias;
			for (auto it_m = l_mCs2Type1.begin(); it_m != l_mCs2Type1.end(); ++it_m)
			{
				map<wxString, wxString> l_mDep2Type;
				if (it_m->second == wxT("Alias"))
				{
					AliasCsDependencyInfo l_aliasInf = l_UnDec.FindAliasCSDependency(it_m->first);
					if (l_aliasInf.m_bIsUSed == false)
					{
						l_iUnusedAlias++;
						l_bIsUnused = true;
						m_vAliasCsInfo.push_back(l_aliasInf);
						l_setAlias.insert(l_aliasInf.m_sName);
						l_setConstantsforAlias.insert(l_aliasInf.m_setConst.begin(), l_aliasInf.m_setConst.end());
						l_setVarforAlias.insert(l_aliasInf.m_setVar.begin(), l_aliasInf.m_setVar.end());
						l_setFunforAlias.insert(l_aliasInf.m_setFun.begin(), l_aliasInf.m_setFun.end());
						l_setSimplcsforAlias.insert(l_aliasInf.m_setSimpleCs.begin(), l_aliasInf.m_setSimpleCs.end());
						l_setCompcsforAlias.insert(l_aliasInf.m_setCompCs.begin(), l_aliasInf.m_setCompCs.end());



						l_mDep2Type[l_aliasInf.m_sName] = wxT("Alias");
						for (auto it_setFun = l_aliasInf.m_setFun.begin(); it_setFun != l_aliasInf.m_setFun.end(); ++it_setFun)
						{
							l_mDep2Type[*it_setFun] = wxT("Function");
						}
						for (auto it_Simp = l_aliasInf.m_setVar.begin(); it_Simp != l_aliasInf.m_setVar.end(); ++it_Simp)
						{
							l_mDep2Type[*it_Simp] = wxT("Variable");
						}
						for (auto it_Con = l_aliasInf.m_setConst.begin(); it_Con != l_aliasInf.m_setConst.end(); ++it_Con)
						{
							l_mDep2Type[*it_Con] = wxT("Constant");
						}
						for (auto it_Compound = l_aliasInf.m_setSimpleCs.begin(); it_Compound != l_aliasInf.m_setSimpleCs.end(); ++it_Compound)
						{
							l_mDep2Type[*it_Compound] = wxT("Simple");
						}

						for (auto it_Compound = l_aliasInf.m_setCompCs.begin(); it_Compound != l_aliasInf.m_setCompCs.end(); ++it_Compound)
						{
							l_mDep2Type[*it_Compound] = wxT("Compound");
						}
						m_mDeclarations2Dependencies[l_aliasInf.m_sName] = l_mDep2Type;


					}

				}
			}


			/*******************/
			//merge unused simple cs and merge compound
			l_setSimplcs.insert(l_setUnusedSimpleCs.begin(), l_setUnusedSimpleCs.end());
			l_setSimplcs.insert(l_setSimplcsforAlias.begin(), l_setSimplcsforAlias.end());
			l_setUnusedStructuredCs.insert(l_setComp.begin(), l_setComp.end());
			l_setUnusedStructuredCs.insert(l_setCompcsforAlias.begin(), l_setCompcsforAlias.end());


			//reset selected list and assign curren items of all dec
			m_vUnUsedConstantsList.erase(m_vUnUsedConstantsList.begin(), m_vUnUsedConstantsList.end());
			m_vUnUsedConstantsList.insert(l_setConstants.begin(), l_setConstants.end());
			m_vUnUsedConstantsList.insert(l_setConstantsforAlias.begin(), l_setConstantsforAlias.end());

			m_setUnUsedFunctionsList.erase(m_setUnUsedFunctionsList.begin(), m_setUnUsedFunctionsList.end());
			m_setUnUsedFunctionsList.insert(l_setFun.begin(), l_setFun.end());
			m_setUnUsedFunctionsList.insert(l_setFunforAlias.begin(), l_setFunforAlias.end());

			m_setUnUsedVariablesList.erase(m_setUnUsedVariablesList.begin(), m_setUnUsedVariablesList.end());
			m_setUnUsedVariablesList.insert(l_setVar.begin(), l_setVar.end());
			m_setUnUsedVariablesList.insert(l_setVarforAlias.begin(), l_setVarforAlias.end());

			m_setUnUsedSimpleCS.erase(m_setUnUsedSimpleCS.begin(), m_setUnUsedSimpleCS.end());
			m_setUnUsedSimpleCS.insert(l_setSimplcs.begin(), l_setSimplcs.end());
			m_setUnUsedCompoundCS.erase(m_setUnUsedCompoundCS.begin(), m_setUnUsedCompoundCS.end());
			m_setUnUsedCompoundCS.insert(l_setUnusedStructuredCs.begin(), l_setUnusedStructuredCs.end());



			if (!l_bIsUnused)
			{
				wxString s_warningMsg = wxT("no unused declarations (Color Sets)");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else {
				wxString l_sReport = wxT("the number of unused Color Sets is: ");
				l_sReport << wxT("Simple Color Sets: ") << l_iUnusedSimpleCs << wxT("\n");
				l_sReport << wxT("Compound Color Sets: ") << l_iUnusedCompCs << wxT("\n");
				l_sReport << wxT("Alias Color Sets: ") << l_iUnusedAlias << wxT("\n");
				SP_LOGMESSAGE(l_sReport);
			}
		}

		else if (l_sDec == wxT("Declarations"))
		{
			CheckAllDeclarations();
			set<wxString> l_setAllVariables;
			set<wxString> l_setAllFuns;
			set<wxString> l_setAllConst;
			set<wxString> l_setAllSimpleCs;
			set<wxString> l_setAllCompoundCs;

			/*compute color sets*/
			wxString l_sGroupSimple = wxT("Simple Color Sets");
			wxString l_sGroupCompound = wxT("Compound Color Sets");
			m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sGroupSimple);
			m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sGroupCompound);

			m_vtrimeddecALLCs.insert(m_vtrimeddecSimplCS.begin(), m_vtrimeddecSimplCS.end());
			m_vtrimeddecALLCs.insert(m_vtrimeddecCompoundCS.begin(), m_vtrimeddecCompoundCS.end());

			m_setUnUsedColorSetsList.erase(m_setUnUsedColorSetsList.begin(), m_setUnUsedColorSetsList.end());//reset all color sets

																											 //compute for simple cs
			m_vSimpleCsInfo.clear();


			set<wxString> l_setUnusedSimpleCs;
			wxString l_sgroupVar = wxT("Variables");
			wxString l_sgroupConstants = wxT("Constants");
			wxString l_sgroupSimple = wxT("Simple Color Sets");
			wxString l_sgroupCompound = wxT("Compound Color Sets");
			wxString l_sgroupFun = wxT("Functions");

			m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroupVar);
			m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupConstants);
			m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroupSimple);
			m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroupCompound);
			m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupFun);

			UpdateSetODeclarations(m_vtrimeddecConstants);
			UpdateSetODeclarations(m_vtrimeddecVariables);
			UpdateSetODeclarations(m_vtrimeddecSimplCS);
			UpdateSetODeclarations(m_vtrimeddecCompoundCS);
			UpdateSetODeclarations(m_vtrimeddecFunctions);

			set<wxString> l_setConstants;
			set<wxString> l_setVar;
			set<wxString> l_setFun;
			set<wxString> l_setComp;

			map<wxString, wxString> l_mCs2Type = l_UnDec.ObtainAllCs2Type();
			for (auto it_map = l_mCs2Type.begin(); it_map != l_mCs2Type.end(); ++it_map)
			{
				map<wxString, wxString> l_mDep2Type;
				if (it_map->second == wxT("Simple"))
				{
					SimpleCsDependencyInfo l_SimplecsInfo = l_UnDec.FindSimpleCSDependencies(it_map->first);
					if (l_SimplecsInfo.m_bIsUSed == false && it_map->first != wxT("Dot"))
					{
						l_iUnusedSimpleCs++;
						l_setUnusedSimpleCs.insert(l_SimplecsInfo.m_sName);
						l_setUnusedSimpleCs.insert(l_SimplecsInfo.m_setIndirectCs.begin(), l_SimplecsInfo.m_setIndirectCs.end());
						m_vSimpleCsInfo.push_back(l_SimplecsInfo);
						l_setConstants.insert(l_SimplecsInfo.m_setConst.begin(), l_SimplecsInfo.m_setConst.end());
						l_setVar.insert(l_SimplecsInfo.m_setVar.begin(), l_SimplecsInfo.m_setVar.end());
						l_setFun.insert(l_SimplecsInfo.m_setFun.begin(), l_SimplecsInfo.m_setFun.end());
						l_setComp.insert(l_SimplecsInfo.m_setCompCs.begin(), l_SimplecsInfo.m_setCompCs.end());


						l_mDep2Type[l_SimplecsInfo.m_sName] = wxT("Simple");
						for (auto it_setFun = l_SimplecsInfo.m_setFun.begin(); it_setFun != l_SimplecsInfo.m_setFun.end(); ++it_setFun)
						{
							l_mDep2Type[*it_setFun] = wxT("Function");
						}
						for (auto it_Simp = l_SimplecsInfo.m_setVar.begin(); it_Simp != l_SimplecsInfo.m_setVar.end(); ++it_Simp)
						{
							l_mDep2Type[*it_Simp] = wxT("Variable");
						}
						for (auto it_Con = l_SimplecsInfo.m_setConst.begin(); it_Con != l_SimplecsInfo.m_setConst.end(); ++it_Con)
						{
							l_mDep2Type[*it_Con] = wxT("Constant");
						}
						for (auto it_Compound = l_SimplecsInfo.m_setCompCs.begin(); it_Compound != l_SimplecsInfo.m_setCompCs.end(); ++it_Compound)
						{
							l_mDep2Type[*it_Compound] = wxT("Compound");
						}
						m_mDeclarations2Dependencies[l_SimplecsInfo.m_sName] = l_mDep2Type;
					}

				}
			}//end for
			 /*compute for compound*/
			m_vCompoundCsInfo.clear();
			m_vSubCsInfo.clear();
			set<wxString> l_setSimplcs;
			set<wxString> l_setUnusedStructuredCs;
			//map<wxString, wxString> l_mCs2Type = l_UnDec.ObtainAllCs2Type();
			for (auto it_map = l_mCs2Type.begin(); it_map != l_mCs2Type.end(); ++it_map)
			{
				map<wxString, wxString> l_mDep2Type;
				if (it_map->second == wxT("Compound"))
				{
					CompoundCsDependencyInfo l_CompoundcsInfo = l_UnDec.FindCompoundCSDependencies(it_map->first);
					if (l_CompoundcsInfo.m_bIsUSed == false)
					{
						l_iUnusedCompCs++;
						m_vCompoundCsInfo.push_back(l_CompoundcsInfo);
						l_setUnusedStructuredCs.insert(l_CompoundcsInfo.m_sName);
						l_setUnusedStructuredCs.insert(l_CompoundcsInfo.m_setIndirectCompCs.begin(), l_CompoundcsInfo.m_setIndirectCompCs.end());
						l_setConstants.insert(l_CompoundcsInfo.m_setConst.begin(), l_CompoundcsInfo.m_setConst.end());
						l_setVar.insert(l_CompoundcsInfo.m_setVar.begin(), l_CompoundcsInfo.m_setVar.end());
						l_setFun.insert(l_CompoundcsInfo.m_setFun.begin(), l_CompoundcsInfo.m_setFun.end());
						l_setSimplcs.insert(l_CompoundcsInfo.m_setSimpleCs.begin(), l_CompoundcsInfo.m_setSimpleCs.end());

						l_mDep2Type[l_CompoundcsInfo.m_sName] = wxT("Compound");
						for (auto it_setFun = l_CompoundcsInfo.m_setFun.begin(); it_setFun != l_CompoundcsInfo.m_setFun.end(); ++it_setFun)
						{
							l_mDep2Type[*it_setFun] = wxT("Function");
						}
						for (auto it_Simp = l_CompoundcsInfo.m_setVar.begin(); it_Simp != l_CompoundcsInfo.m_setVar.end(); ++it_Simp)
						{
							l_mDep2Type[*it_Simp] = wxT("Variable");
						}
						for (auto it_Con = l_CompoundcsInfo.m_setConst.begin(); it_Con != l_CompoundcsInfo.m_setConst.end(); ++it_Con)
						{
							l_mDep2Type[*it_Con] = wxT("Constant");
						}
						for (auto it_Compound = l_CompoundcsInfo.m_setSimpleCs.begin(); it_Compound != l_CompoundcsInfo.m_setSimpleCs.end(); ++it_Compound)
						{
							l_mDep2Type[*it_Compound] = wxT("Simple");
						}
						m_mDeclarations2Dependencies[l_CompoundcsInfo.m_sName] = l_mDep2Type;

					}


				}
				else {
					if (!it_map->second.Contains(wxT("Simple")) && !it_map->second.Contains(wxT("Alias")))
					{
						SubSetCsDependencyInfo l_subInfo = l_UnDec.FindSubCSDependency(it_map->first);
						if (l_subInfo.m_bIsUSed == false)
						{
							l_iUnusedCompCs++;
							m_vSubCsInfo.push_back(l_subInfo);
							l_setUnusedStructuredCs.insert(l_subInfo.m_sName);
							l_setConstants.insert(l_subInfo.m_setConst.begin(), l_subInfo.m_setConst.end());
							l_setVar.insert(l_subInfo.m_setVar.begin(), l_subInfo.m_setVar.end());
							l_setFun.insert(l_subInfo.m_setFun.begin(), l_subInfo.m_setFun.end());
							l_setSimplcs.insert(l_subInfo.m_setSimpleCs.begin(), l_subInfo.m_setSimpleCs.end());

							l_mDep2Type[l_subInfo.m_sName] = wxT("Compound");
							for (auto it_setFun = l_subInfo.m_setFun.begin(); it_setFun != l_subInfo.m_setFun.end(); ++it_setFun)
							{
								l_mDep2Type[*it_setFun] = wxT("Function");
							}
							for (auto it_Simp = l_subInfo.m_setVar.begin(); it_Simp != l_subInfo.m_setVar.end(); ++it_Simp)
							{
								l_mDep2Type[*it_Simp] = wxT("Variable");
							}
							for (auto it_Con = l_subInfo.m_setConst.begin(); it_Con != l_subInfo.m_setConst.end(); ++it_Con)
							{
								l_mDep2Type[*it_Con] = wxT("Constant");
							}
							for (auto it_Compound = l_subInfo.m_setSimpleCs.begin(); it_Compound != l_subInfo.m_setSimpleCs.end(); ++it_Compound)
							{
								l_mDep2Type[*it_Compound] = wxT("Simple");
							}
							m_mDeclarations2Dependencies[l_subInfo.m_sName] = l_mDep2Type;
						}

					}
				}
			}//end for

			 /*******************/
			m_vAliasCsInfo.clear();
			map<wxString, wxString> l_mCs2Type1;
			l_mCs2Type1 = m_unUsedDec.ObtainAllCs2Type();

			set<wxString> l_setConstantsForAlias;
			set<wxString> l_setVarForAlias;
			set<wxString> l_setFunForAlias;
			set<wxString> l_setSimplcsForAlias;
			set<wxString> l_setCompcsForAlias;
			set<wxString> l_setAlias;
			for (auto it_m = l_mCs2Type1.begin(); it_m != l_mCs2Type1.end(); ++it_m)
			{
				map<wxString, wxString> l_mDep2Type;
				if (it_m->second == wxT("Alias"))
				{
					AliasCsDependencyInfo l_aliasInf = m_unUsedDec.FindAliasCSDependency(it_m->first);
					if (l_aliasInf.m_bIsUSed == false)
					{

						l_iUnusedAlias++;
						m_vAliasCsInfo.push_back(l_aliasInf);
						l_setAlias.insert(l_aliasInf.m_sName);
						l_setConstantsForAlias.insert(l_aliasInf.m_setConst.begin(), l_aliasInf.m_setConst.end());
						l_setVarForAlias.insert(l_aliasInf.m_setVar.begin(), l_aliasInf.m_setVar.end());
						l_setFunForAlias.insert(l_aliasInf.m_setFun.begin(), l_aliasInf.m_setFun.end());
						l_setSimplcsForAlias.insert(l_aliasInf.m_setSimpleCs.begin(), l_aliasInf.m_setSimpleCs.end());
						l_setCompcsForAlias.insert(l_aliasInf.m_setCompCs.begin(), l_aliasInf.m_setCompCs.end());



						l_mDep2Type[l_aliasInf.m_sName] = wxT("Alias");
						for (auto it_setFun = l_aliasInf.m_setFun.begin(); it_setFun != l_aliasInf.m_setFun.end(); ++it_setFun)
						{
							l_mDep2Type[*it_setFun] = wxT("Function");
						}
						for (auto it_Simp = l_aliasInf.m_setVar.begin(); it_Simp != l_aliasInf.m_setVar.end(); ++it_Simp)
						{
							l_mDep2Type[*it_Simp] = wxT("Variable");
						}
						for (auto it_Con = l_aliasInf.m_setConst.begin(); it_Con != l_aliasInf.m_setConst.end(); ++it_Con)
						{
							l_mDep2Type[*it_Con] = wxT("Constant");
						}
						for (auto it_Compound = l_aliasInf.m_setSimpleCs.begin(); it_Compound != l_aliasInf.m_setSimpleCs.end(); ++it_Compound)
						{
							l_mDep2Type[*it_Compound] = wxT("Simple");
						}

						for (auto it_Compound = l_aliasInf.m_setCompCs.begin(); it_Compound != l_aliasInf.m_setCompCs.end(); ++it_Compound)
						{
							l_mDep2Type[*it_Compound] = wxT("Compound");
						}
						m_mDeclarations2Dependencies[l_aliasInf.m_sName] = l_mDep2Type;


					}

				}
			}



			/********************/
			//merge unused simple cs and merge compound
			l_setSimplcs.insert(l_setUnusedSimpleCs.begin(), l_setUnusedSimpleCs.end());
			l_setUnusedStructuredCs.insert(l_setComp.begin(), l_setComp.end());

			l_setSimplcs.insert(l_setSimplcsForAlias.begin(), l_setSimplcsForAlias.end());
			l_setUnusedStructuredCs.insert(l_setCompcsForAlias.begin(), l_setCompcsForAlias.end());


			l_setAllSimpleCs.insert(m_setUnUsedSimpleCS.begin(), m_setUnUsedSimpleCS.end());
			l_setAllVariables.insert(l_setVar.begin(), l_setVar.end());
			l_setAllVariables.insert(l_setVarForAlias.begin(), l_setVarForAlias.end());
			l_setAllFuns.insert(l_setFun.begin(), l_setFun.end());
			l_setAllFuns.insert(l_setFunForAlias.begin(), l_setFunForAlias.end());
			l_setAllConst.insert(l_setConstants.begin(), l_setConstants.end());
			l_setAllConst.insert(l_setConstantsForAlias.begin(), l_setConstantsForAlias.end());
			l_setAllCompoundCs.insert(l_setUnusedStructuredCs.begin(), l_setUnusedStructuredCs.end());
			/*constants*/
			m_vConstantsInfo.clear();
			set<wxString> l_setAllselectedConst;






			set<wxString> l_setFun1;
			set<wxString> l_setVar1;
			set<wxString> l_setSimpl1;
			set<wxString> l_setComp1;


			set<wxString> l_setAllconstant = l_UnDec.ObtainAllDefinedConstants();
			for (auto it_ConstantSet = l_setAllconstant.begin(); it_ConstantSet != l_setAllconstant.end(); ++it_ConstantSet)
			{
				map<wxString, wxString> l_mDep2Type;
				ConstantDependencyInfo l_Cinfo = l_UnDec.FindConstantsDependencies(*it_ConstantSet);

				if (l_Cinfo.m_bIsUsed == false) {
					l_iUnusedconstants++;
					m_vConstantsInfo.push_back(l_Cinfo);
					l_setAllselectedConst.insert(l_Cinfo.m_sConstName);
					l_setFun1.insert(l_Cinfo.m_setFun.begin(), l_Cinfo.m_setFun.end());
					l_setVar1.insert(l_Cinfo.m_setVar.begin(), l_Cinfo.m_setVar.end());
					l_setSimpl1.insert(l_Cinfo.m_SetSimpleCS.begin(), l_Cinfo.m_SetSimpleCS.end());
					l_setComp1.insert(l_Cinfo.m_SetCompoundCS.begin(), l_Cinfo.m_SetCompoundCS.end());


					l_mDep2Type[l_Cinfo.m_sConstName] = wxT("Constant");
					for (auto it_setFun = l_Cinfo.m_setFun.begin(); it_setFun != l_Cinfo.m_setFun.end(); ++it_setFun)
					{
						l_mDep2Type[*it_setFun] = wxT("Function");
					}
					for (auto it_Simp = l_Cinfo.m_SetSimpleCS.begin(); it_Simp != l_Cinfo.m_SetSimpleCS.end(); ++it_Simp)
					{
						l_mDep2Type[*it_Simp] = wxT("Simple");
					}

					for (auto it_Compound = l_Cinfo.m_SetCompoundCS.begin(); it_Compound != l_Cinfo.m_SetCompoundCS.end(); ++it_Compound)
					{
						l_mDep2Type[*it_Compound] = wxT("Compound");
					}
					for (auto it_Var = l_Cinfo.m_setVar.begin(); it_Var != l_Cinfo.m_setVar.end(); ++it_Var)
					{
						l_mDep2Type[*it_Var] = wxT("Variable");
					}
					m_mDeclarations2Dependencies[l_Cinfo.m_sConstName] = l_mDep2Type;
				}


			}



			l_setAllSimpleCs.insert(l_setSimpl1.begin(), l_setSimpl1.end());
			l_setAllVariables.insert(l_setVar1.begin(), l_setVar1.end());
			l_setAllFuns.insert(l_setFun1.begin(), l_setFun1.end());
			l_setAllConst.insert(l_setAllselectedConst.begin(), l_setAllselectedConst.end());
			l_setAllCompoundCs.insert(l_setComp1.begin(), l_setComp1.end());
			/*functions*/
			m_vFunctionsInfo.clear();
			set<wxString> l_setUnusedFuns;

			set<wxString> l_setConstants2;
			set<wxString> l_setVar2;
			set<wxString> l_setSimpl2;
			set<wxString> l_setComp2;

			set<wxString> l_setAlldefFuns = l_UnDec.ReadAllDefinedFunctions();
			for (auto it_set = l_setAlldefFuns.begin(); it_set != l_setAlldefFuns.end(); ++it_set)
			{
				map<wxString, wxString> l_mDep2Type;
				FunctionDependencyInfo l_FunDepInf = l_UnDec.FindFunctionDependencies(*it_set);
				if (l_FunDepInf.m_bIsUSed == false)
				{
					l_iUnusedFuns++;
					l_setUnusedFuns.insert(l_FunDepInf.m_sFunName);
					m_vFunctionsInfo.push_back(l_FunDepInf);

					l_setConstants2.insert(l_FunDepInf.m_setConst.begin(), l_FunDepInf.m_setConst.end());
					l_setVar2.insert(l_FunDepInf.m_setVars.begin(), l_FunDepInf.m_setVars.end());
					l_setSimpl2.insert(l_FunDepInf.m_setSimpleCs.begin(), l_FunDepInf.m_setSimpleCs.end());
					l_setComp2.insert(l_FunDepInf.m_setCompoundCs.begin(), l_FunDepInf.m_setCompoundCs.end());

					l_mDep2Type[l_FunDepInf.m_sFunName] = wxT("Function");

					for (auto it_var = l_FunDepInf.m_setVars.begin(); it_var != l_FunDepInf.m_setVars.end(); ++it_var)
					{
						l_mDep2Type[*it_var] = wxT("Variable");
					}


					for (auto it_Simp = l_FunDepInf.m_setSimpleCs.begin(); it_Simp != l_FunDepInf.m_setSimpleCs.end(); ++it_Simp)
					{
						l_mDep2Type[*it_Simp] = wxT("Simple");
					}

					for (auto it_Compound = l_FunDepInf.m_setCompoundCs.begin(); it_Compound != l_FunDepInf.m_setCompoundCs.end(); ++it_Compound)
					{
						l_mDep2Type[*it_Compound] = wxT("Compound");
					}
					m_mDeclarations2Dependencies[l_FunDepInf.m_sFunName] = l_mDep2Type;
				}
			}

			l_setAllSimpleCs.insert(l_setSimpl2.begin(), l_setSimpl2.end());
			l_setAllVariables.insert(l_setVar2.begin(), l_setVar2.end());
			l_setAllFuns.insert(l_setUnusedFuns.begin(), l_setUnusedFuns.end());
			l_setAllConst.insert(l_setConstants2.begin(), l_setConstants2.end());
			l_setAllCompoundCs.insert(l_setComp2.begin(), l_setComp2.end());


			/*******var*****/
			m_vVariablesInfo.clear();



			set<wxString> l_setUnusedVar;


			set<wxString> l_setFunforvar;
			set<wxString> l_setConforvar;
			set<wxString> l_setSimplforvar;
			set<wxString> l_setCompforvar;


			map<wxString, wxString> l_mAlldefinedVar = l_UnDec.ObtainAllVar2Cs();
			for (auto itmap = l_mAlldefinedVar.begin(); itmap != l_mAlldefinedVar.end(); ++itmap)
			{
				map<wxString, wxString> l_mDep2Type;
				VariableDependencyInfo l_varDependencyInfo = l_UnDec.FindVariableDependencies(itmap->first);
				if (l_varDependencyInfo.m_bIsUSed == false)
				{
					l_iUnusedVariables++;
					m_vVariablesInfo.push_back(l_varDependencyInfo);
					l_setUnusedVar.insert(l_varDependencyInfo.m_sName);
					l_setFunforvar.insert(l_varDependencyInfo.m_setFun.begin(), l_varDependencyInfo.m_setFun.end());
					l_setSimplforvar.insert(l_varDependencyInfo.m_setSimpleCs.begin(), l_varDependencyInfo.m_setSimpleCs.end());
					l_setConforvar.insert(l_varDependencyInfo.m_setConst.begin(), l_varDependencyInfo.m_setConst.end());
					l_setCompforvar.insert(l_varDependencyInfo.m_CompoundCS.begin(), l_varDependencyInfo.m_CompoundCS.end());

					l_mDep2Type[l_varDependencyInfo.m_sName] = wxT("Variable");
					for (auto it_setFun = l_varDependencyInfo.m_setFun.begin(); it_setFun != l_varDependencyInfo.m_setFun.end(); ++it_setFun)
					{
						l_mDep2Type[*it_setFun] = wxT("Function");
					}
					for (auto it_Simp = l_varDependencyInfo.m_setSimpleCs.begin(); it_Simp != l_varDependencyInfo.m_setSimpleCs.end(); ++it_Simp)
					{
						l_mDep2Type[*it_Simp] = wxT("Simple");
					}
					for (auto it_Con = l_varDependencyInfo.m_setConst.begin(); it_Con != l_varDependencyInfo.m_setConst.end(); ++it_Con)
					{
						l_mDep2Type[*it_Con] = wxT("Constant");
					}
					for (auto it_Compound = l_varDependencyInfo.m_CompoundCS.begin(); it_Compound != l_varDependencyInfo.m_CompoundCS.end(); ++it_Compound)
					{
						l_mDep2Type[*it_Compound] = wxT("Compound");
					}
					m_mDeclarations2Dependencies[l_varDependencyInfo.m_sName] = l_mDep2Type;
				}
			}

			l_setAllSimpleCs.insert(l_setSimplforvar.begin(), l_setSimplforvar.end());
			l_setAllVariables.insert(l_setUnusedVar.begin(), l_setUnusedVar.end());
			l_setAllFuns.insert(l_setFunforvar.begin(), l_setFunforvar.end());
			l_setAllConst.insert(l_setConforvar.begin(), l_setConforvar.end());
			l_setAllCompoundCs.insert(l_setCompforvar.begin(), l_setCompforvar.end());


			/****************/
			m_mGroup2SelectedSet[l_sgroupConstants] = SetSelectItems(m_vtrimeddecConstants, l_setAllConst);
			m_mGroup2SelectedSet[l_sgroupVar] = SetSelectItems(m_vtrimeddecVariables, l_setAllVariables);
			m_mGroup2SelectedSet[l_sgroupFun] = SetSelectItems(m_vtrimeddecFunctions, l_setAllFuns);
			m_mGroup2SelectedSet[l_sgroupSimple] = SetSelectItems(m_vtrimeddecSimplCS, l_setAllSimpleCs);
			m_mGroup2SelectedSet[l_sgroupCompound] = SetSelectItems(m_vtrimeddecCompoundCS, l_setAllCompoundCs);
			//reset selected list and assign curren items of all dec
			m_vUnUsedConstantsList.erase(m_vUnUsedConstantsList.begin(), m_vUnUsedConstantsList.end());
			m_vUnUsedConstantsList.insert(l_setAllConst.begin(), l_setAllConst.end());
			m_setUnUsedFunctionsList.erase(m_setUnUsedFunctionsList.begin(), m_setUnUsedFunctionsList.end());
			m_setUnUsedFunctionsList.insert(l_setAllFuns.begin(), l_setAllFuns.end());
			m_setUnUsedVariablesList.erase(m_setUnUsedVariablesList.begin(), m_setUnUsedVariablesList.end());
			m_setUnUsedVariablesList.insert(l_setAllVariables.begin(), l_setAllVariables.end());
			m_setUnUsedSimpleCS.erase(m_setUnUsedSimpleCS.begin(), m_setUnUsedSimpleCS.end());
			m_setUnUsedSimpleCS.insert(l_setAllSimpleCs.begin(), l_setAllSimpleCs.end());
			m_setUnUsedCompoundCS.erase(m_setUnUsedCompoundCS.begin(), m_setUnUsedCompoundCS.end());
			m_setUnUsedCompoundCS.insert(l_setAllCompoundCs.begin(), l_setAllCompoundCs.end());


			if (m_mDeclarations2Dependencies.size() == 0)
			{
				wxString s_warningMsg = wxT("no unused declarations");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else {
				wxString l_sReport = wxT("the total number of unused Declarations : ");
				int l_iTotal = l_iUnusedSimpleCs + l_iUnusedCompCs + l_iUnusedAlias + l_iUnusedconstants + l_iUnusedFuns + l_iUnusedVariables;
				l_sReport << l_iTotal << wxT("\n");
				l_sReport << wxT("Details: ") << wxT("\n");
				l_sReport << wxT("Simple Color Sets: ") << l_iUnusedSimpleCs << wxT("\n");
				l_sReport << wxT("Compound Color Sets: ") << l_iUnusedCompCs << wxT("\n");
				l_sReport << wxT("Alias Color Sets: ") << l_iUnusedAlias << wxT("\n");
				l_sReport << wxT("Constants: ") << l_iUnusedconstants << wxT("\n");
				l_sReport << wxT("Functions: ") << l_iUnusedFuns << wxT("\n");
				l_sReport << wxT("Variables: ") << l_iUnusedVariables << wxT("\n");

				SP_LOGMESSAGE(l_sReport);
			}

		}



	}


	if (m_sSelected == wxT("Constants") && !l_bIsColoured) {
		l_vtrimeddec = m_vtrimeddecConstants;
		m_vUnUsedDecList = m_vUnUsedConstantsList;

		for (int i = 0; i < m_vConstantsInfo.size(); i++)
		{
			if (m_vConstantsInfo[i].m_bIsUsed == false)
			{
				wxString l_sgroup = wxT("Constants");
				m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
				{
					if (itmap->second.Trim() == m_vConstantsInfo[i].m_sConstName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId.IsOk()) {//!=NULL
							m_vUnUsedConstantsList.insert(l_sLabel);
							wxColour l_nColor1(wxT("RED"));
							pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
						}

					}
				}

				/*********************/
				if (m_vConstantsInfo[i].m_setForwardFun.size() > 0)
				{
					wxString l_sgroupfun = wxT("Functions");
					m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupfun);
					set<wxString> l_setFuns = m_vConstantsInfo[i].m_setForwardFun;
					for (auto it_setFuns = l_setFuns.begin(); it_setFuns != l_setFuns.end(); ++it_setFuns)
					{
						for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
						{
							if (itmap->second == *it_setFuns)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}
				/*********************/
				// functions group
				if (m_vConstantsInfo[i].m_setFun.size() > 0)
				{
					wxString l_sgroupfun = wxT("Functions");
					m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupfun);
					set<wxString> l_setFuns = m_vConstantsInfo[i].m_setFun;
					for (auto it_setFuns = l_setFuns.begin(); it_setFuns != l_setFuns.end(); ++it_setFuns)
					{
						for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
						{
							if (itmap->second == *it_setFuns)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}
				//observers
				if (m_vConstantsInfo[i].m_setObservers.size() > 0)
				{
					wxString l_sgroupObs = wxT("Observers");
					m_vtrimeddecObservers = l_UnDec.TrimUserDec(l_sgroupObs);
					set<wxString> l_setObs = m_vConstantsInfo[i].m_setObservers;
					for (auto it_setObs = l_setObs.begin(); it_setObs != l_setObs.end(); ++it_setObs)
					{
						for (auto itmap = m_vtrimeddecObservers.begin(); itmap != m_vtrimeddecObservers.end(); ++itmap)
						{
							if (itmap->second == *it_setObs)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}
			}
		}

	}
	else if (m_sSelected == wxT("Observers") && !l_bIsColoured)
	{

		for (int i = 0; i < m_vObserversInfo.size(); i++)
		{
			if (m_vObserversInfo[i].m_bIsUsed == false)
			{
				wxString l_sgroup = wxT("Observers");
				m_vtrimeddecObservers = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecObservers.begin(); itmap != m_vtrimeddecObservers.end(); ++itmap)
				{
					if (itmap->second.Trim() == m_vObserversInfo[i].m_observerName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId.IsOk()) {//!=NULL

							wxColour l_nColor1(wxT("RED"));
							pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
						}

					}
				}
				/***constants group**/
				if (m_vObserversInfo[i].m_setConstants.size() > 0)
				{
					wxString l_sgroupConst = wxT("Constants");
					m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupConst);
					set<wxString> l_setConst = m_vObserversInfo[i].m_setConstants;
					for (auto it_setCons = l_setConst.begin(); it_setCons != l_setConst.end(); ++it_setCons)
					{
						for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
						{
							if (itmap->second.Trim() == *it_setCons)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}
				//functions group
				if (m_vObserversInfo[i].m_setFuns.size() > 0)
				{
					wxString l_sgroupConst = wxT("Functions");
					m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupConst);
					set<wxString> l_setFun = m_vObserversInfo[i].m_setFuns;
					for (auto it_setFun = l_setFun.begin(); it_setFun != l_setFun.end(); ++it_setFun)
					{
						for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
						{
							if (itmap->second.Trim() == *it_setFun)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}
				/************/
				//functions group
				if (m_vObserversInfo[i].m_setForwardFuns.size() > 0)
				{
					wxString l_sgroup = wxT("Functions");
					m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroup);
					set<wxString> l_setFun = m_vObserversInfo[i].m_setForwardFuns;
					for (auto it_setFun = l_setFun.begin(); it_setFun != l_setFun.end(); ++it_setFun)
					{
						for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
						{
							if (itmap->second.Trim() == *it_setFun)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}

				/**********/
			}
		}
	}



	else if (m_sSelected == wxT("Functions") && !l_bIsColoured)
	{

		for (int i = 0; i < m_vFunctionsInfo.size(); i++)
		{
			if (m_vFunctionsInfo[i].m_bIsUSed == false)
			{
				wxString l_sgroup = wxT("Functions");
				m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
				{
					if (itmap->second == m_vFunctionsInfo[i].m_sFunName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId.IsOk()) {//!=NULL

							wxColour l_nColor1(wxT("RED"));
							pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
						}

					}
				}
				/***constants group**/
				if (m_vFunctionsInfo[i].m_setConst.size() > 0)
				{
					wxString l_sgroupConst = wxT("Constants");
					m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupConst);
					set<wxString> l_setConst = m_vFunctionsInfo[i].m_setConst;
					for (auto it_setCons = l_setConst.begin(); it_setCons != l_setConst.end(); ++it_setCons)
					{
						for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
						{
							if (itmap->second.Trim() == *it_setCons)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}
				/**************************/
				if (m_vFunctionsInfo[i].m_setBackwordConst.size() > 0)
				{
					wxString l_sgroupConst = wxT("Constants");
					m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupConst);
					set<wxString> l_setConst = m_vFunctionsInfo[i].m_setBackwordConst;
					for (auto it_setCons = l_setConst.begin(); it_setCons != l_setConst.end(); ++it_setCons)
					{
						for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
						{
							if (itmap->second.Trim() == *it_setCons)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}
				/*************************/

				if (m_vFunctionsInfo[i].m_setBackwordObservers.size() > 0)
				{
					wxString l_sgroupConst = wxT("Observers");
					m_vtrimeddecObservers = l_UnDec.TrimUserDec(l_sgroupConst);
					set<wxString> l_setOb = m_vFunctionsInfo[i].m_setBackwordObservers;
					for (auto it_setOb = l_setOb.begin(); it_setOb != l_setOb.end(); ++it_setOb)
					{
						for (auto itmap = m_vtrimeddecObservers.begin(); itmap != m_vtrimeddecObservers.end(); ++itmap)
						{
							if (itmap->second.Trim() == *it_setOb)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}
				/************************/
			}
		}



	}

	else if (m_sSelected == wxT("Declarations") && !l_bIsColoured) {

		/************************/


		for (int i = 0; i < m_vConstantsInfo.size(); i++)
		{
			if (m_vConstantsInfo[i].m_bIsUsed == false)
			{
				wxString l_sgroup = wxT("Constants");
				m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
				{
					if (itmap->second.Trim() == m_vConstantsInfo[i].m_sConstName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId.IsOk()) {//!=NULL
							m_vUnUsedConstantsList.insert(l_sLabel);
							wxColour l_nColor1(wxT("RED"));
							pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
						}

					}
				}
				// functions group
				if (m_vConstantsInfo[i].m_setFun.size() > 0)
				{
					wxString l_sgroupfun = wxT("Functions");
					m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupfun);
					set<wxString> l_setFuns = m_vConstantsInfo[i].m_setFun;
					for (auto it_setFuns = l_setFuns.begin(); it_setFuns != l_setFuns.end(); ++it_setFuns)
					{
						for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
						{
							if (itmap->second == *it_setFuns)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}
				//observers
				if (m_vConstantsInfo[i].m_setObservers.size() > 0)
				{
					wxString l_sgroupObs = wxT("Observers");
					m_vtrimeddecObservers = l_UnDec.TrimUserDec(l_sgroupObs);
					set<wxString> l_setObs = m_vConstantsInfo[i].m_setObservers;
					for (auto it_setObs = l_setObs.begin(); it_setObs != l_setObs.end(); ++it_setObs)
					{
						for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
						{
							if (itmap->second == *it_setObs)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}
			}
		}




		for (int i = 0; i < m_vObserversInfo.size(); i++)
		{
			if (m_vObserversInfo[i].m_bIsUsed == false)
			{
				wxString l_sgroup = wxT("Observers");
				m_vtrimeddecObservers = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecObservers.begin(); itmap != m_vtrimeddecObservers.end(); ++itmap)
				{
					if (itmap->second.Trim() == m_vObserversInfo[i].m_observerName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId.IsOk()) {//!=NULL

							wxColour l_nColor1(wxT("RED"));
							pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
						}

					}
				}
				/***constants group**/
				if (m_vObserversInfo[i].m_setConstants.size() > 0)
				{
					wxString l_sgroupConst = wxT("Constants");
					m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupConst);
					set<wxString> l_setConst = m_vObserversInfo[i].m_setConstants;
					for (auto it_setCons = l_setConst.begin(); it_setCons != l_setConst.end(); ++it_setCons)
					{
						for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
						{
							if (itmap->second.Trim() == *it_setCons)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}
				//functions group
				if (m_vObserversInfo[i].m_setFuns.size() > 0)
				{
					wxString l_sgroupConst = wxT("Functions");
					m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupConst);
					set<wxString> l_setFun = m_vObserversInfo[i].m_setFuns;
					for (auto it_setFun = l_setFun.begin(); it_setFun != l_setFun.end(); ++it_setFun)
					{
						for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
						{
							if (itmap->second.Trim() == *it_setFun)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}

			}
		}





		for (int i = 0; i < m_vFunctionsInfo.size(); i++)
		{
			if (m_vFunctionsInfo[i].m_bIsUSed == false)
			{
				wxString l_sgroup = wxT("Functions");
				m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
				{
					if (itmap->second == m_vFunctionsInfo[i].m_sFunName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId.IsOk()) {//!=NULL

							wxColour l_nColor1(wxT("RED"));
							pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
						}

					}
				}
				/***constants group**/
				if (m_vFunctionsInfo[i].m_setConst.size() > 0)
				{
					wxString l_sgroupConst = wxT("Constants");
					m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupConst);
					set<wxString> l_setConst = m_vFunctionsInfo[i].m_setConst;
					for (auto it_setCons = l_setConst.begin(); it_setCons != l_setConst.end(); ++it_setCons)
					{
						for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
						{
							if (itmap->second.Trim() == *it_setCons)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}

			}




			/*******************************/

		}
	}
	else if (m_sSelected == wxT("Variables")) {

		l_vtrimeddec = m_vtrimeddecVariables;
		m_vUnUsedDecList = m_setUnUsedVariablesList;

	}
	else if (m_sSelected == wxT("Simple color Sets"))
	{
		l_vtrimeddec = m_vtrimeddecSimplCS;
		m_vUnUsedDecList = m_setUnUsedSimpleCS;
	}
	else if (m_sSelected == wxT("Alias color Sets"))
	{
		l_vtrimeddec = m_vtrimeddecAliasCS;
		m_vUnUsedDecList = m_setUnUsedAliasCS;
	}
	else if (m_sSelected == wxT("Color Sets"))
	{
		l_vtrimeddec = m_vtrimeddecALLCs;
		//m_vUnUsedDecList = m_setUnUsedAliasCS;
	}
	else {
		//other groups
	}

	/**************************************************************/
	if (m_sSelected == wxT("Declarations") && l_bIsColoured)
	{
		for (int i = 0; i < m_vSimpleCsInfo.size(); i++)
		{
			if (m_vSimpleCsInfo[i].m_bIsUSed == false)
			{
				wxString l_sgroup = wxT("Simple Color Sets");
				m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
				{
					if (itmap->second == m_vSimpleCsInfo[i].m_sName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId != NULL) {//!=NULL
							if (foundId.IsOk()) {
								wxColour l_nColor1(wxT("RED"));
								pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
							}
						}

					}
				}
				/*******comp cs*********/
				if (m_vSimpleCsInfo[i].m_setCompCs.size() > 0)
				{
					wxString l_sgroupCompCs = wxT("Compound Color Sets");
					m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroupCompCs);
					set<wxString> l_setCompCs = m_vSimpleCsInfo[i].m_setCompCs;

					for (auto it_setcomp = l_setCompCs.begin(); it_setcomp != l_setCompCs.end(); ++it_setcomp)
					{
						for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
						{
							if (itmap->second == *it_setcomp)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId != NULL) {//!=NULL
									if (foundId.IsOk()) {
										wxColour l_nColor1(wxT("RED"));
										pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
									}
								}

							}
						}
					}
				}
				/*******funs***/
				if (m_vSimpleCsInfo[i].m_setFun.size() > 0) {
					wxString l_sgroupfun = wxT("Functions");
					m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupfun);
					set<wxString> l_setFuns = m_vSimpleCsInfo[i].m_setFun;
					for (auto it_setFuns = l_setFuns.begin(); it_setFuns != l_setFuns.end(); ++it_setFuns)
					{
						for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
						{
							if (itmap->second == *it_setFuns)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId != NULL) {//!=NULL
									if (foundId.IsOk())
									{
										wxColour l_nColor1(wxT("RED"));
										pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
									}

								}

							}
						}
					}
				}
				/************constants******/
				if (m_vSimpleCsInfo[i].m_setConst.size() > 0) {
					wxString l_sgroupfun = wxT("Constants");
					m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupfun);
					set<wxString> l_setCons = m_vSimpleCsInfo[i].m_setConst;
					for (auto it_setCons = l_setCons.begin(); it_setCons != l_setCons.end(); ++it_setCons)
					{
						for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
						{
							if (itmap->second == *it_setCons)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId != NULL) {//!=NULL
									if (foundId.IsOk())
									{
										m_vUnUsedConstantsList.insert(l_sLabel);
										wxColour l_nColor1(wxT("RED"));
										pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
									}

								}

							}
						}
					}
				}
				/***********Variables******/
				if (m_vSimpleCsInfo[i].m_setVar.size() > 0) {

					wxString l_sgroupVar = wxT("Variables");
					m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroupVar);
					set<wxString> l_setVar = m_vSimpleCsInfo[i].m_setVar;
					for (auto it_setvar = l_setVar.begin(); it_setvar != l_setVar.end(); ++it_setvar)
					{
						for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
						{
							if (itmap->second == *it_setvar)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId != NULL) {//!=NULL
									if (foundId.IsOk()) {
										wxColour l_nColor1(wxT("RED"));
										pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
									}
								}

							}
						}
					}
				}
			}

		}
		/***compound (subset) group*********************/
		wxString l_sgroup;
		//compound
		for (int i = 0; i < m_vCompoundCsInfo.size(); i++)
		{
			if (m_vCompoundCsInfo[i].m_bIsUSed == false)
			{
				l_sgroup = wxT("Compound Color Sets");
				m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
				{
					if (itmap->second == m_vCompoundCsInfo[i].m_sName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId != NULL) {//!=NULL
							if (foundId.IsOk()) {
								wxColour l_nColor1(wxT("RED"));
								pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
							}
						}

					}
				}
				/*Functions*/
				l_sgroup = wxT("Functions");
				m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setFuns = m_vCompoundCsInfo[i].m_setFun;
				for (auto it_setFuns = l_setFuns.begin(); it_setFuns != l_setFuns.end(); ++it_setFuns)
				{
					for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
					{
						if (itmap->second == *it_setFuns)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}
				/*****Simple cs***/
				l_sgroup = wxT("Simple Color Sets");
				m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setSimplecs = m_vCompoundCsInfo[i].m_setSimpleCs;
				for (auto it_setsimple = l_setSimplecs.begin(); it_setsimple != l_setSimplecs.end(); ++it_setsimple)
				{
					for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
					{
						if (itmap->second == *it_setsimple)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}

				/*constants*/
				l_sgroup = wxT("Constants");
				m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setCons = m_vCompoundCsInfo[i].m_setConst;
				for (auto it_setCons = l_setCons.begin(); it_setCons != l_setCons.end(); ++it_setCons)
				{
					for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
					{
						if (itmap->second == *it_setCons)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									m_vUnUsedConstantsList.insert(l_sLabel);
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}
				/****variables****/
				l_sgroup = wxT("Variables");
				m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setvar = m_vCompoundCsInfo[i].m_setVar;
				for (auto it_setVar = l_setvar.begin(); it_setVar != l_setvar.end(); ++it_setVar)
				{
					for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
					{
						if (itmap->second == *it_setVar)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}


			}
		}//end for compound
		for (int i = 0; i < m_vSubCsInfo.size(); i++)
		{
			if (m_vSubCsInfo[i].m_bIsUSed == false)
			{
				l_sgroup = wxT("Compound Color Sets");
				m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
				{
					if (itmap->second == m_vSubCsInfo[i].m_sName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId != NULL) {//!=NULL
							if (foundId.IsOk()) {
								wxColour l_nColor1(wxT("RED"));
								pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
							}
						}

					}
				}
				/*simple cs dependencies*/
				l_sgroup = wxT("Simple Color Sets");
				m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setSimplecs = m_vSubCsInfo[i].m_setSimpleCs;
				for (auto it_setsimple = l_setSimplecs.begin(); it_setsimple != l_setSimplecs.end(); ++it_setsimple)
				{
					for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
					{
						if (itmap->second == *it_setsimple)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}

				/***constants dependencies ***/
				l_sgroup = wxT("Constants");
				m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setCons = m_vSubCsInfo[i].m_setConst;
				for (auto it_setCons = l_setCons.begin(); it_setCons != l_setCons.end(); ++it_setCons)
				{
					for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
					{
						if (itmap->second == *it_setCons)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									m_vUnUsedConstantsList.insert(l_sLabel);
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}
				/*********var dependencies*******/
				l_sgroup = wxT("Variables");
				m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setvar = m_vSubCsInfo[i].m_setVar;
				for (auto it_setVar = l_setvar.begin(); it_setVar != l_setvar.end(); ++it_setVar)
				{
					for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
					{
						if (itmap->second == *it_setVar)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}
			}
		}
		/***constants group****************/
		for (int i = 0; i < m_vConstantsInfo.size(); i++)
		{
			if (m_vConstantsInfo[i].m_bIsUsed == false)
			{
				wxString l_sgroup = wxT("Constants");
				m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
				{
					if (itmap->second == m_vConstantsInfo[i].m_sConstName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId.IsOk()) {//!=NULL
							m_vUnUsedConstantsList.insert(l_sLabel);
							wxColour l_nColor1(wxT("RED"));
							pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
						}

					}
				}


				/******************************/
				if (m_vConstantsInfo[i].m_setFun.size() > 0)
				{
					wxString l_sgroupfun = wxT("Functions");
					m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupfun);
					set<wxString> l_setFuns = m_vConstantsInfo[i].m_setFun;
					for (auto it_setFuns = l_setFuns.begin(); it_setFuns != l_setFuns.end(); ++it_setFuns)
					{
						for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
						{
							if (itmap->second == *it_setFuns)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}
				/**************************************/
				if (m_vConstantsInfo[i].m_SetSimpleCS.size() > 0)
				{
					wxString l_sgroupSimpleCs = wxT("Simple Color Sets");
					m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroupSimpleCs);
					set<wxString> l_setSimpleCS = m_vConstantsInfo[i].m_SetSimpleCS;
					for (auto it_setFuns = l_setSimpleCS.begin(); it_setFuns != l_setSimpleCS.end(); ++it_setFuns)
					{
						for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
						{
							if (itmap->second == *it_setFuns)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}//end simple cs
				if (m_vConstantsInfo[i].m_SetCompoundCS.size() > 0)
				{
					wxString l_sgroupCompCs = wxT("Compound Color Sets");
					m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroupCompCs);
					set<wxString> l_setCompCs = m_vConstantsInfo[i].m_SetCompoundCS;
					for (auto it_setFuns = l_setCompCs.begin(); it_setFuns != l_setCompCs.end(); ++it_setFuns)
					{
						for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
						{
							if (itmap->second == *it_setFuns)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}//end compound cs
				 /***var*/
				if (m_vConstantsInfo[i].m_setVar.size() > 0)
				{
					wxString l_sgroupVar = wxT("Variables");
					m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroupVar);
					set<wxString> l_setVar = m_vConstantsInfo[i].m_setVar;
					for (auto it_setFuns = l_setVar.begin(); it_setFuns != l_setVar.end(); ++it_setFuns)
					{
						for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
						{
							if (itmap->second == *it_setFuns)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}
			}
		}//end  all constants
		 /*functio group*****************/

		for (int i = 0; i < m_vFunctionsInfo.size(); i++)
		{
			if (m_vFunctionsInfo[i].m_bIsUSed == false)
			{
				wxString l_sgroup = wxT("Functions");
				m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
				{
					if (itmap->second == m_vFunctionsInfo[i].m_sFunName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId.IsOk()) {//!=NULL

							wxColour l_nColor1(wxT("RED"));
							pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
						}

					}
				}
				/***constants group**/
				if (m_vFunctionsInfo[i].m_setConst.size() > 0)
				{
					wxString l_sgroupConst = wxT("Constants");
					m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupConst);
					set<wxString> l_setConst = m_vFunctionsInfo[i].m_setConst;
					for (auto it_setCons = l_setConst.begin(); it_setCons != l_setConst.end(); ++it_setCons)
					{
						for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
						{
							if (itmap->second == *it_setCons)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}

				/***var**/
				if (m_vFunctionsInfo[i].m_setVars.size() > 0)
				{
					wxString l_sgroupVar = wxT("Variables");
					m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroupVar);
					set<wxString> l_setVar = m_vFunctionsInfo[i].m_setVars;
					for (auto it_setVar = l_setVar.begin(); it_setVar != l_setVar.end(); ++it_setVar)
					{
						for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
						{
							if (itmap->second == *it_setVar)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}//end var
				 /******Simple Cs*/
				if (m_vFunctionsInfo[i].m_setSimpleCs.size() > 0)
				{
					wxString l_sgroupSimpleCs = wxT("Simple Color Sets");
					m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroupSimpleCs);
					set<wxString> l_setSimpleCS = m_vFunctionsInfo[i].m_setSimpleCs;
					for (auto it_setSimplecs = l_setSimpleCS.begin(); it_setSimplecs != l_setSimpleCS.end(); ++it_setSimplecs)
					{
						for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
						{
							if (itmap->second == *it_setSimplecs)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}//end simple cs
				 /***compound cs**/
				if (m_vFunctionsInfo[i].m_setCompoundCs.size() > 0)
				{
					wxString l_sgroupCompCs = wxT("Compound Color Sets");
					m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroupCompCs);
					set<wxString> l_setCompCs = m_vFunctionsInfo[i].m_setCompoundCs;
					for (auto it_setCompcs = l_setCompCs.begin(); it_setCompcs != l_setCompCs.end(); ++it_setCompcs)
					{
						for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
						{
							if (itmap->second == *it_setCompcs)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}//end compound cs
				 /*****/
			}
		}//end functions group
		 /*******var group*/
		for (int i = 0; i < m_vVariablesInfo.size(); i++)
		{
			if (m_vVariablesInfo[i].m_bIsUSed == false)
			{
				wxString l_sgroup = wxT("Variables");
				m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
				{
					if (itmap->second == m_vVariablesInfo[i].m_sName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId.IsOk()) {//!=NULL

							wxColour l_nColor1(wxT("RED"));
							pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
						}

					}
				}
				/***constants group**/
				if (m_vVariablesInfo[i].m_setConst.size() > 0)
				{
					wxString l_sgroupConst = wxT("Constants");
					m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupConst);
					set<wxString> l_setConst = m_vVariablesInfo[i].m_setConst;
					for (auto it_setCons = l_setConst.begin(); it_setCons != l_setConst.end(); ++it_setCons)
					{
						for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
						{
							if (itmap->second == *it_setCons)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}

				/***fun**/
				if (m_vVariablesInfo[i].m_setFun.size() > 0)
				{
					wxString l_sgroupFun = wxT("Functions");
					m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupFun);
					set<wxString> l_setFuns = m_vVariablesInfo[i].m_setFun;
					for (auto it_setf = l_setFuns.begin(); it_setf != l_setFuns.end(); ++it_setf)
					{
						for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
						{
							if (itmap->second == *it_setf)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}//end var
				 /******Simple Cs*/
				if (m_vVariablesInfo[i].m_setSimpleCs.size() > 0)
				{
					wxString l_sgroupSimpleCs = wxT("Simple Color Sets");
					m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroupSimpleCs);
					set<wxString> l_setSimpleCS = m_vVariablesInfo[i].m_setSimpleCs;
					for (auto it_setSimplecs = l_setSimpleCS.begin(); it_setSimplecs != l_setSimpleCS.end(); ++it_setSimplecs)
					{
						for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
						{
							if (itmap->second == *it_setSimplecs)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}//end simple cs
				 /***compound cs**/
				if (m_vVariablesInfo[i].m_CompoundCS.size() > 0)
				{
					wxString l_sgroupCompCs = wxT("Compound Color Sets");
					m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroupCompCs);
					set<wxString> l_setCompCs = m_vVariablesInfo[i].m_CompoundCS;
					for (auto it_setCompcs = l_setCompCs.begin(); it_setCompcs != l_setCompCs.end(); ++it_setCompcs)
					{
						for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
						{
							if (itmap->second == *it_setCompcs)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}//end compound cs
				 /*****/
			}
		}
		//alias cs
		for (int i = 0; i < m_vAliasCsInfo.size(); i++)
		{
			wxString l_sgroup;
			if (m_vAliasCsInfo[i].m_bIsUSed == false)
			{
				l_sgroup = wxT("Alias Color Sets");
				m_vtrimeddecAliasCS = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecAliasCS.begin(); itmap != m_vtrimeddecAliasCS.end(); ++itmap)
				{
					if (itmap->second == m_vAliasCsInfo[i].m_sName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId != NULL) {//!=NULL
							if (foundId.IsOk()) {
								wxColour l_nColor1(wxT("RED"));
								pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
							}
						}

					}
				}
				//simple cs
				l_sgroup = wxT("Simple Color Sets");
				m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setSimplecs = m_vAliasCsInfo[i].m_setSimpleCs;
				for (auto it_setsimple = l_setSimplecs.begin(); it_setsimple != l_setSimplecs.end(); ++it_setsimple)
				{
					for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
					{
						if (itmap->second == *it_setsimple)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}//end simple
				l_sgroup = wxT("Compound Color Sets");
				m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> it_setsimple = m_vAliasCsInfo[i].m_setCompCs;
				for (auto it_setsimple = l_setSimplecs.begin(); it_setsimple != l_setSimplecs.end(); ++it_setsimple)
				{
					for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
					{
						if (itmap->second == *it_setsimple)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}//end compound cs
				 //vars
				l_sgroup = wxT("Variables");
				m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setvar = m_vAliasCsInfo[i].m_setVar;
				for (auto it_setVar = l_setvar.begin(); it_setVar != l_setvar.end(); ++it_setVar)
				{
					for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
					{
						if (itmap->second == *it_setVar)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}
				//funs
				l_sgroup = wxT("Functions");
				m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setFuns = m_vAliasCsInfo[i].m_setFun;
				for (auto it_setFuns = l_setFuns.begin(); it_setFuns != l_setFuns.end(); ++it_setFuns)
				{
					for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
					{
						if (itmap->second == *it_setFuns)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}//end funs
				 //consts




			}

		}
		//end alias
	}//close declaration

	 /************************************************************/
	if (m_sSelected == wxT("Color Sets") && l_bIsColoured)
	{
		for (int i = 0; i < m_vSimpleCsInfo.size(); i++)
		{
			if (m_vSimpleCsInfo[i].m_bIsUSed == false)
			{
				wxString l_sgroup = wxT("Simple Color Sets");
				m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
				{
					if (itmap->second == m_vSimpleCsInfo[i].m_sName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId != NULL) {//!=NULL
							if (foundId.IsOk()) {
								wxColour l_nColor1(wxT("RED"));
								pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
							}
						}

					}

				}
				set<wxString> l_setIndirectSimpleCS = m_vSimpleCsInfo[i].m_setIndirectCs;
				for (auto it_setIndirect = l_setIndirectSimpleCS.begin(); it_setIndirect != l_setIndirectSimpleCS.end(); ++it_setIndirect)
				{

					for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
					{
						if (itmap->second == *it_setIndirect)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}

					}
				}

				/*******comp cs*********/
				if (m_vSimpleCsInfo[i].m_setCompCs.size() > 0)
				{
					wxString l_sgroupCompCs = wxT("Compound Color Sets");
					m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroupCompCs);
					set<wxString> l_setCompCs = m_vSimpleCsInfo[i].m_setCompCs;

					for (auto it_setcomp = l_setCompCs.begin(); it_setcomp != l_setCompCs.end(); ++it_setcomp)
					{
						for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
						{
							if (itmap->second == *it_setcomp)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId != NULL) {//!=NULL
									if (foundId.IsOk()) {
										wxColour l_nColor1(wxT("RED"));
										pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
									}
								}

							}
						}
					}
				}


				/*******funs***/
				if (m_vSimpleCsInfo[i].m_setFun.size() > 0) {
					wxString l_sgroupfun = wxT("Functions");
					m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupfun);
					set<wxString> l_setFuns = m_vSimpleCsInfo[i].m_setFun;
					for (auto it_setFuns = l_setFuns.begin(); it_setFuns != l_setFuns.end(); ++it_setFuns)
					{
						for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
						{
							if (itmap->second == *it_setFuns)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId != NULL) {//!=NULL
									if (foundId.IsOk())
									{
										wxColour l_nColor1(wxT("RED"));
										pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
									}

								}

							}
						}
					}
				}
				/************constants******/
				if (m_vSimpleCsInfo[i].m_setConst.size() > 0) {
					wxString l_sgroupfun = wxT("Constants");
					m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupfun);
					set<wxString> l_setCons = m_vSimpleCsInfo[i].m_setConst;
					for (auto it_setCons = l_setCons.begin(); it_setCons != l_setCons.end(); ++it_setCons)
					{
						for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
						{
							if (itmap->second == *it_setCons)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId != NULL) {//!=NULL
									if (foundId.IsOk())
									{
										m_vUnUsedConstantsList.insert(l_sLabel);
										wxColour l_nColor1(wxT("RED"));
										pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
									}

								}

							}
						}
					}
				}
				/***********Variables******/
				if (m_vSimpleCsInfo[i].m_setVar.size() > 0) {

					wxString l_sgroupVar = wxT("Variables");
					m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroupVar);
					set<wxString> l_setVar = m_vSimpleCsInfo[i].m_setVar;
					for (auto it_setvar = l_setVar.begin(); it_setvar != l_setVar.end(); ++it_setvar)
					{
						for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
						{
							if (itmap->second == *it_setvar)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId != NULL) {//!=NULL
									if (foundId.IsOk()) {
										wxColour l_nColor1(wxT("RED"));
										pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
									}
								}

							}
						}
					}
				}
			}

		}
		/***compound (subset) group***/
		wxString l_sgroup;
		//compound
		for (int i = 0; i < m_vCompoundCsInfo.size(); i++)
		{
			if (m_vCompoundCsInfo[i].m_bIsUSed == false)
			{
				l_sgroup = wxT("Compound Color Sets");
				m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
				{
					if (itmap->second == m_vCompoundCsInfo[i].m_sName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId != NULL) {//!=NULL
							if (foundId.IsOk()) {
								wxColour l_nColor1(wxT("RED"));
								pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
							}
						}

					}
				}

				set<wxString> l_setIndirectCompCS = m_vCompoundCsInfo[i].m_setIndirectCompCs;
				for (auto it_setIndirect = l_setIndirectCompCS.begin(); it_setIndirect != l_setIndirectCompCS.end(); ++it_setIndirect)
				{

					for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
					{
						if (itmap->second == *it_setIndirect)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}

					}
				}
				/*Functions*/
				l_sgroup = wxT("Functions");
				m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setFuns = m_vCompoundCsInfo[i].m_setFun;
				for (auto it_setFuns = l_setFuns.begin(); it_setFuns != l_setFuns.end(); ++it_setFuns)
				{
					for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
					{
						if (itmap->second == *it_setFuns)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}
				/*****Simple cs***/
				l_sgroup = wxT("Simple Color Sets");
				m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setSimplecs = m_vCompoundCsInfo[i].m_setSimpleCs;
				for (auto it_setsimple = l_setSimplecs.begin(); it_setsimple != l_setSimplecs.end(); ++it_setsimple)
				{
					for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
					{
						if (itmap->second == *it_setsimple)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}

				/*constants*/
				l_sgroup = wxT("Constants");
				m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setCons = m_vCompoundCsInfo[i].m_setConst;
				for (auto it_setCons = l_setCons.begin(); it_setCons != l_setCons.end(); ++it_setCons)
				{
					for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
					{
						if (itmap->second == *it_setCons)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									m_vUnUsedConstantsList.insert(l_sLabel);
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}
				/****variables****/
				l_sgroup = wxT("Variables");
				m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setvar = m_vCompoundCsInfo[i].m_setVar;
				for (auto it_setVar = l_setvar.begin(); it_setVar != l_setvar.end(); ++it_setVar)
				{
					for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
					{
						if (itmap->second == *it_setVar)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}


			}
		}//end for compound
		for (int i = 0; i < m_vSubCsInfo.size(); i++)
		{
			if (m_vSubCsInfo[i].m_bIsUSed == false)
			{
				l_sgroup = wxT("Compound Color Sets");
				m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
				{
					if (itmap->second == m_vSubCsInfo[i].m_sName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId != NULL) {//!=NULL
							if (foundId.IsOk()) {
								wxColour l_nColor1(wxT("RED"));
								pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
							}
						}

					}
				}
				/*simple cs dependencies*/
				l_sgroup = wxT("Simple Color Sets");
				m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setSimplecs = m_vSubCsInfo[i].m_setSimpleCs;
				for (auto it_setsimple = l_setSimplecs.begin(); it_setsimple != l_setSimplecs.end(); ++it_setsimple)
				{
					for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
					{
						if (itmap->second == *it_setsimple)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}

				/***constants dependencies ***/
				l_sgroup = wxT("Constants");
				m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setCons = m_vSubCsInfo[i].m_setConst;
				for (auto it_setCons = l_setCons.begin(); it_setCons != l_setCons.end(); ++it_setCons)
				{
					for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
					{
						if (itmap->second == *it_setCons)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									m_vUnUsedConstantsList.insert(l_sLabel);
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}
				/*********var dependencies*******/
				l_sgroup = wxT("Variables");
				m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setvar = m_vSubCsInfo[i].m_setVar;
				for (auto it_setVar = l_setvar.begin(); it_setVar != l_setvar.end(); ++it_setVar)
				{
					for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
					{
						if (itmap->second == *it_setVar)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}
			}
		}

		/*Alias Cs*/
		for (int i = 0; i < m_vAliasCsInfo.size(); i++)
		{
			wxString l_sgroup;
			if (m_vAliasCsInfo[i].m_bIsUSed == false)
			{
				l_sgroup = wxT("Alias Color Sets");
				m_vtrimeddecAliasCS = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecAliasCS.begin(); itmap != m_vtrimeddecAliasCS.end(); ++itmap)
				{
					if (itmap->second == m_vAliasCsInfo[i].m_sName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId != NULL) {//!=NULL
							if (foundId.IsOk()) {
								wxColour l_nColor1(wxT("RED"));
								pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
							}
						}

					}
				}
				//simple cs
				l_sgroup = wxT("Simple Color Sets");
				m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setSimplecs = m_vAliasCsInfo[i].m_setSimpleCs;
				for (auto it_setsimple = l_setSimplecs.begin(); it_setsimple != l_setSimplecs.end(); ++it_setsimple)
				{
					for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
					{
						if (itmap->second == *it_setsimple)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}//end simple
				l_sgroup = wxT("Compound Color Sets");
				m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> it_setsimple = m_vAliasCsInfo[i].m_setCompCs;
				for (auto it_setsimple = l_setSimplecs.begin(); it_setsimple != l_setSimplecs.end(); ++it_setsimple)
				{
					for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
					{
						if (itmap->second == *it_setsimple)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}//end compound cs
				 //vars
				l_sgroup = wxT("Variables");
				m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setvar = m_vAliasCsInfo[i].m_setVar;
				for (auto it_setVar = l_setvar.begin(); it_setVar != l_setvar.end(); ++it_setVar)
				{
					for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
					{
						if (itmap->second == *it_setVar)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}
				//funs
				l_sgroup = wxT("Functions");
				m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setFuns = m_vAliasCsInfo[i].m_setFun;
				for (auto it_setFuns = l_setFuns.begin(); it_setFuns != l_setFuns.end(); ++it_setFuns)
				{
					for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
					{
						if (itmap->second == *it_setFuns)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}//end funs
				 //consts




			}

		}
	}//closed color sets
	 /**************************************************************/
	if (m_sSelected == wxT("Functions") && l_bIsColoured)
	{
		for (int i = 0; i < m_vFunctionsInfo.size(); i++)
		{
			if (m_vFunctionsInfo[i].m_bIsUSed == false)
			{
				wxString l_sgroup = wxT("Functions");
				m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
				{
					if (itmap->second == m_vFunctionsInfo[i].m_sFunName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId.IsOk()) {//!=NULL

							wxColour l_nColor1(wxT("RED"));
							pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
						}

					}
				}
				/***constants group**/
				if (m_vFunctionsInfo[i].m_setConst.size() > 0)
				{
					wxString l_sgroupConst = wxT("Constants");
					m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupConst);
					set<wxString> l_setConst = m_vFunctionsInfo[i].m_setConst;
					for (auto it_setCons = l_setConst.begin(); it_setCons != l_setConst.end(); ++it_setCons)
					{
						for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
						{
							if (itmap->second == *it_setCons)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}

				/***var**/
				if (m_vFunctionsInfo[i].m_setVars.size() > 0)
				{
					wxString l_sgroupVar = wxT("Variables");
					m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroupVar);
					set<wxString> l_setVar = m_vFunctionsInfo[i].m_setVars;
					for (auto it_setVar = l_setVar.begin(); it_setVar != l_setVar.end(); ++it_setVar)
					{
						for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
						{
							if (itmap->second == *it_setVar)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}//end var
				 /******Simple Cs*/
				if (m_vFunctionsInfo[i].m_setSimpleCs.size() > 0)
				{
					wxString l_sgroupSimpleCs = wxT("Simple Color Sets");
					m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroupSimpleCs);
					set<wxString> l_setSimpleCS = m_vFunctionsInfo[i].m_setSimpleCs;
					for (auto it_setSimplecs = l_setSimpleCS.begin(); it_setSimplecs != l_setSimpleCS.end(); ++it_setSimplecs)
					{
						for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
						{
							if (itmap->second == *it_setSimplecs)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}//end simple cs
				 /***compound cs**/
				if (m_vFunctionsInfo[i].m_setCompoundCs.size() > 0)
				{
					wxString l_sgroupCompCs = wxT("Compound Color Sets");
					m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroupCompCs);
					set<wxString> l_setCompCs = m_vFunctionsInfo[i].m_setCompoundCs;
					for (auto it_setCompcs = l_setCompCs.begin(); it_setCompcs != l_setCompCs.end(); ++it_setCompcs)
					{
						for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
						{
							if (itmap->second == *it_setCompcs)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}//end compound cs
				 /*****/
			}
		}
	}

	/***********************variables***************/

	if (m_sSelected == wxT("Variables") && l_bIsColoured)
	{
		for (int i = 0; i < m_vVariablesInfo.size(); i++)
		{
			if (m_vVariablesInfo[i].m_bIsUSed == false)
			{
				wxString l_sgroup = wxT("Variables");
				m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
				{
					if (itmap->second == m_vVariablesInfo[i].m_sName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId.IsOk()) {//!=NULL

							wxColour l_nColor1(wxT("RED"));
							pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
						}

					}
				}
				/***constants group**/
				if (m_vVariablesInfo[i].m_setConst.size() > 0)
				{
					wxString l_sgroupConst = wxT("Constants");
					m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupConst);
					set<wxString> l_setConst = m_vVariablesInfo[i].m_setConst;
					for (auto it_setCons = l_setConst.begin(); it_setCons != l_setConst.end(); ++it_setCons)
					{
						for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
						{
							if (itmap->second == *it_setCons)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}

				/***var**/
				if (m_vVariablesInfo[i].m_setFun.size() > 0)
				{
					wxString l_sgroupFun = wxT("Functions");
					m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupFun);
					set<wxString> l_setFuns = m_vVariablesInfo[i].m_setFun;
					for (auto it_setf = l_setFuns.begin(); it_setf != l_setFuns.end(); ++it_setf)
					{
						for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
						{
							if (itmap->second == *it_setf)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}//end var
				 /******Simple Cs*/
				if (m_vVariablesInfo[i].m_setSimpleCs.size() > 0)
				{
					wxString l_sgroupSimpleCs = wxT("Simple Color Sets");
					m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroupSimpleCs);
					set<wxString> l_setSimpleCS = m_vVariablesInfo[i].m_setSimpleCs;
					for (auto it_setSimplecs = l_setSimpleCS.begin(); it_setSimplecs != l_setSimpleCS.end(); ++it_setSimplecs)
					{
						for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
						{
							if (itmap->second == *it_setSimplecs)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}//end simple cs
				 /***compound cs**/
				if (m_vVariablesInfo[i].m_CompoundCS.size() > 0)
				{
					wxString l_sgroupCompCs = wxT("Compound Color Sets");
					m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroupCompCs);
					set<wxString> l_setCompCs = m_vVariablesInfo[i].m_CompoundCS;
					for (auto it_setCompcs = l_setCompCs.begin(); it_setCompcs != l_setCompCs.end(); ++it_setCompcs)
					{
						for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
						{
							if (itmap->second == *it_setCompcs)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}//end compound cs
				 /*****/
			}
		}
	}

	/**************************************************************/
	if (m_sSelected == wxT("Constants") && l_bIsColoured) {
		for (int i = 0; i < m_vConstantsInfo.size(); i++)
		{
			if (m_vConstantsInfo[i].m_bIsUsed == false)
			{
				wxString l_sgroup = wxT("Constants");
				m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
				{
					if (itmap->second == m_vConstantsInfo[i].m_sConstName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId.IsOk()) {//!=NULL
							m_vUnUsedConstantsList.insert(l_sLabel);
							wxColour l_nColor1(wxT("RED"));
							pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
						}

					}
				}
				/******************************/
				if (m_vConstantsInfo[i].m_setFun.size() > 0)
				{
					wxString l_sgroupfun = wxT("Functions");
					m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupfun);
					set<wxString> l_setFuns = m_vConstantsInfo[i].m_setFun;
					for (auto it_setFuns = l_setFuns.begin(); it_setFuns != l_setFuns.end(); ++it_setFuns)
					{
						for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
						{
							if (itmap->second == *it_setFuns)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}

				}
				/**************************************/
				if (m_vConstantsInfo[i].m_SetSimpleCS.size() > 0)
				{
					wxString l_sgroupSimpleCs = wxT("Simple Color Sets");
					m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroupSimpleCs);
					set<wxString> l_setSimpleCS = m_vConstantsInfo[i].m_SetSimpleCS;
					for (auto it_setFuns = l_setSimpleCS.begin(); it_setFuns != l_setSimpleCS.end(); ++it_setFuns)
					{
						for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
						{
							if (itmap->second == *it_setFuns)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}//end simple cs
				if (m_vConstantsInfo[i].m_SetCompoundCS.size() > 0)
				{
					wxString l_sgroupCompCs = wxT("Compound Color Sets");
					m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroupCompCs);
					set<wxString> l_setCompCs = m_vConstantsInfo[i].m_SetCompoundCS;
					for (auto it_setFuns = l_setCompCs.begin(); it_setFuns != l_setCompCs.end(); ++it_setFuns)
					{
						for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
						{
							if (itmap->second == *it_setFuns)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}//end compound cs
				 /***var*/
				if (m_vConstantsInfo[i].m_setVar.size() > 0)
				{
					wxString l_sgroupVar = wxT("Variables");
					m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroupVar);
					set<wxString> l_setVar = m_vConstantsInfo[i].m_setVar;
					for (auto it_setFuns = l_setVar.begin(); it_setFuns != l_setVar.end(); ++it_setFuns)
					{
						for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
						{
							if (itmap->second == *it_setFuns)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId.IsOk()) {//!=NULL
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}

							}
						}
					}
				}//end var
			}
		}
	}
	else if (m_sSelected == wxT("Simple Color Sets") && l_bIsColoured)
	{
		for (int i = 0; i < m_vSimpleCsInfo.size(); i++)
		{
			if (m_vSimpleCsInfo[i].m_bIsUSed == false)
			{
				wxString l_sgroup = wxT("Simple Color Sets");
				m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
				{
					if (itmap->second == m_vSimpleCsInfo[i].m_sName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId != NULL) {//!=NULL
							if (foundId.IsOk()) {
								wxColour l_nColor1(wxT("RED"));
								pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
							}
						}

					}
				}
				set<wxString> l_setIndirectSimpleCS = m_vSimpleCsInfo[i].m_setIndirectCs;
				for (auto it_setIndirect = l_setIndirectSimpleCS.begin(); it_setIndirect != l_setIndirectSimpleCS.end(); ++it_setIndirect)
				{

					for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
					{
						if (itmap->second == *it_setIndirect)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}

					}
				}
				/*******comp cs*********/
				if (m_vSimpleCsInfo[i].m_setCompCs.size() > 0)
				{
					wxString l_sgroupCompCs = wxT("Compound Color Sets");
					m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroupCompCs);
					set<wxString> l_setCompCs = m_vSimpleCsInfo[i].m_setCompCs;

					for (auto it_setcomp = l_setCompCs.begin(); it_setcomp != l_setCompCs.end(); ++it_setcomp)
					{
						for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
						{
							if (itmap->second == *it_setcomp)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId != NULL) {//!=NULL
									if (foundId.IsOk()) {
										wxColour l_nColor1(wxT("RED"));
										pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
									}
								}

							}
						}
					}
				}
				/*******funs***/
				if (m_vSimpleCsInfo[i].m_setFun.size() > 0) {
					wxString l_sgroupfun = wxT("Functions");
					m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroupfun);
					set<wxString> l_setFuns = m_vSimpleCsInfo[i].m_setFun;
					for (auto it_setFuns = l_setFuns.begin(); it_setFuns != l_setFuns.end(); ++it_setFuns)
					{
						for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
						{
							if (itmap->second == *it_setFuns)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId != NULL) {//!=NULL
									if (foundId.IsOk())
									{
										wxColour l_nColor1(wxT("RED"));
										pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
									}

								}

							}
						}
					}
				}
				/************constants******/
				if (m_vSimpleCsInfo[i].m_setConst.size() > 0) {
					wxString l_sgroupfun = wxT("Constants");
					m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroupfun);
					set<wxString> l_setCons = m_vSimpleCsInfo[i].m_setConst;
					for (auto it_setCons = l_setCons.begin(); it_setCons != l_setCons.end(); ++it_setCons)
					{
						for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
						{
							if (itmap->second == *it_setCons)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId != NULL) {//!=NULL
									if (foundId.IsOk())
									{
										m_vUnUsedConstantsList.insert(l_sLabel);
										wxColour l_nColor1(wxT("RED"));
										pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
									}

								}

							}
						}
					}
				}
				/***********Variables******/
				if (m_vSimpleCsInfo[i].m_setVar.size() > 0) {

					wxString l_sgroupVar = wxT("Variables");
					m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroupVar);
					set<wxString> l_setVar = m_vSimpleCsInfo[i].m_setVar;
					for (auto it_setvar = l_setVar.begin(); it_setvar != l_setVar.end(); ++it_setvar)
					{
						for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
						{
							if (itmap->second == *it_setvar)
							{
								wxString l_sLabel = itmap->first;
								wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
								if (foundId != NULL) {//!=NULL
									if (foundId.IsOk()) {
										wxColour l_nColor1(wxT("RED"));
										pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
									}
								}

							}
						}
					}
				}
			}

		}
	}
	else if (m_sSelected == wxT("Compound Color Sets") && l_bIsColoured)
	{
		wxString l_sgroup;
		//compound
		for (int i = 0; i < m_vCompoundCsInfo.size(); i++)
		{
			if (m_vCompoundCsInfo[i].m_bIsUSed == false)
			{
				l_sgroup = wxT("Compound Color Sets");
				m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
				{
					if (itmap->second == m_vCompoundCsInfo[i].m_sName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId != NULL) {//!=NULL
							if (foundId.IsOk()) {
								wxColour l_nColor1(wxT("RED"));
								pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
							}
						}

					}
				}


				set<wxString> l_setIndirectCompCS = m_vCompoundCsInfo[i].m_setIndirectCompCs;
				for (auto it_setIndirect = l_setIndirectCompCS.begin(); it_setIndirect != l_setIndirectCompCS.end(); ++it_setIndirect)
				{

					for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
					{
						if (itmap->second == *it_setIndirect)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}

					}
				}
				/*Functions*/
				l_sgroup = wxT("Functions");
				m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setFuns = m_vCompoundCsInfo[i].m_setFun;
				for (auto it_setFuns = l_setFuns.begin(); it_setFuns != l_setFuns.end(); ++it_setFuns)
				{
					for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
					{
						if (itmap->second == *it_setFuns)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}
				/*****Simple cs***/
				l_sgroup = wxT("Simple Color Sets");
				m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setSimplecs = m_vCompoundCsInfo[i].m_setSimpleCs;
				for (auto it_setsimple = l_setSimplecs.begin(); it_setsimple != l_setSimplecs.end(); ++it_setsimple)
				{
					for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
					{
						if (itmap->second == *it_setsimple)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}

				/*constants*/
				l_sgroup = wxT("Constants");
				m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setCons = m_vCompoundCsInfo[i].m_setConst;
				for (auto it_setCons = l_setCons.begin(); it_setCons != l_setCons.end(); ++it_setCons)
				{
					for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
					{
						if (itmap->second == *it_setCons)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									m_vUnUsedConstantsList.insert(l_sLabel);
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}
				/****variables****/
				l_sgroup = wxT("Variables");
				m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setvar = m_vCompoundCsInfo[i].m_setVar;
				for (auto it_setVar = l_setvar.begin(); it_setVar != l_setvar.end(); ++it_setVar)
				{
					for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
					{
						if (itmap->second == *it_setVar)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}


			}
		}//end for compound
		for (int i = 0; i < m_vSubCsInfo.size(); i++)
		{
			if (m_vSubCsInfo[i].m_bIsUSed == false)
			{
				l_sgroup = wxT("Compound Color Sets");
				m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
				{
					if (itmap->second == m_vSubCsInfo[i].m_sName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId != NULL) {//!=NULL
							if (foundId.IsOk()) {
								wxColour l_nColor1(wxT("RED"));
								pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
							}
						}

					}
				}
				/*simple cs dependencies*/
				l_sgroup = wxT("Simple Color Sets");
				m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setSimplecs = m_vSubCsInfo[i].m_setSimpleCs;
				for (auto it_setsimple = l_setSimplecs.begin(); it_setsimple != l_setSimplecs.end(); ++it_setsimple)
				{
					for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
					{
						if (itmap->second == *it_setsimple)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}

				/***constants dependencies ***/
				l_sgroup = wxT("Constants");
				m_vtrimeddecConstants = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setCons = m_vSubCsInfo[i].m_setConst;
				for (auto it_setCons = l_setCons.begin(); it_setCons != l_setCons.end(); ++it_setCons)
				{
					for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
					{
						if (itmap->second == *it_setCons)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									m_vUnUsedConstantsList.insert(l_sLabel);
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}
				/*********var dependencies*******/
				l_sgroup = wxT("Variables");
				m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setvar = m_vSubCsInfo[i].m_setVar;
				for (auto it_setVar = l_setvar.begin(); it_setVar != l_setvar.end(); ++it_setVar)
				{
					for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
					{
						if (itmap->second == *it_setVar)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}
			}
		}
	}//closed compound col cs
	else if (m_sSelected == wxT("Alias Color Sets") && l_bIsColoured)
	{
		for (int i = 0; i < m_vAliasCsInfo.size(); i++)
		{
			wxString l_sgroup;
			if (m_vAliasCsInfo[i].m_bIsUSed == false)
			{
				l_sgroup = wxT("Alias Color Sets");
				m_vtrimeddecAliasCS = l_UnDec.TrimUserDec(l_sgroup);
				for (auto itmap = m_vtrimeddecAliasCS.begin(); itmap != m_vtrimeddecAliasCS.end(); ++itmap)
				{
					if (itmap->second == m_vAliasCsInfo[i].m_sName)
					{
						wxString l_sLabel = itmap->first;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						if (foundId != NULL) {//!=NULL
							if (foundId.IsOk()) {
								wxColour l_nColor1(wxT("RED"));
								pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
							}
						}

					}
				}
				//simple cs
				l_sgroup = wxT("Simple Color Sets");
				m_vtrimeddecSimplCS = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setSimplecs = m_vAliasCsInfo[i].m_setSimpleCs;
				for (auto it_setsimple = l_setSimplecs.begin(); it_setsimple != l_setSimplecs.end(); ++it_setsimple)
				{
					for (auto itmap = m_vtrimeddecSimplCS.begin(); itmap != m_vtrimeddecSimplCS.end(); ++itmap)
					{
						if (itmap->second == *it_setsimple)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}//end simple
				l_sgroup = wxT("Compound Color Sets");
				m_vtrimeddecCompoundCS = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> it_setsimple = m_vAliasCsInfo[i].m_setCompCs;
				for (auto it_setsimple = l_setSimplecs.begin(); it_setsimple != l_setSimplecs.end(); ++it_setsimple)
				{
					for (auto itmap = m_vtrimeddecCompoundCS.begin(); itmap != m_vtrimeddecCompoundCS.end(); ++itmap)
					{
						if (itmap->second == *it_setsimple)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}//end compound cs
				 //vars
				l_sgroup = wxT("Variables");
				m_vtrimeddecVariables = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setvar = m_vAliasCsInfo[i].m_setVar;
				for (auto it_setVar = l_setvar.begin(); it_setVar != l_setvar.end(); ++it_setVar)
				{
					for (auto itmap = m_vtrimeddecVariables.begin(); itmap != m_vtrimeddecVariables.end(); ++itmap)
					{
						if (itmap->second == *it_setVar)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}
				//funs
				l_sgroup = wxT("Functions");
				m_vtrimeddecFunctions = l_UnDec.TrimUserDec(l_sgroup);
				set<wxString> l_setFuns = m_vAliasCsInfo[i].m_setFun;
				for (auto it_setFuns = l_setFuns.begin(); it_setFuns != l_setFuns.end(); ++it_setFuns)
				{
					for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
					{
						if (itmap->second == *it_setFuns)
						{
							wxString l_sLabel = itmap->first;
							wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
							if (foundId != NULL) {//!=NULL
								if (foundId.IsOk()) {
									wxColour l_nColor1(wxT("RED"));
									pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
								}
							}

						}
					}
				}//end funs
				 //consts




			}

		}

	}

	/**************************************************************/
	/*
	for (auto v_it = m_vUnUsedDecList.begin(); v_it != m_vUnUsedDecList.end(); ++v_it)
	{
	wxString l_label;
	bool l_bfound = false;

	auto it = l_vtrimeddec.begin();
	while (it != l_vtrimeddec.end())
	{
	wxString se = (*v_it);
	se = se.Trim();
	if (it->second.Trim() == se)
	{
	l_label = it->first;
	l_bfound = true;
	break;
	}
	++it;
	}
	if (l_bfound)
	{
	wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_label, true, true);
	if (foundId != NULL) {
	wxColour l_nColor1(wxT("RED"));
	pTreeCtrl->SetItemTextColour(foundId, l_nColor1);
	}
	}
	}

	*/


}





void SP_WDG_DeclarationTreectrl::DeleteConstants() {


	wxString l_sNet = m_pcGraph->GetNetclass()->GetDisplayName();
	bool l_bIsColored = false;
	if (!l_sNet.Contains(wxT("Colored")))
	{
		l_bIsColored = true;
		map<wxString, wxString> l_setAllConst = m_vtrimeddecConstants;

		for (auto it_setCon = m_vtrimeddecConstants.begin(); it_setCon != m_vtrimeddecConstants.end(); ++it_setCon)
		{
			wxString l_sConst = it_setCon->first;
			if (l_sConst.Contains(":"))
			{
				l_sConst = l_sConst.BeforeFirst(':').Trim();
			}

			map<wxString, wxString> l_setTobeDeleted;
			wxString l_sFun;
			bool l_bIsFun = false;
			wxString l_sObserver;
			bool l_bIsObserver = false;
			auto it_foundConstant = m_mDeclarations2Dependencies.find(l_sConst);
			if (it_foundConstant != m_mDeclarations2Dependencies.end())
			{
				l_setTobeDeleted = m_mDeclarations2Dependencies[l_sConst];
				for (auto it_toBedel = l_setTobeDeleted.begin(); it_toBedel != l_setTobeDeleted.end(); ++it_toBedel)
				{

					if (it_toBedel->second == wxT("Constant"))
					{
						auto it_findItem = m_mDeclarations2Dependencies.find(it_toBedel->first);
						if (it_findItem != m_mDeclarations2Dependencies.end())
						{
							m_unUsedDec.DeleteUnusedConstant(it_toBedel->first);
							m_mDeclarations2Dependencies.erase(it_toBedel->first);
						}

					}

					if (it_toBedel->second == wxT("Function"))
					{
						auto it_findItem = m_mDeclarations2Dependencies.find(it_toBedel->first);
						if (it_findItem != m_mDeclarations2Dependencies.end())
						{
							m_unUsedDec.DeleteUnusedFunction(it_toBedel->first);
							m_mDeclarations2Dependencies.erase(it_toBedel->first);
							for (auto it_m = m_vtrimeddecFunctions.begin(); it_m != m_vtrimeddecFunctions.end(); ++it_m)
							{
								if (it_toBedel->first == it_m->second)
								{
									l_sFun = it_m->first;
									l_bIsFun = true;
								}
							}

						}
					}
					if (it_toBedel->second == wxT("Observer"))
					{
						auto it_findItem = m_mDeclarations2Dependencies.find(it_toBedel->first);
						if (it_findItem != m_mDeclarations2Dependencies.end())
						{
							m_unUsedDec.DeleteUnUsedObserver(it_toBedel->first);
							m_mDeclarations2Dependencies.erase(it_toBedel->first);
							for (auto it_m = m_vtrimeddecObservers.begin(); it_m != m_vtrimeddecObservers.end(); ++it_m)
							{
								if (it_toBedel->first == it_m->second)
								{
									l_sObserver = it_m->first;
									l_bIsObserver = true;
								}
							}
						}
					}
					if (l_bIsObserver)
					{
						wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), l_sObserver, true, true);
						if (foundId.IsOk())
						{
							Delete(foundId);
							l_bIsObserver = false;
						}
					}

					if (l_bIsFun)
					{
						wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), l_sFun, true, true);
						if (foundId.IsOk())
						{
							Delete(foundId);
							l_bIsFun = false;
						}
					}
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_toBedel->first, true, true);

					if (foundId.IsOk())
					{
						Delete(foundId);
					}
				}
				m_mDeclarations2Dependencies.erase(it_setCon->first);
				wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_setCon->first, true, true);

				if (foundId.IsOk())
				{
					Delete(foundId);
				}
			}

		}


	}


}
void SP_WDG_DeclarationTreectrl::DeleteFunctions()
{
	bool l_bIsObserver = false;
	wxString l_sObserver;
	wxString l_sFunction;
	wxString l_sConst;
	bool l_bConst = false;

	set<wxString> l_setAllFun = m_unUsedDec.ReadAllDefinedFunctions();
	for (auto it_setFun = l_setAllFun.begin(); it_setFun != l_setAllFun.end(); ++it_setFun)
	{
		map<wxString, wxString> l_setTobeDeleted;
		auto it_foundFun = m_mDeclarations2Dependencies.find(*it_setFun);
		if (it_foundFun != m_mDeclarations2Dependencies.end())
		{
			l_setTobeDeleted = m_mDeclarations2Dependencies[*it_setFun];
			for (auto it_toBedel = l_setTobeDeleted.begin(); it_toBedel != l_setTobeDeleted.end(); ++it_toBedel)
			{
				if (it_toBedel->second == wxT("Constant"))
				{
					auto it_findItem = m_mDeclarations2Dependencies.find(it_toBedel->first);
					if (it_findItem != m_mDeclarations2Dependencies.end())
					{
						m_unUsedDec.DeleteUnusedConstant(it_toBedel->first);
						m_mDeclarations2Dependencies.erase(it_toBedel->first);
						for (auto it_mCon = m_vtrimeddecConstants.begin(); it_mCon != m_vtrimeddecConstants.end(); ++it_mCon)
						{
							if (it_mCon->second == it_toBedel->first)
							{
								l_sConst = it_mCon->first;
								l_bConst = true;

							}
						}
					}
				}

				if (it_toBedel->second == wxT("Observer"))
				{
					auto it_findItem = m_mDeclarations2Dependencies.find(it_toBedel->first);
					if (it_findItem != m_mDeclarations2Dependencies.end()) {
						m_unUsedDec.DeleteUnUsedObserver(it_toBedel->first);
						m_mDeclarations2Dependencies.erase(it_toBedel->first);
						for (auto it_m = m_vtrimeddecObservers.begin(); it_m != m_vtrimeddecObservers.end(); ++it_m)
						{
							if (it_toBedel->first == it_m->second)
							{
								l_sObserver = it_m->first;
								l_bIsObserver = true;
							}
						}
					}

				}
				if (l_bConst)
				{
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), l_sConst, true, true);

					if (foundId.IsOk())
					{
						Delete(foundId);
						l_bConst = false;
					}

				}
				if (l_bIsObserver)
				{
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), l_sObserver, true, true);

					if (foundId.IsOk())
					{
						Delete(foundId);
						l_bIsObserver = false;
					}
				}
				wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_toBedel->first, true, true);

				if (foundId.IsOk())
				{
					Delete(foundId);
				}

			}
			m_mDeclarations2Dependencies.erase(*it_setFun);
			wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *it_setFun, true, true);

			if (foundId.IsOk())
			{
				Delete(foundId);
			}

			for (auto it_m = m_vtrimeddecFunctions.begin(); it_m != m_vtrimeddecFunctions.end(); ++it_m)
			{
				if (*it_setFun == it_m->second)
				{
					l_sFunction = it_m->first;
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), l_sFunction, true, true);

					if (foundId.IsOk())
					{
						Delete(foundId);
					}
				}
			}
		}

	}

}

void SP_WDG_DeclarationTreectrl::DeleteObservers() {

	map<wxString, wxString> l_mAllObs = m_vtrimeddecObservers;

	for (auto it_mObserver = m_vtrimeddecObservers.begin(); it_mObserver != m_vtrimeddecObservers.end(); ++it_mObserver)
	{
		wxString l_sConst = it_mObserver->first;
		if (l_sConst.Contains("="))
		{
			l_sConst = l_sConst.BeforeFirst('=').Trim();
		}
		bool l_bIsFun = false;
		wxString l_sFun;
		bool l_bIsOb = false;
		wxString l_sObserver;
		map<wxString, wxString> l_setTobeDeleted;
		auto it_foundConstant = m_mDeclarations2Dependencies.find(l_sConst);
		if (it_foundConstant != m_mDeclarations2Dependencies.end())
		{
			l_setTobeDeleted = m_mDeclarations2Dependencies[l_sConst];
			for (auto it_toBedel = l_setTobeDeleted.begin(); it_toBedel != l_setTobeDeleted.end(); ++it_toBedel)
			{
				if (it_toBedel->second == wxT("Constant"))
				{
					auto it_findItem = m_mDeclarations2Dependencies.find(it_toBedel->first);
					if (it_findItem != m_mDeclarations2Dependencies.end())
					{
						m_unUsedDec.DeleteUnusedConstant(it_toBedel->first);
						m_mDeclarations2Dependencies.erase(it_mObserver->first);
					}

				}

				if (it_toBedel->second == wxT("Function"))
				{
					auto it_findItem = m_mDeclarations2Dependencies.find(it_toBedel->first);
					if (it_findItem != m_mDeclarations2Dependencies.end())
					{
						m_unUsedDec.DeleteUnusedFunction(it_toBedel->first);
						m_mDeclarations2Dependencies.erase(it_mObserver->first);
						for (auto it_m = m_vtrimeddecFunctions.begin(); it_m != m_vtrimeddecFunctions.end(); ++it_m)
						{
							if (it_m->second == it_toBedel->first)
							{
								l_sFun = it_m->first;
								l_bIsFun = true;

							}
						}
					}
				}
				if (it_toBedel->second == wxT("Observer"))
				{
					auto it_findItem = m_mDeclarations2Dependencies.find(it_toBedel->first);
					if (it_findItem != m_mDeclarations2Dependencies.end())
					{
						m_unUsedDec.DeleteUnUsedObserver(it_toBedel->first);
						m_mDeclarations2Dependencies.erase(it_mObserver->first);
						for (auto it_m = m_vtrimeddecObservers.begin(); it_m != m_vtrimeddecObservers.end(); ++it_m)
						{
							if (it_m->second == it_toBedel->first)
							{
								l_bIsOb = true;
								l_sObserver = it_m->first;
							}
						}
					}
				}

				if (l_bIsOb)
				{
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), l_sObserver, true, true);

					if (foundId.IsOk())
					{
						Delete(foundId);
						l_bIsOb = false;
					}
				}

				if (l_bIsFun)
				{
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), l_sFun, true, true);

					if (foundId.IsOk())
					{
						Delete(foundId);
						l_bIsFun = false;
					}
				}
				wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_toBedel->first, true, true);

				if (foundId.IsOk())
				{
					Delete(foundId);
				}
			}
			m_mDeclarations2Dependencies.erase(it_mObserver->first);
			wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_mObserver->first, true, true);

			if (foundId.IsOk())
			{
				Delete(foundId);
			}
		}

	}



}
void SP_WDG_DeclarationTreectrl::DeleteFunctionsForColPN()
{
	set<wxString> l_setAllFun = m_unUsedDec.ReadAllDefinedFunctions();
	for (auto it_setFun = l_setAllFun.begin(); it_setFun != l_setAllFun.end(); ++it_setFun)
	{
		map<wxString, wxString> l_setTobeDeleted;
		auto it_foundConstant = m_mDeclarations2Dependencies.find(*it_setFun);
		if (it_foundConstant != m_mDeclarations2Dependencies.end())
		{
			l_setTobeDeleted = m_mDeclarations2Dependencies[*it_setFun];
			for (auto it_toBedel = l_setTobeDeleted.begin(); it_toBedel != l_setTobeDeleted.end(); ++it_toBedel)
			{
				if (it_toBedel->second == wxT("Constant"))
				{
					//		m_unUsedDec.DeleteUnusedConstant(it_toBedel->first);
				}
				if (it_toBedel->second == wxT("Simple") | it_toBedel->second == wxT("Compound") | it_toBedel->second == wxT("Alias"))
				{
					//		m_unUsedDec.DeleteUnusedColorSet(it_toBedel->first);
				}
				if (it_toBedel->second == wxT("Function"))
				{
					m_unUsedDec.DeleteUnusedFunction(it_toBedel->first);
				}
				wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_toBedel->first, true, true);

				if (foundId.IsOk())
				{
					Delete(foundId);
				}

			}
			m_mDeclarations2Dependencies.erase(*it_setFun);
			wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *it_setFun, true, true);

			if (foundId.IsOk())
			{
				Delete(foundId);
			}
		}

	}
}
void SP_WDG_DeclarationTreectrl::DeleteVariablesForColPN()
{
	set<wxString> l_setAllVar = m_unUsedDec.ObtainAllDefinedVars();
	for (auto it_setVar = l_setAllVar.begin(); it_setVar != l_setAllVar.end(); ++it_setVar)
	{
		map<wxString, wxString> l_setTobeDeleted;
		auto it_foundConstant = m_mDeclarations2Dependencies.find(*it_setVar);
		if (it_foundConstant != m_mDeclarations2Dependencies.end())
		{//one var has been selected to be deleted
			l_setTobeDeleted = m_mDeclarations2Dependencies[*it_setVar];
			for (auto it_toBedel = l_setTobeDeleted.begin(); it_toBedel != l_setTobeDeleted.end(); ++it_toBedel)
			{
				if (it_toBedel->second == wxT("Variable"))
				{
					m_unUsedDec.DeleteUnusedVariable(it_toBedel->first);
				}


			}
			m_mDeclarations2Dependencies.erase(*it_setVar);
			wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *it_setVar, true, true);

			if (foundId.IsOk())
			{
				Delete(foundId);
			}
		}

	}
}

void SP_WDG_DeclarationTreectrl::DeleteConstantsForColPN()
{

	map<wxString, wxString> l_setAllConst = m_vtrimeddecConstants;

	for (auto it_setCon = m_vtrimeddecConstants.begin(); it_setCon != m_vtrimeddecConstants.end(); ++it_setCon)
	{
		wxString l_sConst = it_setCon->first;
		if (l_sConst.Contains(":"))
		{
			l_sConst = l_sConst.BeforeFirst(':').Trim();
		}

		map<wxString, wxString> l_setTobeDeleted;
		auto it_foundConstant = m_mDeclarations2Dependencies.find(l_sConst);
		if (it_foundConstant != m_mDeclarations2Dependencies.end())
		{
			l_setTobeDeleted = m_mDeclarations2Dependencies[l_sConst];
			for (auto it_toBedel = l_setTobeDeleted.begin(); it_toBedel != l_setTobeDeleted.end(); ++it_toBedel)
			{
				if (it_toBedel->second == wxT("Constant"))
				{
					auto it_findItem = m_mDeclarations2Dependencies.find(it_toBedel->first);//check if it is selected
					if (it_findItem != m_mDeclarations2Dependencies.end())
					{
						m_unUsedDec.DeleteUnusedConstant(it_toBedel->first);
						m_mDeclarations2Dependencies.erase(it_toBedel->first);
					}

				}
				if (it_toBedel->second == wxT("Simple") | it_toBedel->second == wxT("Compound") | it_toBedel->second == wxT("Alias"))
				{
					//auto it_findItem = m_mDeclarations2Dependencies.find(it_toBedel->first);//check if it is selected
					//	if (it_findItem != m_mDeclarations2Dependencies.end()) {
					m_unUsedDec.DeleteUnusedColorSet(it_toBedel->first);
					m_mDeclarations2Dependencies.erase(it_toBedel->first);
					//	}

				}
				if (it_toBedel->second == wxT("Function"))
				{
					auto it_findItem = m_mDeclarations2Dependencies.find(it_toBedel->first);//check if it is selected
					if (it_findItem != m_mDeclarations2Dependencies.end()) {
						m_unUsedDec.DeleteUnusedFunction(it_toBedel->first);
						m_mDeclarations2Dependencies.erase(it_toBedel->first);
					}

				}
				if (it_toBedel->second == wxT("Variable"))
				{
					auto it_findItem = m_mDeclarations2Dependencies.find(it_toBedel->first);//check if it is selected
					if (it_findItem != m_mDeclarations2Dependencies.end()) {
						m_unUsedDec.DeleteUnusedVariable(it_toBedel->first);
						m_mDeclarations2Dependencies.erase(it_toBedel->first);
					}

				}

				wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_toBedel->first, true, true);

				if (foundId.IsOk())
				{
					Delete(foundId);
				}
			}
			m_mDeclarations2Dependencies.erase(it_setCon->first);
			wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_setCon->first, true, true);

			if (foundId.IsOk())
			{
				Delete(foundId);
			}
		}

	}

}
void SP_WDG_DeclarationTreectrl::DeleteSimpleColorSet()
{
	map<wxString, wxString> l_mCs2Type = m_unUsedDec.ObtainAllCs2Type();
	for (auto it_mapCs = l_mCs2Type.begin(); it_mapCs != l_mCs2Type.end(); ++it_mapCs)
	{
		map<wxString, wxString> l_setTobeDeleted;
		auto it_foundConstant = m_mDeclarations2Dependencies.find(it_mapCs->first);
		if (it_foundConstant != m_mDeclarations2Dependencies.end() && it_mapCs->second == wxT("Simple"))
		{
			l_setTobeDeleted = m_mDeclarations2Dependencies[it_mapCs->first];
			for (auto it_toBedel = l_setTobeDeleted.begin(); it_toBedel != l_setTobeDeleted.end(); ++it_toBedel)
			{

				if (it_toBedel->second == wxT("Variable"))
				{

					m_unUsedDec.DeleteUnusedVariable(it_toBedel->first);
					m_mDeclarations2Dependencies.erase(it_toBedel->first);
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_toBedel->first, true, true);

					if (foundId.IsOk())
					{
						Delete(foundId);
					}

				}
				if (it_toBedel->second == wxT("Function"))
				{
					m_unUsedDec.DeleteUnusedFunction(it_toBedel->first);
					m_mDeclarations2Dependencies.erase(it_toBedel->first);
					//m_unUsedDec.DeleteUnusedVariable(it_toBedel->first);
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_toBedel->first, true, true);

					if (foundId.IsOk())
					{
						Delete(foundId);
					}

				}
				if (it_toBedel->second == wxT("Compound"))
				{
					m_unUsedDec.DeleteUnusedColorSet(it_toBedel->first);
					m_mDeclarations2Dependencies.erase(it_toBedel->first);
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_toBedel->first, true, true);

					if (foundId.IsOk())
					{
						Delete(foundId);
					}


				}

			}
			m_mDeclarations2Dependencies.erase(it_mapCs->first);
			wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_mapCs->first, true, true);

			if (foundId.IsOk())
			{
				Delete(foundId);
				m_unUsedDec.DeleteUnusedColorSet(it_mapCs->first);
			}
		}

	}
}
void SP_WDG_DeclarationTreectrl::DeleteCompoundColorSet()
{
	map<wxString, wxString> l_mCs2Type = m_unUsedDec.ObtainAllCs2Type();
	for (auto it_mapCs = l_mCs2Type.begin(); it_mapCs != l_mCs2Type.end(); ++it_mapCs)
	{
		if (it_mapCs->second == wxT("Compound"))
		{
			map<wxString, wxString> l_setTobeDeleted;
			auto it_foundConstant = m_mDeclarations2Dependencies.find(it_mapCs->first);
			if (it_foundConstant != m_mDeclarations2Dependencies.end() && it_mapCs->second == wxT("Compound"))
			{
				l_setTobeDeleted = m_mDeclarations2Dependencies[it_mapCs->first];
				for (auto it_toBedel = l_setTobeDeleted.begin(); it_toBedel != l_setTobeDeleted.end(); ++it_toBedel)
				{
					if (it_toBedel->second == wxT("Compound"))
					{
						m_unUsedDec.DeleteUnusedColorSet(it_toBedel->first);
						m_mDeclarations2Dependencies.erase(it_toBedel->first);
						wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_toBedel->first, true, true);

						if (foundId.IsOk())
						{
							Delete(foundId);
						}
					}
					if (it_toBedel->second == wxT("Simple"))
					{
						//	m_unUsedDec.DeleteUnusedColorSet(it_toBedel->first);
						//m_mDeclarations2Dependencies.erase(it_toBedel->first);
						for (auto it_mSimpleCS = m_vtrimeddecSimplCS.begin(); it_mSimpleCS != m_vtrimeddecSimplCS.end(); ++it_mSimpleCS)
						{
							if (it_toBedel->first == it_mSimpleCS->second)
							{

								wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_mSimpleCS->first, true, true);

								if (foundId.IsOk())
								{
									//		Delete(foundId);
									break;
								}
							}
						}
					}
					if (it_toBedel->second == wxT("Constant"))
					{
						//	m_unUsedDec.DeleteUnusedConstant(it_toBedel->first);
						//	m_mDeclarations2Dependencies.erase(it_toBedel->first);
						for (auto it_m = m_vtrimeddecConstants.begin(); it_m != m_vtrimeddecConstants.end(); it_m++)
						{
							if (it_m->second == it_toBedel->first)
							{
								wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_m->first, true, true);

								if (foundId.IsOk())
								{
									//			Delete(foundId);
									break;
								}
							}
						}

					}
					if (it_toBedel->second == wxT("Variable"))
					{
						m_unUsedDec.DeleteUnusedVariable(it_toBedel->first);
						m_mDeclarations2Dependencies.erase(it_toBedel->first);
						wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_toBedel->first, true, true);

						if (foundId.IsOk())
						{
							Delete(foundId);
							//	break;
						}
					}
					if (it_toBedel->second == wxT("Function"))
					{
						m_unUsedDec.DeleteUnusedFunction(it_toBedel->first);
						m_mDeclarations2Dependencies.erase(it_toBedel->first);
						wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_toBedel->first, true, true);

						if (foundId.IsOk())
						{
							Delete(foundId);
							//	break;
						}
					}
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_toBedel->first, true, true);

					if (foundId.IsOk())
					{
						//	Delete(foundId);
						//break;
					}
				}
				m_mDeclarations2Dependencies.erase(it_mapCs->first);


			}

		}

	}
}

void SP_WDG_DeclarationTreectrl::OnCleanUnusedItems(wxCommandEvent& p_cEvent)//Added by G.Assaf
{
	std::map<wxString, wxString> l_mDec;
	std::set<wxString> l_setTobeDeleted;
	bool l_bIsColored = false;

	wxString l_sNet = m_pcGraph->GetNetclass()->GetDisplayName();
	if (l_sNet.Contains(wxT("Colored")))
	{
		l_bIsColored = true;
	}

	if (l_bIsColored)
	{
		//colored PN
		if (m_sSelected == wxT("Constants"))
		{
			DeleteConstantsForColPN();

		}
		else if (m_sSelected == wxT("Functions"))
		{
			DeleteFunctionsForColPN();

		}

		else if (m_sSelected == wxT("Variables"))
		{
			DeleteVariablesForColPN();

		}
		else if (m_sSelected == wxT("Simple Color Sets"))
		{

			DeleteSimpleColorSet();

		}
		else if (m_sSelected == wxT("Compound Color Sets"))
		{
			//DeleteSimpleColorSet();
			DeleteCompoundColorSet();

		}
		else if (m_sSelected == wxT("Declarations")) {
			DeleteSimpleColorSet();
			DeleteCompoundColorSet();
			DeleteConstantsForColPN();
			DeleteFunctionsForColPN();
			DeleteVariablesForColPN();
		}
		else {
			//color sets
			DeleteSimpleColorSet();
			DeleteCompoundColorSet();
		}

	}
	else {
		//clean for un-colored pns

		if (m_sSelected == wxT("Constants"))
		{
			DeleteConstants();
		}
		else if (m_sSelected == wxT("Functions"))
		{
			DeleteFunctions();
		}
		else if (m_sSelected == wxT("Observers"))
		{
			DeleteObservers();
		}
		else {
			//all declarations
			DeleteConstants();
			DeleteFunctions();
			DeleteObservers();
		}

	}

}
void SP_WDG_DeclarationTreectrl::OnSelecting(wxCommandEvent& p_cEvent)
{
	bool l_bIsColoured = false;
	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();
	if (l_netType.Contains(wxT("Colored")))
	{
		l_bIsColoured = true;
	}

	if (l_bIsColoured)
	{

		//for coloured pn
		bool l_bIsfound = false;
		map<wxString, wxString> l_setDependenciesUnselected = ObtainItemDependenciesForSelect(m_sSelected, l_bIsfound);
		for (auto it_map = l_setDependenciesUnselected.begin(); it_map != l_setDependenciesUnselected.end(); ++it_map)
		{
			wxColour l_nColor1(wxT("Red"));
			wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_map->first, true, true);
			this->SetItemTextColour(foundId, l_nColor1);
			if (foundId.IsOk()) {
				wxTreeItemId l_treeItemParent = this->GetItemParent(foundId);
				if (l_treeItemParent.IsOk()) {
					wxString l_sParent = this->GetItemText(l_treeItemParent);

					auto l_setSelected = m_mGroup2SelectedSet[l_sParent];
					l_setSelected.insert(m_sSelected);
					m_mGroup2SelectedSet[l_sParent] = l_setSelected;
				}
			}
		}
		if (l_bIsfound)
		{
			if (m_sSelected.Contains(wxT(":")))
			{
				m_sSelected = m_sSelected.BeforeFirst(':');
			}
			m_mDeclarations2Dependencies[m_sSelected] = l_setDependenciesUnselected;
		}



	}
	else {


		/***************/
		bool l_bIsfound = false;
		map<wxString, wxString> l_setDependenciesUnselected = ObtainItemDependenciesForSelect(m_sSelected, l_bIsfound);
		map<wxString, wxString> l_setDependenciesUnselectednew;
		for (auto it_m = l_setDependenciesUnselected.begin(); it_m != l_setDependenciesUnselected.end(); ++it_m)
		{
			for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
			{
				if (it_m->first == itmap->second)
				{
					l_setDependenciesUnselectednew[itmap->first] = wxT("Function");

				}
			}
		}
		for (auto it_m = l_setDependenciesUnselected.begin(); it_m != l_setDependenciesUnselected.end(); ++it_m)
		{
			for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
			{
				if (it_m->first == itmap->first)
				{
					l_setDependenciesUnselectednew[itmap->first] = wxT("Constant");

				}
			}
		}
		for (auto it_m = l_setDependenciesUnselected.begin(); it_m != l_setDependenciesUnselected.end(); ++it_m)
		{
			for (auto itmap = m_vtrimeddecObservers.begin(); itmap != m_vtrimeddecObservers.end(); ++itmap)
			{
				if (it_m->first == itmap->second)
				{
					l_setDependenciesUnselectednew[itmap->first] = wxT("Observer");

				}
			}
		}

		for (auto it_map = l_setDependenciesUnselectednew.begin(); it_map != l_setDependenciesUnselectednew.end(); ++it_map)
		{
			wxColour l_nColor1(wxT("Red"));
			wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_map->first, true, true);
			this->SetItemTextColour(foundId, l_nColor1);
			if (foundId.IsOk()) {
				wxTreeItemId l_treeItemParent = this->GetItemParent(foundId);
				if (l_treeItemParent.IsOk()) {
					wxString l_sParent = this->GetItemText(l_treeItemParent);

					auto l_setSelected = m_mGroup2SelectedSet[l_sParent];
					l_setSelected.insert(m_sSelected);
					m_mGroup2SelectedSet[l_sParent] = l_setSelected;
				}
			}
		}
		if (l_bIsfound)
		{
			if (m_sSelected.Contains(wxT(":")))
			{
				m_sSelected = m_sSelected.BeforeFirst(':');
			}
			else if (m_sSelected.Contains(wxT("(")))
			{
				m_sSelected = m_sSelected.BeforeFirst('(');
			}
			else if (m_sSelected.Contains(wxT("=")))
			{
				m_sSelected = m_sSelected.BeforeFirst('=').Trim();
			}
			m_mDeclarations2Dependencies[m_sSelected] = l_setDependenciesUnselected;
		}



		/**********/

	}

}

void  SP_WDG_DeclarationTreectrl::OnSelectAll(wxCommandEvent& p_cEvent)
{
	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();
	bool l_bIsColoured = false;
	wxString l_sGroup = m_sSelected;
	std::set<wxString> l_setUnusedDec;
	std::map<wxString, wxString> l_mTrimedDec;
	if (l_netType.Contains(wxT("Colored")))
	{
		l_bIsColoured = true;
	}

	if (!l_bIsColoured)
	{
		if (l_sGroup == wxT("Constants"))
		{

			set<wxString> l_setConstants = m_unUsedDec.ObtainAllDefinedConstants();

			for (auto it_set = l_setConstants.begin(); it_set != l_setConstants.end(); ++it_set)
			{
				map<wxString, wxString> l_setDependenciesUnselectednew;
				bool l_bIsFound = false;
				map<wxString, wxString>  l_setDependenciesSelected = ObtainItemDependenciesForSelect(*it_set, l_bIsFound);


				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
					{
						if (it_m->first == itmap->second)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Function");

						}
					}
				}
				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
					{
						if (it_m->first == itmap->first)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Constant");

						}
					}
				}
				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecObservers.begin(); itmap != m_vtrimeddecObservers.end(); ++itmap)
					{
						if (it_m->first == itmap->first)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Observer");

						}
					}
				}

				for (auto it_map = l_setDependenciesUnselectednew.begin(); it_map != l_setDependenciesUnselectednew.end(); ++it_map)
				{
					wxColour l_nColor1(wxT("Red"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_map->first, true, true);
					this->SetItemTextColour(foundId, l_nColor1);
					if (foundId.IsOk()) {
						wxTreeItemId l_treeItemParent = this->GetItemParent(foundId);
						if (l_treeItemParent.IsOk()) {
							wxString l_sParent = this->GetItemText(l_treeItemParent);

							auto l_setSelected = m_mGroup2SelectedSet[l_sParent];
							l_setSelected.insert(m_sSelected);
							m_mGroup2SelectedSet[l_sParent] = l_setSelected;
						}
					}
				}
				if (l_bIsFound)
				{
					if (m_sSelected.Contains(wxT(":")))
					{
						m_sSelected = m_sSelected.BeforeFirst(':');
					}
					else if (m_sSelected.Contains(wxT("(")))
					{
						m_sSelected = m_sSelected.BeforeFirst('(');
					}
					else if (m_sSelected.Contains(wxT("=")))
					{
						m_sSelected = m_sSelected.BeforeFirst('=').Trim();
					}
					m_mDeclarations2Dependencies[m_sSelected] = l_setDependenciesSelected;
				}

			}


			//////
		}
		else if (l_sGroup == wxT("Functions"))
		{

			set<wxString> l_setFunctions = m_unUsedDec.ReadAllDefinedFunctions();

			for (auto it_setFun = l_setFunctions.begin(); it_setFun != l_setFunctions.end(); ++it_setFun)
			{
				map<wxString, wxString> l_setDependenciesUnselectednew;
				bool l_bIsFound = false;
				map<wxString, wxString>  l_setDependenciesSelected = ObtainItemDependenciesForSelect(*it_setFun, l_bIsFound);


				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
					{
						if (it_m->first == itmap->second)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Function");

						}
					}
				}
				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
					{
						if (it_m->first == itmap->first)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Constant");

						}
					}
				}
				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecObservers.begin(); itmap != m_vtrimeddecObservers.end(); ++itmap)
					{
						if (it_m->first == itmap->first)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Observer");

						}
					}
				}

				for (auto it_map = l_setDependenciesUnselectednew.begin(); it_map != l_setDependenciesUnselectednew.end(); ++it_map)
				{
					wxColour l_nColor1(wxT("Red"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_map->first, true, true);
					this->SetItemTextColour(foundId, l_nColor1);
					if (foundId.IsOk()) {
						wxTreeItemId l_treeItemParent = this->GetItemParent(foundId);
						if (l_treeItemParent.IsOk()) {
							wxString l_sParent = this->GetItemText(l_treeItemParent);

							auto l_setSelected = m_mGroup2SelectedSet[l_sParent];
							l_setSelected.insert(m_sSelected);
							m_mGroup2SelectedSet[l_sParent] = l_setSelected;
						}
					}
				}
				if (l_bIsFound)
				{
					if (m_sSelected.Contains(wxT(":")))
					{
						m_sSelected = m_sSelected.BeforeFirst(':');
					}
					else if (m_sSelected.Contains(wxT("(")))
					{
						m_sSelected = m_sSelected.BeforeFirst('(');
					}
					else if (m_sSelected.Contains(wxT("=")))
					{
						m_sSelected = m_sSelected.BeforeFirst('=').Trim();
					}
					m_mDeclarations2Dependencies[m_sSelected] = l_setDependenciesSelected;
				}

			}

			/******/
		}
		else if (l_sGroup == wxT("Observers")) {
			//observers

			map<wxString, wxString> l_mapObservers = m_vtrimeddecObservers;


			for (auto it_setMap = l_mapObservers.begin(); it_setMap != l_mapObservers.end(); ++it_setMap)
			{
				map<wxString, wxString> l_setDependenciesUnselectednew;
				bool l_bIsFound = false;
				map<wxString, wxString>  l_setDependenciesSelected = ObtainItemDependenciesForSelect(it_setMap->second, l_bIsFound);


				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
					{
						if (it_m->first == itmap->second)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Function");

						}
					}
				}
				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
					{
						if (it_m->first == itmap->first)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Constant");

						}
					}
				}
				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecObservers.begin(); itmap != m_vtrimeddecObservers.end(); ++itmap)
					{
						if (it_m->first == itmap->first)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Observer");

						}
					}
				}

				for (auto it_map = l_setDependenciesUnselectednew.begin(); it_map != l_setDependenciesUnselectednew.end(); ++it_map)
				{
					wxColour l_nColor1(wxT("Red"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_map->first, true, true);
					this->SetItemTextColour(foundId, l_nColor1);
					if (foundId.IsOk()) {
						wxTreeItemId l_treeItemParent = this->GetItemParent(foundId);
						if (l_treeItemParent.IsOk()) {
							wxString l_sParent = this->GetItemText(l_treeItemParent);

							auto l_setSelected = m_mGroup2SelectedSet[l_sParent];
							l_setSelected.insert(m_sSelected);
							m_mGroup2SelectedSet[l_sParent] = l_setSelected;
						}
					}
				}
				if (l_bIsFound)
				{
					if (m_sSelected.Contains(wxT(":")))
					{
						m_sSelected = m_sSelected.BeforeFirst(':');
					}
					else if (m_sSelected.Contains(wxT("(")))
					{
						m_sSelected = m_sSelected.BeforeFirst('(');
					}
					else if (m_sSelected.Contains(wxT("=")))
					{
						m_sSelected = m_sSelected.BeforeFirst('=').Trim();
					}
					m_mDeclarations2Dependencies[m_sSelected] = l_setDependenciesSelected;
				}

			}
			/********/
		}
		else {
			//all
			//l_setUnusedDec = m_vUnUsedDecList;
			//l_mTrimedDec = m_vtrimeddecAll;
			/********************/

			set<wxString> l_setConstants = m_unUsedDec.ObtainAllDefinedConstants();

			for (auto it_set = l_setConstants.begin(); it_set != l_setConstants.end(); ++it_set)
			{
				map<wxString, wxString> l_setDependenciesUnselectednew;
				bool l_bIsFound = false;
				map<wxString, wxString>  l_setDependenciesSelected = ObtainItemDependenciesForSelect(*it_set, l_bIsFound);


				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
					{
						if (it_m->first == itmap->second)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Function");

						}
					}
				}
				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
					{
						if (it_m->first == itmap->first)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Constant");

						}
					}
				}
				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecObservers.begin(); itmap != m_vtrimeddecObservers.end(); ++itmap)
					{
						if (it_m->first == itmap->first)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Observer");

						}
					}
				}

				for (auto it_map = l_setDependenciesUnselectednew.begin(); it_map != l_setDependenciesUnselectednew.end(); ++it_map)
				{
					wxColour l_nColor1(wxT("Red"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_map->first, true, true);
					this->SetItemTextColour(foundId, l_nColor1);
					if (foundId.IsOk()) {
						wxTreeItemId l_treeItemParent = this->GetItemParent(foundId);
						if (l_treeItemParent.IsOk()) {
							wxString l_sParent = this->GetItemText(l_treeItemParent);

							auto l_setSelected = m_mGroup2SelectedSet[l_sParent];
							l_setSelected.insert(m_sSelected);
							m_mGroup2SelectedSet[l_sParent] = l_setSelected;
						}
					}
				}
				if (l_bIsFound)
				{
					if (m_sSelected.Contains(wxT(":")))
					{
						m_sSelected = m_sSelected.BeforeFirst(':');
					}
					else if (m_sSelected.Contains(wxT("(")))
					{
						m_sSelected = m_sSelected.BeforeFirst('(');
					}
					else if (m_sSelected.Contains(wxT("=")))
					{
						m_sSelected = m_sSelected.BeforeFirst('=').Trim();
					}
					m_mDeclarations2Dependencies[m_sSelected] = l_setDependenciesSelected;
				}

			}

			/***********funs********/

			set<wxString> l_setFunctions = m_unUsedDec.ReadAllDefinedFunctions();

			for (auto it_setFun = l_setFunctions.begin(); it_setFun != l_setFunctions.end(); ++it_setFun)
			{
				map<wxString, wxString> l_setDependenciesUnselectednew;
				bool l_bIsFound = false;
				map<wxString, wxString>  l_setDependenciesSelected = ObtainItemDependenciesForSelect(*it_setFun, l_bIsFound);


				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
					{
						if (it_m->first == itmap->second)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Function");

						}
					}
				}
				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
					{
						if (it_m->first == itmap->first)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Constant");

						}
					}
				}
				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecObservers.begin(); itmap != m_vtrimeddecObservers.end(); ++itmap)
					{
						if (it_m->first == itmap->first)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Observer");

						}
					}
				}

				for (auto it_map = l_setDependenciesUnselectednew.begin(); it_map != l_setDependenciesUnselectednew.end(); ++it_map)
				{
					wxColour l_nColor1(wxT("Red"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_map->first, true, true);
					this->SetItemTextColour(foundId, l_nColor1);
					if (foundId.IsOk()) {
						wxTreeItemId l_treeItemParent = this->GetItemParent(foundId);
						if (l_treeItemParent.IsOk()) {
							wxString l_sParent = this->GetItemText(l_treeItemParent);

							auto l_setSelected = m_mGroup2SelectedSet[l_sParent];
							l_setSelected.insert(m_sSelected);
							m_mGroup2SelectedSet[l_sParent] = l_setSelected;
						}
					}
				}
				if (l_bIsFound)
				{
					if (m_sSelected.Contains(wxT(":")))
					{
						m_sSelected = m_sSelected.BeforeFirst(':');
					}
					else if (m_sSelected.Contains(wxT("(")))
					{
						m_sSelected = m_sSelected.BeforeFirst('(');
					}
					else if (m_sSelected.Contains(wxT("=")))
					{
						m_sSelected = m_sSelected.BeforeFirst('=').Trim();
					}
					m_mDeclarations2Dependencies[m_sSelected] = l_setDependenciesSelected;
				}

			}


			/*********observers************/

			map<wxString, wxString> l_mapObservers = m_vtrimeddecObservers;


			for (auto it_setMap = l_mapObservers.begin(); it_setMap != l_mapObservers.end(); ++it_setMap)
			{
				map<wxString, wxString> l_setDependenciesUnselectednew;
				bool l_bIsFound = false;
				map<wxString, wxString>  l_setDependenciesSelected = ObtainItemDependenciesForSelect(it_setMap->second, l_bIsFound);


				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecFunctions.begin(); itmap != m_vtrimeddecFunctions.end(); ++itmap)
					{
						if (it_m->first == itmap->second)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Function");

						}
					}
				}
				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecConstants.begin(); itmap != m_vtrimeddecConstants.end(); ++itmap)
					{
						if (it_m->first == itmap->first)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Constant");

						}
					}
				}
				for (auto it_m = l_setDependenciesSelected.begin(); it_m != l_setDependenciesSelected.end(); ++it_m)
				{
					for (auto itmap = m_vtrimeddecObservers.begin(); itmap != m_vtrimeddecObservers.end(); ++itmap)
					{
						if (it_m->first == itmap->first)
						{
							l_setDependenciesUnselectednew[itmap->first] = wxT("Observer");

						}
					}
				}

				for (auto it_map = l_setDependenciesUnselectednew.begin(); it_map != l_setDependenciesUnselectednew.end(); ++it_map)
				{
					wxColour l_nColor1(wxT("Red"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_map->first, true, true);
					this->SetItemTextColour(foundId, l_nColor1);
					if (foundId.IsOk()) {
						wxTreeItemId l_treeItemParent = this->GetItemParent(foundId);
						if (l_treeItemParent.IsOk()) {
							wxString l_sParent = this->GetItemText(l_treeItemParent);

							auto l_setSelected = m_mGroup2SelectedSet[l_sParent];
							l_setSelected.insert(m_sSelected);
							m_mGroup2SelectedSet[l_sParent] = l_setSelected;
						}
					}
				}
				if (l_bIsFound)
				{
					if (m_sSelected.Contains(wxT(":")))
					{
						m_sSelected = m_sSelected.BeforeFirst(':');
					}
					else if (m_sSelected.Contains(wxT("(")))
					{
						m_sSelected = m_sSelected.BeforeFirst('(');
					}
					else if (m_sSelected.Contains(wxT("=")))
					{
						m_sSelected = m_sSelected.BeforeFirst('=').Trim();
					}
					m_mDeclarations2Dependencies[m_sSelected] = l_setDependenciesSelected;
				}

			}


			/*********************/
		}

	}
	else {

		bool l_bIsFound = false;
		if (l_sGroup == wxT("Constants"))
		{

			set<wxString> l_setConstants = m_unUsedDec.ObtainAllDefinedConstants();
			map<wxString, wxString> l_setDependenciesSelected;
			for (auto it_setConstants = l_setConstants.begin(); it_setConstants != l_setConstants.end(); ++it_setConstants)
			{
				l_setDependenciesSelected = ObtainItemDependenciesForSelect(*it_setConstants, l_bIsFound);
				for (auto it_map = l_setDependenciesSelected.begin(); it_map != l_setDependenciesSelected.end(); ++it_map)
				{
					wxColour l_nColor1(wxT("Red"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_map->first, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);
						l_pcItem2[1]->Enable(true);
						l_pcItem2[2]->Enable(false);

					}
				}
				if (l_bIsFound)
				{
					m_mDeclarations2Dependencies[*it_setConstants] = l_setDependenciesSelected;

				}


			}

		}
		if (l_sGroup == wxT("Variables"))
		{

			set<wxString> l_setVar = m_unUsedDec.ObtainAllDefinedVars();
			for (auto it_setVar = l_setVar.begin(); it_setVar != l_setVar.end(); ++it_setVar)
			{
				map<wxString, wxString>  l_setDependenciesSelected = ObtainItemDependenciesForSelect(*it_setVar, l_bIsFound);
				for (auto itset = l_setDependenciesSelected.begin(); itset != l_setDependenciesSelected.end(); ++itset)
				{
					wxColour l_nColor1(wxT("Red"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), itset->first, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);
						l_pcItem2[1]->Enable(true);
						l_pcItem2[2]->Enable(false);

					}
				}
				if (l_bIsFound)
				{
					m_mDeclarations2Dependencies[*it_setVar] = l_setDependenciesSelected;
				}


			}

		}
		else if (l_sGroup == wxT("Functions"))
		{
			set<wxString> l_setFunctions = m_unUsedDec.ReadAllDefinedFunctions();
			for (auto it_setFun = l_setFunctions.begin(); it_setFun != l_setFunctions.end(); ++it_setFun)
			{
				map<wxString, wxString> l_setDependenciesSelected = ObtainItemDependenciesForSelect(*it_setFun, l_bIsFound);
				for (auto itset = l_setDependenciesSelected.begin(); itset != l_setDependenciesSelected.end(); ++itset)
				{
					wxColour l_nColor1(wxT("Red"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), itset->first, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);
						l_pcItem2[1]->Enable(true);
						l_pcItem2[2]->Enable(false);

					}
				}
				if (l_bIsFound)
				{
					m_mDeclarations2Dependencies[*it_setFun] = l_setDependenciesSelected;
				}

			}
		}
		else if (l_sGroup == wxT("Simple Color Sets"))
		{
			map<wxString, wxString> l_setAllcs = m_unUsedDec.ObtainAllCs2Type();

			for (auto it_mapCS = l_setAllcs.begin(); it_mapCS != l_setAllcs.end(); ++it_mapCS)
			{
				map<wxString, wxString> l_setDependenciesSelected;
				if (it_mapCS->second == wxT("Simple"))
				{
					l_setDependenciesSelected = ObtainItemDependenciesForSelect(it_mapCS->first, l_bIsFound);
					for (auto itset = l_setDependenciesSelected.begin(); itset != l_setDependenciesSelected.end(); ++itset)
					{
						wxColour l_nColor1(wxT("Red"));
						wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), itset->first, true, true);
						if (foundId.IsOk())
						{
							this->SetItemTextColour(foundId, l_nColor1);
							l_pcItem2[1]->Enable(true);
							l_pcItem2[2]->Enable(false);


						}
					}
					if (l_bIsFound)
					{
						m_mDeclarations2Dependencies[it_mapCS->first] = l_setDependenciesSelected;
					}


				}

			}

		}

		else if (l_sGroup == wxT("Compound Color Sets"))
		{
			map<wxString, wxString> l_setAllcs = m_unUsedDec.ObtainAllCs2Type();

			for (auto it_mapCS = l_setAllcs.begin(); it_mapCS != l_setAllcs.end(); ++it_mapCS)
			{
				map<wxString, wxString> l_setDependenciesSelected;
				if (it_mapCS->second == wxT("Compound"))
				{
					l_setDependenciesSelected = ObtainItemDependenciesForSelect(it_mapCS->first, l_bIsFound);
					for (auto itset = l_setDependenciesSelected.begin(); itset != l_setDependenciesSelected.end(); ++itset)
					{
						wxColour l_nColor1(wxT("Red"));
						wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), itset->first, true, true);
						if (foundId.IsOk())
						{
							this->SetItemTextColour(foundId, l_nColor1);
							l_pcItem2[1]->Enable(true);
							l_pcItem2[2]->Enable(false);


						}
					}
					if (l_bIsFound)
					{
						m_mDeclarations2Dependencies[it_mapCS->first] = l_setDependenciesSelected;
					}

				}

			}

		}
		else if (l_sGroup == wxT("Alias Color Sets"))
		{
			map<wxString, wxString> l_setAllcs = m_unUsedDec.ObtainAllCs2Type();

			for (auto it_mapCS = l_setAllcs.begin(); it_mapCS != l_setAllcs.end(); ++it_mapCS)
			{
				map<wxString, wxString> l_setDependenciesSelected;
				if (it_mapCS->second == wxT("Alias"))
				{
					l_setDependenciesSelected = ObtainItemDependenciesForSelect(it_mapCS->first, l_bIsFound);
					for (auto itset = l_setDependenciesSelected.begin(); itset != l_setDependenciesSelected.end(); ++itset)
					{
						wxColour l_nColor1(wxT("Red"));
						wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), itset->first, true, true);
						if (foundId.IsOk())
						{
							this->SetItemTextColour(foundId, l_nColor1);
							l_pcItem2[1]->Enable(true);
							l_pcItem2[2]->Enable(false);


						}
					}
					if (l_bIsFound) {
						m_mDeclarations2Dependencies[it_mapCS->first] = l_setDependenciesSelected;
					}
				}

			}

		}
		else if (l_sGroup == wxT("Color Sets"))
		{
			map<wxString, wxString> l_setAllcs = m_unUsedDec.ObtainAllCs2Type();

			for (auto it_mapCS = l_setAllcs.begin(); it_mapCS != l_setAllcs.end(); ++it_mapCS)
			{

				map<wxString, wxString> l_setDependenciesSelected = ObtainItemDependenciesForSelect(it_mapCS->first, l_bIsFound);
				for (auto itset = l_setDependenciesSelected.begin(); itset != l_setDependenciesSelected.end(); ++itset)
				{
					wxColour l_nColor1(wxT("Red"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), itset->first, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);
						l_pcItem2[1]->Enable(true);
						l_pcItem2[2]->Enable(false);


					}
				}
				if (l_bIsFound)
				{
					m_mDeclarations2Dependencies[it_mapCS->first] = l_setDependenciesSelected;
				}
			}

		}

		else if (l_sGroup == wxT("Declarations"))
		{
			map<wxString, wxString> l_setAllcs = m_unUsedDec.ObtainAllCs2Type();

			for (auto it_mapCS = l_setAllcs.begin(); it_mapCS != l_setAllcs.end(); ++it_mapCS)
			{

				map<wxString, wxString> l_setDependenciesSelected = ObtainItemDependenciesForSelect(it_mapCS->first, l_bIsFound);
				for (auto itset = l_setDependenciesSelected.begin(); itset != l_setDependenciesSelected.end(); ++itset)
				{
					wxColour l_nColor1(wxT("Red"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), itset->first, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);
						l_pcItem2[1]->Enable(true);
						l_pcItem2[2]->Enable(false);


					}
				}
				if (l_bIsFound) {
					m_mDeclarations2Dependencies[it_mapCS->first] = l_setDependenciesSelected;
				}
			}
			//constants

			set<wxString> l_setConstants = m_unUsedDec.ObtainAllDefinedConstants();
			for (auto it_setConstants = l_setConstants.begin(); it_setConstants != l_setConstants.end(); ++it_setConstants)
			{
				map<wxString, wxString> l_setDependenciesSelected = ObtainItemDependenciesForSelect(*it_setConstants, l_bIsFound);
				for (auto itset = l_setDependenciesSelected.begin(); itset != l_setDependenciesSelected.end(); ++itset)
				{
					wxColour l_nColor1(wxT("Red"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), itset->first, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);
						l_pcItem2[1]->Enable(true);
						l_pcItem2[2]->Enable(false);

					}
				}
				if (l_bIsFound) {
					m_mDeclarations2Dependencies[*it_setConstants] = l_setDependenciesSelected;
				}


			}
			//functions
			set<wxString> l_setFunctions = m_unUsedDec.ReadAllDefinedFunctions();
			for (auto it_setFun = l_setFunctions.begin(); it_setFun != l_setFunctions.end(); ++it_setFun)
			{
				map<wxString, wxString> l_setDependenciesSelected = ObtainItemDependenciesForSelect(*it_setFun, l_bIsFound);
				for (auto itset = l_setDependenciesSelected.begin(); itset != l_setDependenciesSelected.end(); ++itset)
				{
					wxColour l_nColor1(wxT("Red"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), itset->first, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);
						l_pcItem2[1]->Enable(true);
						l_pcItem2[2]->Enable(false);

					}
				}
				if (l_bIsFound)
				{
					m_mDeclarations2Dependencies[*it_setFun] = l_setDependenciesSelected;
				}

			}
			//vars
			set<wxString> l_setVar = m_unUsedDec.ObtainAllDefinedVars();
			for (auto it_setVar = l_setVar.begin(); it_setVar != l_setVar.end(); ++it_setVar)
			{
				map<wxString, wxString> l_setDependenciesSelected = ObtainItemDependenciesForSelect(*it_setVar, l_bIsFound);
				for (auto itset = l_setDependenciesSelected.begin(); itset != l_setDependenciesSelected.end(); ++itset)
				{
					wxColour l_nColor1(wxT("Red"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), itset->first, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);
						l_pcItem2[1]->Enable(true);
						l_pcItem2[2]->Enable(false);

					}
				}
				if (l_bIsFound)
				{
					m_mDeclarations2Dependencies[*it_setVar] = l_setDependenciesSelected;
				}

			}
			//color sets
			map<wxString, wxString> l_setAllcs1 = m_unUsedDec.ObtainAllCs2Type();

			for (auto it_mapCS1 = l_setAllcs1.begin(); it_mapCS1 != l_setAllcs1.end(); ++it_mapCS1)
			{

				map<wxString, wxString> l_setDependenciesSelected = ObtainItemDependenciesForSelect(it_mapCS1->first, l_bIsFound);
				for (auto itset = l_setDependenciesSelected.begin(); itset != l_setDependenciesSelected.end(); ++itset)
				{
					wxColour l_nColor1(wxT("Red"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), itset->first, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);
						l_pcItem2[1]->Enable(true);
						l_pcItem2[2]->Enable(false);


					}
				}
				if (l_bIsFound)
				{
					m_mDeclarations2Dependencies[it_mapCS1->first] = l_setDependenciesSelected;
				}

			}



		}

	}
}

void  SP_WDG_DeclarationTreectrl::OnUnSelectAll(wxCommandEvent& p_cEvent)
{
	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();
	bool l_bIsChanged = false;
	bool l_bIsColoured = false;
	wxString l_sGroup = m_sSelected;
	std::set<wxString> l_setSelectedGroup;

	if (l_netType.Contains(wxT("Colored")))
	{
		l_bIsColoured = true;
	}

	if (!l_bIsColoured)
	{
		//unselect constatns
		if (m_sSelected == wxT("Constants"))
		{
			for (auto it_con = m_vtrimeddecConstants.begin(); it_con != m_vtrimeddecConstants.end(); ++it_con)
			{
				set<wxString>	l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(it_con->first);
				for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
				{
					wxColour l_nColor1(wxT("Blue"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_con->first, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);

						l_pcItem2[1]->Enable(false);
						l_pcItem2[2]->Enable(true);

					}
				}
				for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
				{
					auto foundItem = l_setDependenciesUnselected.find(it_map->first);
					if (foundItem != l_setDependenciesUnselected.end())
					{
						m_mDeclarations2Dependencies.erase(it_map->second);

					}


				}
			}

		}
		else if (m_sSelected == wxT("Functions"))
		{

			for (auto it_fun = m_vtrimeddecFunctions.begin(); it_fun != m_vtrimeddecFunctions.end(); ++it_fun)
			{
				set<wxString>	l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(it_fun->first);
				for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
				{
					wxColour l_nColor1(wxT("Blue"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);

						l_pcItem2[1]->Enable(false);
						l_pcItem2[2]->Enable(true);

					}
				}
				for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
				{
					auto foundItem = l_setDependenciesUnselected.find(it_map->first);
					if (foundItem != l_setDependenciesUnselected.end())
					{
						m_mDeclarations2Dependencies.erase(it_map->second);

					}


				}
			}







		}
		else if (m_sSelected == wxT("Observers"))
		{
			for (auto it_obs = m_vtrimeddecObservers.begin(); it_obs != m_vtrimeddecObservers.end(); ++it_obs)
			{
				set<wxString>	l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(it_obs->first);
				for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
				{
					wxColour l_nColor1(wxT("Blue"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);

						l_pcItem2[1]->Enable(false);
						l_pcItem2[2]->Enable(true);

					}
				}
				for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
				{
					auto foundItem = l_setDependenciesUnselected.find(it_map->first);
					if (foundItem != l_setDependenciesUnselected.end())
					{
						m_mDeclarations2Dependencies.erase(it_map->second);

					}


				}
			}
		}
		else
		{
			if (m_sSelected == wxT("Declarations"))
			{
				/*constants*/
				for (auto it_con = m_vtrimeddecConstants.begin(); it_con != m_vtrimeddecConstants.end(); ++it_con)
				{
					set<wxString>	l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(it_con->first);
					for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
					{
						wxColour l_nColor1(wxT("Blue"));
						wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_con->first, true, true);
						if (foundId.IsOk())
						{
							this->SetItemTextColour(foundId, l_nColor1);

							l_pcItem2[1]->Enable(false);
							l_pcItem2[2]->Enable(true);

						}
					}
					for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
					{
						auto foundItem = l_setDependenciesUnselected.find(it_map->first);
						if (foundItem != l_setDependenciesUnselected.end())
						{
							m_mDeclarations2Dependencies.erase(it_map->second);

						}


					}
				}
				/*functions*/
				for (auto it_fun = m_vtrimeddecFunctions.begin(); it_fun != m_vtrimeddecFunctions.end(); ++it_fun)
				{
					set<wxString>	l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(it_fun->first);
					for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
					{
						wxColour l_nColor1(wxT("Blue"));
						wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
						if (foundId.IsOk())
						{
							this->SetItemTextColour(foundId, l_nColor1);

							l_pcItem2[1]->Enable(false);
							l_pcItem2[2]->Enable(true);

						}
					}
					for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
					{
						auto foundItem = l_setDependenciesUnselected.find(it_map->first);
						if (foundItem != l_setDependenciesUnselected.end())
						{
							m_mDeclarations2Dependencies.erase(it_map->second);

						}


					}
				}
				/*observers*/

				for (auto it_obs = m_vtrimeddecObservers.begin(); it_obs != m_vtrimeddecObservers.end(); ++it_obs)
				{
					set<wxString>	l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(it_obs->first);
					for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
					{
						wxColour l_nColor1(wxT("Blue"));
						wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
						if (foundId.IsOk())
						{
							this->SetItemTextColour(foundId, l_nColor1);

							l_pcItem2[1]->Enable(false);
							l_pcItem2[2]->Enable(true);

						}
					}
					for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
					{
						auto foundItem = l_setDependenciesUnselected.find(it_map->first);
						if (foundItem != l_setDependenciesUnselected.end())
						{
							m_mDeclarations2Dependencies.erase(it_map->second);

						}


					}
				}




			}
		}


	}
	else {
		m_bisSecond = true;
		//colored class
		//do unselect all on Constants
		l_pcItem2[1]->Enable(false);
		l_pcItem2[2]->Enable(true);
		if (l_sGroup == wxT("Constants"))
		{

			set<wxString> l_setConstants = m_unUsedDec.ObtainAllDefinedConstants();
			for (auto it_setConstants = l_setConstants.begin(); it_setConstants != l_setConstants.end(); ++it_setConstants)
			{
				set<wxString> l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(*it_setConstants);
				for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
				{
					wxColour l_nColor1(wxT("Blue"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);
						l_pcItem2[1]->Enable(false);
						l_pcItem2[2]->Enable(true);

					}
				}
				for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
				{
					auto foundItem = l_setDependenciesUnselected.find(it_map->first);
					if (foundItem != l_setDependenciesUnselected.end())
					{
						m_mDeclarations2Dependencies.erase(it_map->second);

					}

				}
				//auto foundIt = m_mDeclarations2Dependencies.find(*it_setConstants);
				//if (foundIt != m_mDeclarations2Dependencies.end())
				//{
				//	m_mDeclarations2Dependencies.erase(foundIt);
				//}
			}

		}
		else if (l_sGroup == wxT("Variables"))
		{//do unselect all only on variables

			set<wxString> l_setAllVarList = m_unUsedDec.ObtainAllDefinedVars();

			for (auto it_setVar = l_setAllVarList.begin(); it_setVar != l_setAllVarList.end(); ++it_setVar)
			{
				set<wxString> l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(*it_setVar);
				for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
				{
					wxColour l_nColor1(wxT("Blue"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);
						l_pcItem2[1]->Enable(false);
						l_pcItem2[2]->Enable(true);


					}
				}


				for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
				{
					auto foundItem = l_setDependenciesUnselected.find(it_map->first);
					if (foundItem != l_setDependenciesUnselected.end())
					{
						m_mDeclarations2Dependencies.erase(it_map->second);

					}

				}
				//auto foundIt = m_mDeclarations2Dependencies.find(*it_setVar);
				//if (foundIt != m_mDeclarations2Dependencies.end())
				//{
				//	m_mDeclarations2Dependencies.erase(foundIt);
				//}

			}

		}
		else if (l_sGroup == wxT("Functions"))
		{//do unselect all only on functions

			set<wxString> l_setAllFuns = m_unUsedDec.ReadAllDefinedFunctions();

			for (auto it_setFun = l_setAllFuns.begin(); it_setFun != l_setAllFuns.end(); ++it_setFun)
			{
				set<wxString> l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(*it_setFun);
				for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
				{
					wxColour l_nColor1(wxT("Blue"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);
						l_pcItem2[1]->Enable(false);
						l_pcItem2[2]->Enable(true);

					}
				}


				for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
				{
					auto foundItem = l_setDependenciesUnselected.find(it_map->first);
					if (foundItem != l_setDependenciesUnselected.end())
					{
						m_mDeclarations2Dependencies.erase(it_map->second);

					}

				}
				//	auto foundIt = m_mDeclarations2Dependencies.find(*it_setFun);
				//if (foundIt != m_mDeclarations2Dependencies.end())
				//	{
				//		m_mDeclarations2Dependencies.erase(foundIt);
				//	}
			}

		}
		else if (l_sGroup == wxT("Simple Color Sets"))
		{//do unselect all only on Simple cs

			map<wxString, wxString> l_setAllFuns = m_unUsedDec.ObtainAllCs2Type();

			for (auto it_setFun = l_setAllFuns.begin(); it_setFun != l_setAllFuns.end(); ++it_setFun)
			{
				if (it_setFun->second == wxT("Simple"))
				{
					set<wxString> l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(it_setFun->first);
					for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
					{
						wxColour l_nColor1(wxT("Blue"));
						wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
						if (foundId.IsOk())
						{
							this->SetItemTextColour(foundId, l_nColor1);
							l_pcItem2[1]->Enable(false);
							l_pcItem2[2]->Enable(true);

						}
					}

					for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
					{
						auto foundItem = l_setDependenciesUnselected.find(it_map->first);
						if (foundItem != l_setDependenciesUnselected.end())
						{
							m_mDeclarations2Dependencies.erase(it_map->second);

						}

					}
					//	auto foundIt = m_mDeclarations2Dependencies.find(it_setFun->first);
					//	if (foundIt != m_mDeclarations2Dependencies.end())
					//	{
					//		m_mDeclarations2Dependencies.erase(foundIt);
					//	}
				}


			}

		}


		else if (l_sGroup == wxT("Compound Color Sets"))
		{//do unselect all only on Simple cs

			map<wxString, wxString> l_setAllFuns = m_unUsedDec.ObtainAllCs2Type();

			for (auto it_setFun = l_setAllFuns.begin(); it_setFun != l_setAllFuns.end(); ++it_setFun)
			{
				if (it_setFun->second == wxT("Compound"))
				{
					set<wxString> l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(it_setFun->first);
					for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
					{
						wxColour l_nColor1(wxT("Blue"));
						wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
						if (foundId.IsOk())
						{
							this->SetItemTextColour(foundId, l_nColor1);
							l_pcItem2[1]->Enable(false);
							l_pcItem2[2]->Enable(true);


						}
					}


					for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
					{
						auto foundItem = l_setDependenciesUnselected.find(it_map->first);
						if (foundItem != l_setDependenciesUnselected.end())
						{
							m_mDeclarations2Dependencies.erase(it_map->second);

						}

					}
					//auto foundIt = m_mDeclarations2Dependencies.find(it_setFun->first);
					//if (foundIt != m_mDeclarations2Dependencies.end())
					//{
					//	m_mDeclarations2Dependencies.erase(foundIt);
					//}
				}


			}

		}

		else if (l_sGroup == wxT("Alias Color Sets"))
		{//do unselect all only on Simple cs

			map<wxString, wxString> l_setAllCs = m_unUsedDec.ObtainAllCs2Type();

			for (auto it_setcs = l_setAllCs.begin(); it_setcs != l_setAllCs.end(); ++it_setcs)
			{
				if (it_setcs->second == wxT("Alias"))
				{
					set<wxString> l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(it_setcs->first);
					for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
					{
						wxColour l_nColor1(wxT("Blue"));
						wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
						if (foundId.IsOk())
						{
							this->SetItemTextColour(foundId, l_nColor1);
							l_pcItem2[1]->Enable(false);
							l_pcItem2[2]->Enable(true);


						}
					}

					for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
					{
						auto foundItem = l_setDependenciesUnselected.find(it_map->first);
						if (foundItem != l_setDependenciesUnselected.end())
						{
							m_mDeclarations2Dependencies.erase(it_map->second);

						}

					}
					//auto foundIt = m_mDeclarations2Dependencies.find(it_setcs->first);
					//if (foundIt != m_mDeclarations2Dependencies.end())
					//{
					//	m_mDeclarations2Dependencies.erase(foundIt);
					//}
				}


			}

		}

		else if (l_sGroup == wxT("Color Sets"))
		{//do unselect all only on cs

			map<wxString, wxString> it_setAllCs = m_unUsedDec.ObtainAllCs2Type();

			for (auto it_cs = it_setAllCs.begin(); it_cs != it_setAllCs.end(); ++it_cs)
			{
				if (it_cs->second == wxT("Simple") | it_cs->second == wxT("Compound") | it_cs->second == wxT("Alias"))
				{
					set<wxString> l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(it_cs->first);
					for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
					{
						wxColour l_nColor1(wxT("Blue"));
						wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
						if (foundId.IsOk())
						{
							this->SetItemTextColour(foundId, l_nColor1);

							l_pcItem2[1]->Enable(false);
							l_pcItem2[2]->Enable(true);
						}
					}
					for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
					{
						auto foundItem = l_setDependenciesUnselected.find(it_map->first);
						if (foundItem != l_setDependenciesUnselected.end())
						{
							m_mDeclarations2Dependencies.erase(it_map->second);

						}

					}
					//	auto foundIt = m_mDeclarations2Dependencies.find(it_cs->first);
					//if (foundIt != m_mDeclarations2Dependencies.end())
					//{
					//	m_mDeclarations2Dependencies.erase(foundIt);
					//}
				}


			}

		}
		if (l_sGroup == wxT("Declarations"))
		{
			//color sets 
			map<wxString, wxString> it_setAllCs = m_unUsedDec.ObtainAllCs2Type();

			for (auto it_cs = it_setAllCs.begin(); it_cs != it_setAllCs.end(); ++it_cs)
			{
				if (it_cs->second == wxT("Simple") | it_cs->second == wxT("Compound") | it_cs->second == wxT("Alias"))
				{
					set<wxString> l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(it_cs->first);
					for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
					{
						wxColour l_nColor1(wxT("Blue"));
						wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
						if (foundId.IsOk())
						{
							this->SetItemTextColour(foundId, l_nColor1);

							l_pcItem2[1]->Enable(false);
							l_pcItem2[2]->Enable(true);
						}
					}

					for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
					{
						auto foundItem = l_setDependenciesUnselected.find(it_map->first);
						if (foundItem != l_setDependenciesUnselected.end())
						{
							m_mDeclarations2Dependencies.erase(it_map->second);

						}

					}
					//auto foundIt = m_mDeclarations2Dependencies.find(it_cs->first);
					//if (foundIt != m_mDeclarations2Dependencies.end())
					//{
					//	m_mDeclarations2Dependencies.erase(foundIt);
					//}
				}


			}
			//Constants
			set<wxString> l_setConstants = m_unUsedDec.ObtainAllDefinedConstants();
			for (auto it_setConstants = l_setConstants.begin(); it_setConstants != l_setConstants.end(); ++it_setConstants)
			{
				set<wxString> l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(*it_setConstants);
				for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
				{
					wxColour l_nColor1(wxT("Blue"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);

						l_pcItem2[1]->Enable(false);
						l_pcItem2[2]->Enable(true);
					}
				}
				for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
				{
					auto foundItem = l_setDependenciesUnselected.find(it_map->first);
					if (foundItem != l_setDependenciesUnselected.end())
					{
						m_mDeclarations2Dependencies.erase(it_map->second);

					}

				}
				//auto foundIt = m_mDeclarations2Dependencies.find(*it_setConstants);
				//if (foundIt != m_mDeclarations2Dependencies.end())
				//{
				//	m_mDeclarations2Dependencies.erase(foundIt);
				//}

			}
			//functions
			set<wxString> l_setAllFuns = m_unUsedDec.ReadAllDefinedFunctions();

			for (auto it_setFun = l_setAllFuns.begin(); it_setFun != l_setAllFuns.end(); ++it_setFun)
			{
				set<wxString> l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(*it_setFun);
				for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
				{
					wxColour l_nColor1(wxT("Blue"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);

						l_pcItem2[1]->Enable(false);
						l_pcItem2[2]->Enable(true);
					}
				}

				for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
				{
					auto foundItem = l_setDependenciesUnselected.find(it_map->first);
					if (foundItem != l_setDependenciesUnselected.end())
					{
						m_mDeclarations2Dependencies.erase(it_map->second);

					}

				}
				//auto foundIt = m_mDeclarations2Dependencies.find(*it_setFun);
				//	if (foundIt != m_mDeclarations2Dependencies.end())
				//	{
				//		m_mDeclarations2Dependencies.erase(foundIt);
				//	}

			}

			//variables
			set<wxString> l_setAllVarList = m_unUsedDec.ObtainAllDefinedVars();

			for (auto it_setVar = l_setAllVarList.begin(); it_setVar != l_setAllVarList.end(); ++it_setVar)
			{
				set<wxString> l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(*it_setVar);
				for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
				{
					wxColour l_nColor1(wxT("Blue"));
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
					if (foundId.IsOk())
					{
						this->SetItemTextColour(foundId, l_nColor1);

						l_pcItem2[1]->Enable(false);
						l_pcItem2[2]->Enable(true);
					}
				}

				for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
				{
					auto foundItem = l_setDependenciesUnselected.find(it_map->first);
					if (foundItem != l_setDependenciesUnselected.end())
					{
						m_mDeclarations2Dependencies.erase(it_map->second);

					}

				}
				//	auto foundIt = m_mDeclarations2Dependencies.find(*it_setVar);
				//if (foundIt != m_mDeclarations2Dependencies.end())
				//	{
				//		m_mDeclarations2Dependencies.erase(foundIt);
				//	}
			}


		}//colse all declarations


	}


}
void  SP_WDG_DeclarationTreectrl::OnUnSelecting(wxCommandEvent& p_cEvent)
{
	set<wxString> l_setDependenciesUnselected;
	if (!m_pcGraph->GetNetclass()->GetDisplayName().Contains("Colored"))
	{
		//uncolored pn
		l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(m_sSelected);
		for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
		{
			wxColour l_nColor1(wxT("Blue"));
			wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
			if (foundId.IsOk())
			{
				this->SetItemTextColour(foundId, l_nColor1);

				l_pcItem1[0]->Enable(true);
				l_pcItem1[1]->Enable(false);

			}
		}
		for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
		{
			auto foundItem = l_setDependenciesUnselected.find(it_map->first);
			if (foundItem != l_setDependenciesUnselected.end())
			{
				m_mDeclarations2Dependencies.erase(it_map->second);

			}


		}


	}
	else {
		//	colored PN
		//individual colored item un-selection
		set<wxString> l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(m_sSelected);
		for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
		{
			wxColour l_nColor1(wxT("Blue"));
			wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
			if (foundId.IsOk())
			{
				this->SetItemTextColour(foundId, l_nColor1);

				l_pcItem1[0]->Enable(true);
				l_pcItem1[1]->Enable(false);

			}
		}


		for (auto it_map = m_vtrimeddecAll.begin(); it_map != m_vtrimeddecAll.end(); ++it_map)
		{
			auto foundItem = l_setDependenciesUnselected.find(it_map->first);
			if (foundItem != l_setDependenciesUnselected.end())
			{
				m_mDeclarations2Dependencies.erase(it_map->second);

			}


		}

		if (m_sSelected.Contains(":"))
			m_sSelected = m_sSelected.BeforeFirst(':');

		auto foundItem = m_mDeclarations2Dependencies.find(m_sSelected);
		if (foundItem != m_mDeclarations2Dependencies.end())
		{
			m_mDeclarations2Dependencies.erase(foundItem);

		}

	}
}

map<wxString, int> SP_WDG_DeclarationTreectrl::ComputeSelection(wxTreeItemId p_TreeITem)
{
	map<wxString, int> l_mResult;
	if (p_TreeITem.IsOk() && this->ItemHasChildren(p_TreeITem))
	{
		int l_iSize = this->GetChildrenCount(p_TreeITem);
		wxTreeItemId l_cRootId;
		l_cRootId = p_TreeITem;
		wxTreeItemIdValue l_nCookie = NULL;


		int k = 1;
		int l_iBlue = 0;
		int l_iRed = 0;
		wxTreeItemId l_cFirstChild = GetFirstChild(l_cRootId, l_nCookie);
		wxColour l_nColor1(wxT("Blue"));
		wxColour l_nColorRed(wxT("Red"));
		wxColour l_itemColour = this->GetItemTextColour(l_cFirstChild);
		if (l_itemColour == l_nColor1)
		{
			l_iBlue++;
			l_mResult[wxT("Blue")] = l_iBlue;
		}
		if (l_itemColour == l_nColorRed)
		{
			l_iRed++;
			l_mResult[wxT("Red")] = l_iRed;
		}
		while (k < l_iSize)
		{
			//GetNextSibling
			wxTreeItemId l_cFirstChild_FirstChild = GetNextSibling(l_cFirstChild);

			wxColour l_itemColour1 = this->GetItemTextColour(l_cFirstChild_FirstChild);
			if (l_itemColour1 == l_nColor1)
			{
				l_iBlue++;
				l_mResult[wxT("Blue")] = l_iBlue;
			}
			if (l_itemColour1 == l_nColorRed)
			{
				l_iRed++;
				l_mResult[wxT("Red")] = l_iRed;
			}
			l_cFirstChild = l_cFirstChild_FirstChild;
			DeleteChildren(l_cFirstChild_FirstChild);
			k++;
		}
		//wxTreeItemId l_cFirstChild = GetFirstChild(l_cRootId, l_nCookie);
		//wxTreeItemId l_cFirstChild_FirstChild = GetFirstChild(l_cFirstChild, l_nCookie);
		//DeleteChildren(l_cFirstChild_FirstChild);


	}

	return l_mResult;
}

void SP_WDG_DeclarationTreectrl::OnRMouseClick(wxTreeEvent& p_cEvent)
{
	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();

	bool l_bIsColoured = false;
	map<wxString, int> l_mSelection2Num;

	if (l_netType.Contains(wxT("Colored")))
	{
		l_bIsColoured = true;
	}
	wxTreeItemId item = p_cEvent.GetItem();
	m_sSelected = GetItemText(item);




	map<wxString, int> l_mSelection2NumSimple;// = ComputeSelection(item);
	map<wxString, int> l_mSelection2NumCompound;// = ComputeSelection(item);
	map<wxString, int> l_mSelection2NumAlias;// = ComputeSelection(item);
	map<wxString, int> l_mSelection2NumConst;// = ComputeSelection(item);
	map<wxString, int> l_mSelection2NumFun;// = ComputeSelection(item);
	map<wxString, int> l_mSelection2NumVar;// = ComputeSelection(item);
	map<wxString, int> l_mSelection2NumObservers;// = ComputeSelection(item);

	if (/*m_bisIsFirst && */(m_sSelected == wxT("Declarations") | m_sSelected == wxT("Color Sets"))) {
		wxString l_sSimplecs = wxT("Simple Color Sets");
		wxString l_sCompoundcs = wxT("Compound Color Sets");
		wxString l_sAlias = wxT("Alias Color Sets");
		wxString l_sConst = wxT("Constants");
		wxString l_sFun = wxT("Functions");
		wxString l_sVar = wxT("Variables");
		wxString l_sObserver = wxT("Observers");

		wxTreeItemId foundSimplecs = findTreeItem(this, this->GetRootItem(), l_sSimplecs, true, true);
		if (foundSimplecs.IsOk())
		{
			l_mSelection2NumSimple = ComputeSelection(foundSimplecs);
		}

		wxTreeItemId foundCompoundcs = findTreeItem(this, this->GetRootItem(), l_sCompoundcs, true, true);
		if (foundCompoundcs.IsOk())
		{
			l_mSelection2NumCompound = ComputeSelection(foundCompoundcs);
		}

		wxTreeItemId foundAlias = findTreeItem(this, this->GetRootItem(), l_sAlias, true, true);
		if (foundAlias.IsOk())
		{
			l_mSelection2NumAlias = ComputeSelection(foundAlias);
		}
		wxTreeItemId foundFun = findTreeItem(this, this->GetRootItem(), l_sFun, true, true);
		if (foundFun.IsOk())
		{
			l_mSelection2NumFun = ComputeSelection(foundFun);
		}

		wxTreeItemId foundConst = findTreeItem(this, this->GetRootItem(), l_sConst, true, true);
		if (foundConst.IsOk())
		{
			l_mSelection2NumConst = ComputeSelection(foundConst);
		}

		wxTreeItemId foundVar = findTreeItem(this, this->GetRootItem(), l_sVar, true, true);
		if (foundVar.IsOk())
		{
			l_mSelection2NumVar = ComputeSelection(foundVar);
		}

		wxTreeItemId foundOb = findTreeItem(this, this->GetRootItem(), l_sObserver, true, true);
		if (foundOb.IsOk())
		{
			l_mSelection2NumObservers = ComputeSelection(foundOb);

		}
		if (m_sSelected == wxT("Declarations") && l_mSelection2NumSimple.size() == 0 && l_mSelection2NumCompound.size() == 0 && l_mSelection2NumAlias.size() == 0 &&
			l_mSelection2NumFun.size() == 0 && l_mSelection2NumConst.size() == 0 && l_mSelection2NumVar.size() == 0 && l_mSelection2NumObservers.size() == 0)
		{
			l_pcItem2[0]->Enable(true);
			l_pcItem2[1]->Enable(false);
			l_pcItem2[2]->Enable(false);
			l_pcItem2[3]->Enable(false);
		}
		else {
			int l_iBlueItems = 0;
			int l_iRedItems = 0;
			l_iBlueItems = l_mSelection2NumSimple[wxT("Blue")];
			l_iRedItems = l_mSelection2NumSimple[wxT("Red")];
			l_iBlueItems += l_mSelection2NumCompound[wxT("Blue")] + l_mSelection2NumAlias[wxT("Blue")] + l_mSelection2NumFun[wxT("Blue")] + l_mSelection2NumConst[wxT("Blue")] + l_mSelection2NumVar[wxT("Blue")] + l_mSelection2NumObservers[wxT("Blue")];

			l_iRedItems += l_mSelection2NumCompound[wxT("Red")] + l_mSelection2NumAlias[wxT("Red")] + l_mSelection2NumFun[wxT("Red")] + l_mSelection2NumConst[wxT("Red")] + l_mSelection2NumVar[wxT("Red")] + l_mSelection2NumObservers[wxT("Red")];




			if (l_iBlueItems > 0 && l_iRedItems == 0)
			{
				l_pcItem2[0]->Enable(true);
				l_pcItem2[1]->Enable(false);
				l_pcItem2[2]->Enable(true);
				l_pcItem2[3]->Enable(false);
			}
			else if (l_iBlueItems == 0 && l_iRedItems > 0)
			{
				l_pcItem2[0]->Enable(true);
				l_pcItem2[1]->Enable(true);
				l_pcItem2[2]->Enable(false);
				l_pcItem2[3]->Enable(true);
			}
			else {
				l_pcItem2[0]->Enable(true);
				l_pcItem2[1]->Enable(true);
				l_pcItem2[2]->Enable(true);
				l_pcItem2[3]->Enable(true);
			}

		}
		if (m_sSelected == wxT("Color Sets") && l_mSelection2NumSimple.size() == 0 && l_mSelection2NumCompound.size() == 0 && l_mSelection2NumAlias.size() == 0)
		{
			l_pcItem2[0]->Enable(true);
			l_pcItem2[1]->Enable(false);
			l_pcItem2[2]->Enable(false);
			l_pcItem2[3]->Enable(false);
		}
		else {
			if (m_sSelected == wxT("Color Sets")) {
				int l_iBlueItems = l_mSelection2NumSimple[wxT("Blue")];
				int l_iRedItems = l_mSelection2NumSimple[wxT("Red")];

				l_iBlueItems += l_mSelection2NumCompound[wxT("Blue")] + l_mSelection2NumAlias[wxT("Blue")];
				l_iRedItems += l_mSelection2NumCompound[wxT("Red")] + l_mSelection2NumAlias[wxT("Red")];



				if (l_iBlueItems > 0 && l_iRedItems == 0)
				{
					l_pcItem2[0]->Enable(true);
					l_pcItem2[1]->Enable(false);
					l_pcItem2[2]->Enable(true);
					l_pcItem2[3]->Enable(false);
				}
				else if (l_iBlueItems == 0 && l_iRedItems > 0)
				{
					l_pcItem2[0]->Enable(true);
					l_pcItem2[1]->Enable(true);
					l_pcItem2[2]->Enable(false);
					l_pcItem2[3]->Enable(true);
				}
				else {
					l_pcItem2[0]->Enable(true);
					l_pcItem2[1]->Enable(true);
					l_pcItem2[2]->Enable(true);
					l_pcItem2[3]->Enable(true);
				}
			}
		}

		//l_pcItem2[0]->Enable(true);
		//l_pcItem2[1]->Enable(true);
		//l_pcItem2[2]->Enable(true);
		//l_pcItem2[3]->Enable(true);
		PopupMenu(&l_cMenu);
	}
	//if (!m_bisIsFirst && (m_sSelected == wxT("Declarations") | m_sSelected == wxT("Color Sets"))) {
	//l_pcItem2[0]->Enable(true);
	//l_pcItem2[1]->Enable(false);
	//l_pcItem2[2]->Enable(false);
	//l_pcItem2[3]->Enable(false);
	//PopupMenu(&l_cMenu);
	//}
	else if (item.IsOk() && this->ItemHasChildren(item))
	{

		l_mSelection2Num = ComputeSelection(item);
		if (l_mSelection2Num.size() == 0)
		{
			l_pcItem2[0]->Enable(true);
			l_pcItem2[1]->Enable(false);
			l_pcItem2[2]->Enable(false);
			l_pcItem2[3]->Enable(false);
		}
		else {
			int l_iBlueItems = l_mSelection2Num[wxT("Blue")];
			int l_iRedItems = l_mSelection2Num[wxT("Red")];
			if (l_iBlueItems > 0 && l_iRedItems == 0)
			{
				l_pcItem2[0]->Enable(true);
				l_pcItem2[1]->Enable(false);
				l_pcItem2[2]->Enable(true);
				l_pcItem2[3]->Enable(false);
			}
			else if (l_iBlueItems == 0 && l_iRedItems > 0) {
				l_pcItem2[0]->Enable(true);
				l_pcItem2[1]->Enable(true);
				l_pcItem2[2]->Enable(false);
				l_pcItem2[3]->Enable(true);
			}
			else if (l_iBlueItems> 0 && l_iRedItems > 0) {
				l_pcItem2[0]->Enable(true);
				l_pcItem2[1]->Enable(true);
				l_pcItem2[2]->Enable(true);
				l_pcItem2[3]->Enable(true);
			}

		}


		PopupMenu(&l_cMenu);// p_cEvent.GetPosition());

	}
	else if (item.IsOk())
	{
		//wxMenu l_cMenuOptions;
		//l_cMenuOptions.Append(SP_MENU_RITEM1_TREE+500, wxT("select item"));
		//l_cMenuOptions.Append(SP_MENU_RITEM2_TREE+500, wxT("unselect item"));

		bool l_bIsFound = false;
		std::set<wxString> l_setUnusedList;
		std::map<wxString, wxString> l_mapTrimedDec;
		auto f_it = m_vtrimeddecConstants.find(m_sSelected);
		wxString l_sSearch;
		if (f_it != m_vtrimeddecConstants.end())
		{
			wxString l_setConstGroup = wxT("Constants");
			l_mapTrimedDec = m_vtrimeddecConstants;
			l_setUnusedList = m_vUnUsedConstantsList;
			l_bIsFound = true;
			l_sSearch = (f_it->second).Trim();
		}
		if (!l_bIsFound)
		{
			auto f_it = m_vtrimeddecFunctions.find(m_sSelected);
			if (f_it != m_vtrimeddecFunctions.end())
			{
				l_mapTrimedDec = m_vtrimeddecFunctions;
				l_setUnusedList = m_setUnUsedFunctionsList;
				l_bIsFound = true;
				l_sSearch = (f_it->second).Trim();
			}

		}

		if (!l_bIsFound)
		{
			auto f_it = m_vtrimeddecObservers.find(m_sSelected);
			if (f_it != m_vtrimeddecObservers.end())
			{
				l_mapTrimedDec = m_vtrimeddecObservers;
				l_setUnusedList = m_setUnUsedObserverList;
				l_bIsFound = true;
				l_sSearch = (f_it->second).Trim();
			}

		}
		if (!l_bIsFound)
		{
			auto f_it = m_vtrimeddecVariables.find(m_sSelected);
			if (f_it != m_vtrimeddecVariables.end())
			{
				l_mapTrimedDec = m_vtrimeddecVariables;
				l_setUnusedList = m_setUnUsedVariablesList;
				l_bIsFound = true;
				l_sSearch = (f_it->second).Trim();
			}

		}

		if (!l_bIsFound)
		{
			auto f_it = m_vtrimeddecSimplCS.find(m_sSelected);
			if (f_it != m_vtrimeddecSimplCS.end())
			{
				l_mapTrimedDec = m_vtrimeddecSimplCS;
				l_setUnusedList = m_setUnUsedSimpleCS;
				l_bIsFound = true;
				l_sSearch = (f_it->second).Trim();
			}

		}

		if (!l_bIsFound)
		{
			auto f_it = m_vtrimeddecCompoundCS.find(m_sSelected);
			if (f_it != m_vtrimeddecCompoundCS.end())
			{
				l_mapTrimedDec = m_vtrimeddecCompoundCS;
				l_setUnusedList = m_setUnUsedCompoundCS;
				l_bIsFound = true;
				l_sSearch = (f_it->second).Trim();
			}
			if (!l_bIsFound)
			{
				auto f_it = m_vtrimeddecAliasCS.find(m_sSelected);
				if (f_it != m_vtrimeddecAliasCS.end())
				{
					l_mapTrimedDec = m_vtrimeddecAliasCS;
					l_setUnusedList = m_setUnUsedAliasCS;
					l_bIsFound = true;
					l_sSearch = (f_it->second).Trim();
				}

			}
		}
		if (l_bIsFound) {
			auto it_found = l_setUnusedList.find(l_sSearch);

			if (it_found == l_setUnusedList.end())
			{
				l_pcItem1[0]->Enable(false);
				l_pcItem1[1]->Enable(false);

			}
			else {
				l_pcItem1[0]->Enable(true);
				l_pcItem1[1]->Enable(true);

				wxTreeItemId item = p_cEvent.GetItem();
				//m_sSelected = GetItemText(item);
				wxColour l_itemColour1 = this->GetItemTextColour(item);
				wxColour l_colourRed(wxT("Red"));
				wxColour l_colourBlue(wxT("Blue"));
				if (l_itemColour1 == l_colourRed)
				{
					l_pcItem1[0]->Enable(false);
					l_pcItem1[1]->Enable(true);
				}
				else {
					l_pcItem1[0]->Enable(true);
					l_pcItem1[1]->Enable(false);
				}

			}
		}
		else {
			//wxMenuItemList
			l_pcItem1 = l_cMenuOptions.GetMenuItems();
			l_pcItem1[0]->Enable(false);
			l_pcItem1[1]->Enable(false);

		}


		PopupMenu(&l_cMenuOptions);
		l_pcItem1 = l_cMenuOptions.GetMenuItems();



		wxColour l_nColor1(wxT("Blue"));
		wxColour l_nColorRed(wxT("Red"));
		wxColour l_itemColour = this->GetItemTextColour(item);
		//wxMenuItemList 
		l_pcItem1 = l_cMenuOptions.GetMenuItems();
		if (l_nColor1 == l_itemColour) {

			l_pcItem1[0]->Enable(true);
			l_pcItem1[1]->Enable(false);
		}
		else if (l_nColorRed == l_itemColour)
		{
			l_pcItem1[0]->Enable(false);
			l_pcItem1[1]->Enable(true);
		}
		else {
			l_pcItem1[0]->Enable(false);
			l_pcItem1[1]->Enable(false);
		}
	}

}
void
SP_WDG_DeclarationTreectrl::OnDoubleClick(wxTreeEvent& p_cEvent)
{

	wxString l_label = GetItemText(p_cEvent.GetItem());
	if (p_cEvent.GetItem() == GetRootItem())
	{
		ExpandAllChildren(GetRootItem());
	}
	if (l_label == wxT("Color Sets"))
	{
		SP_DLG_ColorSetSetting l_cColorSetDlg(NULL, BASICCOLORSETGRID);
		l_cColorSetDlg.ShowModal();
	}
	//if(l_label==wxT("Basic Colorsets"))
	if (l_label == wxT("Simple Color Sets"))
	{
		SP_DLG_ColorSetSetting l_cColorSetDlg(NULL, BASICCOLORSETGRID);
		l_cColorSetDlg.ShowModal();
	}
	if (l_label == wxT("Compound Color Sets"))
	{
		SP_DLG_ColorSetSetting l_cColorSetDlg(NULL, STRUCTUREDCOLORSETGRID);
		l_cColorSetDlg.ShowModal();
	}

	//if(l_label==wxT("Alias Colorsets"))
	if (l_label == wxT("Alias Color Sets"))
	{
		SP_DLG_ColorSetSetting l_cColorSetDlg(NULL, ALIASCOLORSETGRID);
		l_cColorSetDlg.ShowModal();
	}

	if (l_label == wxT("Constants"))
	{
		SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
		if (!l_pcGraph)
			return;

		wxString l_sNetClassName = l_pcGraph->GetNetclass()->GetName();

		if (l_sNetClassName.Contains(wxT("Colored")))
		{
			SP_DLG_ConstantDefinition l_cConstantDlg(NULL);
			l_cConstantDlg.ShowModal();
		}
		else if (l_sNetClassName.Contains(wxT("Fuzzy")))
		{
			SP_DLG_FpnConstantDefinition l_cConstantDlg(NULL);
			l_cConstantDlg.ShowModal();
		}
		else
		{
			SP_DLG_NewConstantDefinition l_cConstantDlg(NULL);
			l_cConstantDlg.ShowModal();
		}

	}

	if (l_label == wxT("Variables"))
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
	wxTreeItemId l_cRootId = GetRootItem();
	wxTreeItemIdValue l_nCookie = NULL;

	m_ColorsMap.clear();

	GetFirstChild(l_cRootId, l_nCookie);
	wxTreeItemId l_cSecondChild = GetFirstChild(l_cRootId, l_nCookie);
	DeleteChildren(l_cSecondChild);

	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();

	//wxString cName = p_doc->GetNetclassName();
	wxString netClass = SP_Core::Instance()->GetRootDocument()->GetNetclassName();
	if (!l_pcGraph)
		return;

	SP_DS_Metadataclass* l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_META_CONSTANT);//SP_DS_META_CONSTANT="Constant Class"
	if (l_pcMetadataclass)
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
			wxString l_sMetadataType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->GetValue();

			for (unsigned int i = 0; i < l_pcColList->GetRowCount(); ++i)
			{
				if (l_sMetadataType.Cmp(wxT("TFN")) == 0 && (netClass == SP_DS_FUZZYSPN_CLASS || netClass == SP_DS_FUZZYCPN_CLASS || netClass == SP_DS_FUZZYHPN_CLASS))
				{


					l_sMetadataValue << wxT("(") << l_pcColList->GetCell(i, 1) << wxT(");");
				}
				else if (l_sMetadataType.Cmp(wxT("BFN")) == 0 && (netClass == SP_DS_FUZZYSPN_CLASS || netClass == SP_DS_FUZZYHPN_CLASS || netClass == SP_DS_FUZZYCPN_CLASS))
				{
					l_sMetadataValue << wxT("(") << l_pcColList->GetCell(i, 1) << wxT(");");

				}
				else if (l_sMetadataType.Cmp(wxT("TZN")) == 0 && (netClass == SP_DS_FUZZYSPN_CLASS || netClass == SP_DS_FUZZYHPN_CLASS || netClass == SP_DS_FUZZYCPN_CLASS))
				{
					l_sMetadataValue << wxT("(") << l_pcColList->GetCell(i, 1) << wxT(");");

				}
				else
				{
					l_sMetadataValue << l_pcColList->GetCell(i, 1) << wxT(";");

				}
			}
			////////////////
			if ((l_sMetadataType.Cmp(wxT("TFN")) == 0 || l_sMetadataType.Cmp(wxT("BFN")) == 0 || l_sMetadataType.Cmp(wxT("TZN")) == 0) && (netClass == SP_DS_FUZZYSPN_CLASS || netClass == SP_DS_FUZZYHPN_CLASS || netClass == SP_DS_FUZZYCPN_CLASS))
			{
				m_ColorsMap[l_sMetadataName] = l_sMetadataGroup + wxT(" : ") + l_sMetadataValue;
				AppendFileItem(l_cSecondChild, l_sMetadataName + l_sMetadataValue, NULL);
				wxString msg = "new" + l_sMetadataType + " Fuzzy Number is defined  " + l_sMetadataName + ":" + l_sMetadataValue;
				SP_LOGMESSAGE(msg);
			}
			/////////////////

			else {
				if ((netClass != SP_DS_FUZZYSPN_CLASS || netClass == SP_DS_FUZZYHPN_CLASS || netClass != SP_DS_FUZZYCPN_CLASS) && !l_sMetadataValue.Contains(","))
				{

					m_ColorsMap[l_sMetadataName] = l_sMetadataGroup + wxT(" : ") + l_sMetadataValue;
					AppendFileItem(l_cSecondChild, l_sMetadataName + wxT(" : ") + l_sMetadataValue, NULL);
				}
				else if (netClass == SP_DS_FUZZYSPN_CLASS || netClass == SP_DS_FUZZYHPN_CLASS || netClass == SP_DS_FUZZYCPN_CLASS)
				{
					m_ColorsMap[l_sMetadataName] = l_sMetadataGroup + wxT(" : ") + l_sMetadataValue;
					AppendFileItem(l_cSecondChild, l_sMetadataName + wxT(" : ") + l_sMetadataValue, NULL);
				}
				else {}
			}
		}


	}


	wxTreeItemId l_cThirdChild = GetNextChild(l_cRootId, l_nCookie);
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
	l_cRootId = GetRootItem();
	wxTreeItemIdValue l_nCookie = NULL;

	m_ColorsMap.clear();

	map<wxString, wxString> l_mCSName2PlaceColor;

	//colorset
	wxTreeItemId l_cFirstChild = GetFirstChild(l_cRootId, l_nCookie);
	wxTreeItemId l_cFirstChild_FirstChild = GetFirstChild(l_cFirstChild, l_nCookie);
	DeleteChildren(l_cFirstChild_FirstChild);
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if (!l_pcGraph)
		return;

	SP_DS_Metadataclass* l_pcMetadataclass = NULL;
	SP_DS_Metadata* l_pcNewMetadata = NULL;

	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_BASICCOLORSETCLASS);
	if (!l_pcMetadataclass)
		return;

	if (l_pcMetadataclass->GetElements()->empty())
		return;

	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ColorsetList")));
	if (!l_pcColList)
		return;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		wxString l_sColors = l_pcColList->GetCell(i, 2).c_str();
		m_ColorsMap[l_sName] = l_sColors;

		wxString l_sPlaceColor = l_pcColList->GetCell(i, 3).c_str();
		wxString l_sShow = l_pcColList->GetCell(i, 4);
		if (l_sShow == wxT("1"))
			l_mCSName2PlaceColor[l_sName] = l_sPlaceColor;

		AppendFileItem(l_cFirstChild_FirstChild, l_sName, NULL);
	}

	wxTreeItemId l_cFirstChild_SecondChild = GetNextChild(l_cFirstChild, l_nCookie);
	DeleteChildren(l_cFirstChild_SecondChild);
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_STRUCTUREDCOLORSETCLASS);
	if (!l_pcMetadataclass)
		return;

	if (l_pcMetadataclass->GetElements()->empty())
		return;

	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("StructuredColorsetList")));
	if (!l_pcColList)
		return;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		wxString l_sColors = l_pcColList->GetCell(i, 2).c_str();
		m_ColorsMap[l_sName] = l_sColors;

		wxString l_sPlaceColor = l_pcColList->GetCell(i, 4).c_str();
		wxString l_sShow = l_pcColList->GetCell(i, 5);
		if (l_sShow == wxT("1"))
			l_mCSName2PlaceColor[l_sName] = l_sPlaceColor;

		AppendFileItem(l_cFirstChild_SecondChild, l_sName, NULL);
	}


	wxTreeItemId l_cFirstChild_FourthChild = GetNextChild(l_cFirstChild, l_nCookie);
	DeleteChildren(l_cFirstChild_FourthChild);
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_ALIASCOLORSETCLASS);
	if (!l_pcMetadataclass)
		return;

	if (l_pcMetadataclass->GetElements()->empty())
		return;

	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("AliasColorsetList")));
	if (!l_pcColList)
		return;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		wxString l_sColors = l_pcColList->GetCell(i, 1).c_str();
		m_ColorsMap[l_sName] = l_sColors;
		AppendFileItem(l_cFirstChild_FourthChild, l_sName, NULL);
	}

	l_cRootId = GetRootItem();
	GetFirstChild(l_cRootId, l_nCookie);
	wxTreeItemId l_cSecondChild = GetNextChild(l_cRootId, l_nCookie);
	DeleteChildren(l_cSecondChild);
	/*********constants******/
	//l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANTCLASS);
	//if (!l_pcMetadataclass)
	//	return;

	//if (l_pcMetadataclass->GetElements()->empty())
	//	return;

	//l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	//l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("ConstantList")));
	//if (!l_pcColList)
	//	return;
	SP_DS_Metadataclass* m_pcConstants = l_pcGraph->GetMetadataclass(SP_DS_CPN_CONSTANT_HARMONIZING);
	if (m_pcConstants) {
		for (SP_DS_Metadata* l_pcMetadata : *(m_pcConstants->GetElements()))
		{
			wxString l_sMetadataName = dynamic_cast<SP_DS_NameAttribute*>(l_pcMetadata->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
			wxString l_sMetadataGroup = dynamic_cast<SP_DS_TextAttribute*>(l_pcMetadata->GetAttribute(wxT("Group")))->GetValue();
			wxString l_sMetadataType = dynamic_cast<SP_DS_TypeAttribute*>(l_pcMetadata->GetAttribute(wxT("Type")))->GetValue();
			wxString l_sMetadataComment = dynamic_cast<SP_DS_TextAttribute*>(l_pcMetadata->GetAttribute(wxT("Comment")))->GetValue();

			//wxString l_sMetadataShow = l_pcMetadata->GetShow() ? wxT("1") : wxT("0");
			SP_DS_ColListAttribute * l_pcColList = dynamic_cast<SP_DS_ColListAttribute*>(l_pcMetadata->GetAttribute(wxT("ValueList")));
			wxString l_sVsets;
			for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
			{
				wxString l_sSetName = l_pcColList->GetCell(i, 0).c_str();
				wxString l_sValue = l_pcColList->GetCell(i, 1).c_str();
				//wxString l_sValue = l_pcColList->GetCell(i, 2).c_str();
				if (l_sMetadataType == wxT("int") && i == 0)
				{
					m_ColorsMap[l_sMetadataName] = l_sMetadataType + wxT(":") + l_sValue;
				}
				l_sVsets << l_sValue << ";";
				if (i == l_pcColList->GetRowCount() - 1)
				{
					AppendFileItem(l_cSecondChild, l_sMetadataName + wxT(":") + l_sVsets, NULL);
					break;
				}
			}

		}
	}
	/*********************************/

	wxTreeItemId l_cThirdChild = GetNextChild(l_cRootId, l_nCookie);
	DeleteChildren(l_cThirdChild);
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_FUNCTIONCLASS);
	if (!l_pcMetadataclass)
		return;

	if (l_pcMetadataclass->GetElements()->empty())
		return;

	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("FunctionList")));
	if (!l_pcColList)
		return;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 1).c_str();
		wxString l_sColors = l_pcColList->GetCell(i, 0) + wxT(" ") + l_pcColList->GetCell(i, 1) + wxT("( ") +
			l_pcColList->GetCell(i, 2) + wxT(") : ") + l_pcColList->GetCell(i, 3);
		m_ColorsMap[l_sName] = l_sColors;
		AppendFileItem(l_cThirdChild, l_sName, NULL);
	}

	wxTreeItemId l_cFourthChild = GetNextChild(l_cRootId, l_nCookie);
	DeleteChildren(l_cFourthChild);
	l_pcMetadataclass = l_pcGraph->GetMetadataclass(SP_DS_CPN_VARIABLECLASS);
	if (!l_pcMetadataclass)
		return;

	if (l_pcMetadataclass->GetElements()->empty())
		return;

	l_pcNewMetadata = l_pcMetadataclass->GetElements()->front();

	l_pcColList = dynamic_cast<SP_DS_ColListAttribute*> (l_pcNewMetadata->GetAttribute(wxT("VariableList")));
	if (!l_pcColList)
		return;

	for (unsigned int i = 0; i < l_pcColList->GetRowCount(); i++)
	{
		wxString l_sName = l_pcColList->GetCell(i, 0).c_str();
		wxString l_sColors = l_pcColList->GetCell(i, 1).c_str();
		m_ColorsMap[l_sName] = l_sColors;
		AppendFileItem(l_cFourthChild, l_sName, NULL);
	}

 
	//update the real color of a place
	SP_GUI_Canvas *l_cCanvas = NULL;
	wxString l_sNetClassName = l_pcGraph->GetNetclass()->GetName();
	wxString l_sPlaceNodeClassName;

	if (l_sNetClassName == SP_DS_COLCPN_CLASS)
		l_sPlaceNodeClassName = SP_DS_CONTINUOUS_PLACE;
	else
		l_sPlaceNodeClassName = SP_DS_DISCRETE_PLACE;

	SP_DS_Nodeclass* l_pcNodeclass = l_pcGraph->GetNodeclass(l_sPlaceNodeClassName);
	SP_ListNode::const_iterator l_itElem;
	bool l_bCanvas = true;
	for (l_itElem = l_pcNodeclass->GetElements()->begin(); l_itElem != l_pcNodeclass->GetElements()->end(); ++l_itElem)
	{
		SP_DS_Node* l_pcPlaceNode = (*l_itElem);
		SP_DS_TextAttribute* l_pcTextAttibute = dynamic_cast< SP_DS_TextAttribute* >(l_pcPlaceNode->GetAttribute(SP_DS_CPN_COLORSETNAME));
		if (!l_pcTextAttibute)
			return;
		wxString l_sColorSetName = l_pcTextAttibute->GetValue();

		wxString l_sPlaceRColor;
		if (l_mCSName2PlaceColor.find(l_sColorSetName) != l_mCSName2PlaceColor.end())
			l_sPlaceRColor = l_mCSName2PlaceColor.find(l_sColorSetName)->second;
		else
			l_sPlaceRColor = wxT("WHITE");
		wxColour l_PlaceRColor = wxColour(l_sPlaceRColor);

		SP_ListGraphic::iterator l_Iter;
		SP_ListGraphic* l_PlaceGraphicsList = l_pcPlaceNode->GetGraphics();
		if (l_bCanvas)
		{
			l_cCanvas = (*(l_PlaceGraphicsList->begin()))->GetCanvas();
			l_bCanvas = false;
		}
		for (l_Iter = l_PlaceGraphicsList->begin(); l_Iter != l_PlaceGraphicsList->end(); ++l_Iter)
		{
			wxBrush* l_pcBrush = wxTheBrushList->FindOrCreateBrush(l_PlaceRColor, (*l_Iter)->GetBrush()->GetStyle());
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



	m_unUsedDec = {};//reset the object SP_DS_UnUsedDeclaration();
	ResetCheckingResults();

}


void SP_WDG_DeclarationTreectrl::OnToolTip(wxTreeEvent& p_cEvent)
{
	wxString l_sColorsetName = this->GetItemText(p_cEvent.GetItem());
	if (m_ColorsMap.find(l_sColorsetName) != m_ColorsMap.end())
	{
		wxString l_sColors = m_ColorsMap.find(l_sColorsetName)->second;
		p_cEvent.SetToolTip(l_sColors);
	}
	wxTreeItemId foundConstID = findTreeItem(this, this->GetRootItem(), l_sColorsetName
		, true, true);
	wxColour l_nColor1(wxT("Blue"));
	wxColour l_nColor2(wxT("Red"));
	wxColour l_itemColour = this->GetItemTextColour(foundConstID);

	if (foundConstID.IsOk() && l_nColor1 == l_itemColour)
	{
		//wxColour l_nColor1(wxT("Red"));
		//this->SetItemTextColour(foundConstID, l_nColor1);
		l_pcItem1[0]->Enable(true);
		l_pcItem1[1]->Enable(false);
	}
	else
		if (foundConstID.IsOk() && l_nColor2 == l_itemColour) {
			l_pcItem1[0]->Enable(false);
			l_pcItem1[1]->Enable(true);
		}
		else {
			l_pcItem1[0]->Enable(false);
			l_pcItem1[1]->Enable(false);
		}

}

//Added by G.Assaf
void SP_WDG_DeclarationTreectrl::ReadTreeItems(wxTreeCtrl* pTreeCtrl, const wxTreeItemId& root, SP_VectorString &vres)
{
	wxTreeItemId item = root, child;
	wxTreeItemIdValue cookie;
	SP_VectorString res, left;



	while (item.IsOk())
	{
		vres.push_back(pTreeCtrl->GetItemText(item));
		//if (!bCaseSensitive) itemtext.MakeLower();
		//	bFound = bExactMatch ? (itemtext == findtext) : itemtext.Contains(findtext);
		if (pTreeCtrl->GetCount() == 0) return;
		child = pTreeCtrl->GetFirstChild(item, cookie);
		if (child.IsOk())   ReadTreeItems(pTreeCtrl, child, vres);
		//if (child.IsOk()) return ;
		item = pTreeCtrl->GetNextSibling(item);
	} // while(item.IsOk())




}
wxTreeItemId SP_WDG_DeclarationTreectrl::findTreeItem(wxTreeCtrl* pTreeCtrl, const wxTreeItemId& root, const wxString& text, bool bCaseSensitive, bool bExactMatch)
{
	wxTreeItemId item = root, child;
	wxTreeItemIdValue cookie;
	wxString findtext(text), itemtext;
	bool bFound;
	if (!bCaseSensitive) findtext.MakeLower();

	while (item.IsOk())
	{
		itemtext = pTreeCtrl->GetItemText(item);
		if (!bCaseSensitive) itemtext.MakeLower();
		bFound = bExactMatch ? (itemtext == findtext) : itemtext.Contains(findtext);
		if (bFound) return item;
		child = pTreeCtrl->GetFirstChild(item, cookie);
		if (child.IsOk()) child = findTreeItem(pTreeCtrl, child, text, bCaseSensitive, bExactMatch);
		if (child.IsOk()) return child;
		item = pTreeCtrl->GetNextSibling(item);
	} // while(item.IsOk())

	return item;
}
std::map<wxString, wxString> SP_WDG_DeclarationTreectrl::ObtainDeclarationGroup(wxString &p_sGroup)
{
	std::map<wxString, wxString> l_mDecGroup;
	return l_mDecGroup;
}

void SP_WDG_DeclarationTreectrl::ResetCheckingResults()
{

	m_sSelected = wxT("");
	m_setSelectedItems;
	m_setSelectedConstants.clear();
	m_setSelectedfunctions.clear();
	m_setSelectedObservers.clear();
	m_setUnSelectedItems.clear();
	m_setUnSelectedContants.clear();
	m_setUnSelectedFunctions.clear();
	m_setUnSelectedObservers.clear();
	m_vUnUsedDecList.clear();
	m_vUnUsedConstantsList.clear();
	m_setUnUsedFunctionsList.clear();
	m_setUnUsedVariablesList.clear();
	m_setUnUsedColorSetsList.clear();
	m_setUnUsedObserverList.clear();
	m_setUnUsedSimpleCS.clear();
	m_setUnUsedCompoundCS.clear();
	m_setUnUsedAliasCS.clear();
	m_setFinalUnusedDec.clear();
	m_vConstantsInfo.clear();
	m_vSimpleCsInfo.clear();
	m_vCompoundCsInfo.clear();
	m_vFunctionsInfo.clear();
	m_vSubCsInfo.clear();
	m_vVariablesInfo.clear();
	m_mGroup2SelectedSet.clear();

	m_vtrimeddecConstants.clear();
	m_vtrimeddecFunctions.clear();
	m_vtrimeddecVariables.clear();
	m_vtrimeddecObservers.clear();
	m_vtrimeddecSimplCS.clear();
	m_vtrimeddecCompoundCS.clear();
	m_vtrimeddecAliasCS.clear();
	m_vtrimeddecAll.clear();
	m_vtrimeddecALLCs.clear();
	m_mDeclarations2Dependencies.clear();

	m_mDeclarations2Dependencies = {};
	m_vConstantsInfo = {};
	m_vSimpleCsInfo = {};
	m_vCompoundCsInfo = {};
	m_vFunctionsInfo = {};
	m_vSubCsInfo = {};
	m_vVariablesInfo = {};
	m_vObserversInfo = {};
	m_mGroup2SelectedSet = {};
}
