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
//#include "sp_ds/extensions/unusedDeclarationsFeature/SP_DS_UnUsedDeclaration.h"
//#include "sp_ds/extensions/unusedDeclarationsFeature/SP_DS_Graph_Declarations.h"

BEGIN_EVENT_TABLE(SP_WDG_DeclarationTreectrl, SP_WDG_Treectrl)

EVT_TREE_ITEM_ACTIVATED(SP_ID_COLORSETTREE_CTRL_ID, SP_WDG_DeclarationTreectrl::OnDoubleClick)
EVT_TREE_ITEM_GETTOOLTIP(SP_ID_COLORSETTREE_CTRL_ID, SP_WDG_DeclarationTreectrl::OnToolTip)
EVT_TREE_ITEM_RIGHT_CLICK(SP_ID_COLORSETTREE_CTRL_ID, SP_WDG_DeclarationTreectrl::OnRMouseClick) //Added by G.Assaf

EVT_MENU(SP_MENU_RITEM1_TREE, SP_WDG_DeclarationTreectrl::OnCheckForUnusedItemsNew) //Added by G.Assaf
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
	SP_DS_Graph_Declarations l_CheckDec(m_pcGraph);

	m_CheckDecDep = l_CheckDec;
	m_sSelected = wxT("");
	//m_setSelectedItems = {};
	//m_setSelectedConstants = {};
	//m_setSelectedfunctions = {};
	//m_setSelectedObservers = {};
	//m_setUnSelectedItems = {};
	//m_setUnSelectedContants = {};
	//m_setUnSelectedFunctions = {};
	//m_setUnSelectedObservers = {};
	//m_vUnUsedDecList = {};
	//m_vUnUsedConstantsList = {};
	///m_setUnUsedFunctionsList = {};
	///m_setUnUsedVariablesList = {};
	//m_setUnUsedColorSetsList = {};
	//m_setUnUsedObserverList = {};
	//m_setUnUsedSimpleCS = {};
	//m_setUnUsedCompoundCS = {};
	//m_setUnUsedAliasCS = {};
	//m_mDeclarations2Dependencies = {};
	//m_vConstantsInfo = {};
	//m_vSimpleCsInfo = {};
	//m_vCompoundCsInfo = {};
	//m_vFunctionsInfo = {};
	//m_vSubCsInfo = {};
	//m_vVariablesInfo = {};
	//m_vObserversInfo = {};
	//m_mGroup2SelectedSet = {};
	//m_vtrimeddecConstants = {};
	//m_vtrimeddecFunctions = {};
	//m_vtrimeddecVariables = {};
	//m_vtrimeddecObservers = {};
	//m_vtrimeddecSimplCS = {};
	//m_vtrimeddecCompoundCS = {};
	//m_vtrimeddecAliasCS = {};
	//m_vtrimeddecAll = {};
	//m_vtrimeddecALLCs = {};

#ifdef __WXMSW__
	m_ActivatedItem = wxTreeItemId();
#endif

}

SP_WDG_DeclarationTreectrl::~SP_WDG_DeclarationTreectrl()
{
	ClearAll();
}


void SP_WDG_DeclarationTreectrl::OnCheckForUnusedItemsNew(wxCommandEvent& p_cEvent)
{

	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();
	bool l_bIsColoured = false;

	wxTreeCtrl* pTreeCtrl = this;// m_pcGraph->GetDeclarationTree();
	wxTreeItemId l_root = pTreeCtrl->GetRootItem();

	if (l_netType.Contains(wxT("Colored")))
	{
		l_bIsColoured = true;
	}
	//REset the tree colouring
	MarkTreeWithDefaultColour(pTreeCtrl, l_root);

	SP_DS_Graph_Declarations l_CheckDec(m_pcGraph);
	m_CheckDecDep = l_CheckDec;

	m_CheckDecDep();

	l_pcConstantsDependencies = m_CheckDecDep.GetConstantsDependencyVector();
	l_pcColorSetDependencies = m_CheckDecDep.GetColorSetDependenncyVector();
	l_pcColorfunctionsDependencies = m_CheckDecDep.GetFunctionDependencyVector();
    l_pcVariableDependencies = m_CheckDecDep.GetVariableDependencyVector();
	l_pcObserverDependencies = m_CheckDecDep.GetObserverDependencyVector();

	  if (l_bIsColoured )//colored net
	{
		if (m_sSelected == wxT("Constants") )
		{
			unsigned int l_nConstCounter = 0;

			for (unsigned int i = 0; i < l_pcConstantsDependencies.size(); i++)
			{
				if (!l_pcConstantsDependencies[i]->isUsed)
				{
					l_nConstCounter++;

					wxString l_sLabel =l_pcConstantsDependencies[i]->key;
					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL

						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}
					//mark all its dependencies with red as well
					MarkTree(l_pcConstantsDependencies[i], pTreeCtrl, l_root, l_nColorRed);
				}
			}

			if (l_nConstCounter == 0)//report when no unused  consts found
			{
				wxString s_warningMsg = wxT("no unused declarations (constants and their groups that they belong to)");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else {
				wxString l_sFoundReport = wxT("the number of unused Constants is:");
				l_sFoundReport << l_nConstCounter < "\n";
				SP_LOGMESSAGE(l_sFoundReport);
			}
		}

		else if (m_sSelected == wxT("Color Sets"))
		{
			unsigned int l_nColorSetCounter = 0;
			for (unsigned int i = 0; i < l_pcColorSetDependencies.size(); i++)
			{
				if (!l_pcColorSetDependencies[i]->isUsed && l_pcColorSetDependencies[i]->key!=wxT("Dot"))
				{
					l_nColorSetCounter++;

					wxString l_sLabel = l_pcColorSetDependencies[i]->key;
					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL

						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}
					//mark all its dependencies with red as well
					MarkTree(l_pcColorSetDependencies[i], pTreeCtrl, l_root, l_nColorRed);
				}
			}

			if (l_nColorSetCounter == 0)
			{
				wxString s_warningMsg = wxT("no unused  color sets");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window

			}
			else {

				wxString l_sReport = wxT("the number of unused Color Sets is: ");
				l_sReport << l_nColorSetCounter << wxT("\n");
				SP_LOGMESSAGE(l_sReport);
			}
		}

		else if (m_sSelected == wxT("Functions") )
		{
			unsigned int l_nCoolorFunctions = 0;
			for (unsigned int i = 0; i < l_pcColorfunctionsDependencies.size(); i++)
			{
				if (!l_pcColorfunctionsDependencies[i]->isUsed)
				{
					l_nCoolorFunctions++;

					wxString l_sLabel = l_pcColorfunctionsDependencies[i]->key;

					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL

						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}
					//mark all its dependencies with red as well
					MarkTree(l_pcColorfunctionsDependencies[i], pTreeCtrl, l_root, l_nColorRed);
				}
			}

			if (l_nCoolorFunctions == 0)
			{
				wxString s_warningMsg = wxT("no unused  color sets");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window

			}
			else {

				wxString l_sReport = wxT("the number of unused Color functions Sets is: ");
				l_sReport << l_nCoolorFunctions << wxT("\n");
				SP_LOGMESSAGE(l_sReport);
			}
		}

		else if (m_sSelected == wxT("Variables") )
		{
			unsigned int l_nVariable = 0;
			for (unsigned int i = 0; i < l_pcVariableDependencies.size(); i++)
			{
				l_nVariable++;

				wxString l_sLabel = l_pcVariableDependencies[i]->key;

				wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
				wxColour l_nColorRed(wxT("RED"));
				if (foundId.IsOk()) {//!=NULL

					pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
				}
				//mark all its dependencies with red as well
				MarkTree(l_pcVariableDependencies[i], pTreeCtrl, l_root, l_nColorRed);
			}

			if (l_nVariable == 0)
			{
				wxString s_warningMsg = wxT("no unused variables");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else
			{
				wxString l_sReport = wxT("the number of unused variables is: ");
				l_sReport << l_nVariable << wxT("\n");
				SP_LOGMESSAGE(l_sReport);
			}
		}
		else {//all declarations
			if (m_sSelected == wxT("Declarations"))
			{
				unsigned int l_nDeclarationCount = 0;
				unsigned int l_nConst = 0;
				unsigned int l_nColorSet = 0;
				unsigned int l_nFun = 0;
				unsigned int l_nVar = 0;
				for (unsigned int i = 0; i < l_pcConstantsDependencies.size(); i++)
				{
					if (!l_pcConstantsDependencies[i]->isUsed)
					{
						l_nDeclarationCount++;
						l_nConst++;
						wxString l_sLabel = l_pcConstantsDependencies[i]->key;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						wxColour l_nColorRed(wxT("RED"));
						if (foundId.IsOk()) {//!=NULL

							pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
						}
					}
				}

				//color sets
				for (unsigned int i = 0; i < l_pcColorSetDependencies.size(); i++)
				{
					if (!l_pcColorSetDependencies[i]->isUsed && l_pcColorSetDependencies[i]->key != wxT("Dot"))
					{
						l_nDeclarationCount++;
						l_nColorSet++;
						wxString l_sLabel = l_pcColorSetDependencies[i]->key;
						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						wxColour l_nColorRed(wxT("RED"));
						if (foundId.IsOk()) {//!=NULL

							pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
						}
					}
				}
				//functions
				for (unsigned int i = 0; i < l_pcColorfunctionsDependencies.size(); i++)
				{
					if (!l_pcColorfunctionsDependencies[i]->isUsed)
					{
						l_nDeclarationCount++;
						l_nFun++;
						wxString l_sLabel = l_pcColorfunctionsDependencies[i]->key;

						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						wxColour l_nColorRed(wxT("RED"));
						if (foundId.IsOk()) {//!=NULL

							pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
						}
					}
				}
				//variables
				for (unsigned int i = 0; i < l_pcVariableDependencies.size(); i++)
				{
					if (!l_pcVariableDependencies[i]->isUsed)
					{
						l_nDeclarationCount++;
						l_nVar++;
						wxString l_sLabel = l_pcVariableDependencies[i]->key;

						wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
						wxColour l_nColorRed(wxT("RED"));
						if (foundId.IsOk()) {//!=NULL

							pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
						}
					}
				}

				if (l_nDeclarationCount==0)
				{
					wxString s_warningMsg = wxT("no unused declarations");
					SP_LOGMESSAGE(s_warningMsg);// add the message to log window
				}
				else
				{
					wxString l_sReport = wxT("the number of unused declarations is: ");
					l_sReport << l_nDeclarationCount << wxT("\n");
					l_sReport << wxT("number of unused constants is:")<<l_nConst<<wxT("\n");
					l_sReport << wxT("number of unused color sets is:") << l_nColorSet << wxT("\n");
					l_sReport << wxT("number of unused functions is:") << l_nFun << wxT("\n");
					l_sReport << wxT("number of unused variables is:") << l_nVar << wxT("\n");

					SP_LOGMESSAGE(l_sReport);
				}
			}
		}

	}

	if (!l_bIsColoured)//Plain PN net
	{
		if (m_sSelected == wxT("Constants"))
		{
			unsigned int l_nConstCounter = 0;
			for (unsigned int i = 0; i < l_pcConstantsDependencies.size(); i++)
			{
				if (!l_pcConstantsDependencies[i]->isUsed)
				{
					l_nConstCounter++;
					wxString l_sLabel = l_pcConstantsDependencies[i]->key;
					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL

						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}
					//mark all its dependencies with red as well
					MarkTree(l_pcConstantsDependencies[i], pTreeCtrl, l_root, l_nColorRed);
				}
			}

			if (l_nConstCounter == 0)//report when no unused  consts found
			{
				wxString s_warningMsg = wxT("no unused declarations constants");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else {
				wxString l_sFoundReport = wxT("the number of unused Constants is:");
				l_sFoundReport << l_nConstCounter < "\n";
				SP_LOGMESSAGE(l_sFoundReport);
			}
		}

		else if (m_sSelected == wxT("Functions"))
		{
			unsigned int l_nFunctionCounter = 0;
			for (unsigned int i = 0; i < l_pcColorfunctionsDependencies.size(); i++)
			{
				if (!l_pcColorfunctionsDependencies[i]->isUsed)
				{
					l_nFunctionCounter++;

					wxString l_sLabel = l_pcColorfunctionsDependencies[i]->key;

					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL

						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}
					//mark all its dependencies with red as well
					MarkTree(l_pcColorfunctionsDependencies[i], pTreeCtrl, l_root, l_nColorRed);
				}
			}

			if (!l_nFunctionCounter)
			{
				wxString s_warningMsg = wxT("no unused functions");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else
			{
				wxString l_sFoundReport;
				l_sFoundReport << wxT("the number of unused functions is:") << l_nFunctionCounter << wxT("\n");
				SP_LOGMESSAGE(l_sFoundReport);
			}
		}
		else if (m_sSelected == wxT("Observers"))
		{
			unsigned int l_nObservers = 0;
			for (unsigned int i = 0; i < l_pcObserverDependencies.size(); i++)
			{
				if (!l_pcObserverDependencies[i]->isUsed)
				{
					l_nObservers++;
					wxString l_sLabel = l_pcObserverDependencies[i]->key;
					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL
						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}

				}
			}
			if (!l_nObservers)
			{
				wxString s_warningMsg = wxT("no unused observers");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else
			{
				wxString l_sFoundReport;
				l_sFoundReport<<wxT("the number of unused observers is:") << l_nObservers << wxT("\n");;

				SP_LOGMESSAGE(l_sFoundReport);
			}
		}
		if (m_sSelected == wxT("Declarations"))
		{
			unsigned int l_nCountDec = 0;
			unsigned int l_nConst = 0;
			unsigned int l_nFun = 0;
			unsigned int l_nObservers = 0;

			for (unsigned int i = 0; i < l_pcConstantsDependencies.size(); i++)
			{
				if (!l_pcConstantsDependencies[i]->isUsed)
				{
					l_nCountDec++;
					l_nConst++;
					wxString l_sLabel = l_pcConstantsDependencies[i]->key;
					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL

						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}

				}
			}

			//functions
			for (unsigned int i = 0; i < l_pcColorfunctionsDependencies.size(); i++)
			{
				if (!l_pcColorfunctionsDependencies[i]->isUsed)
				{
					l_nCountDec++;
					l_nFun++;

					wxString l_sLabel = l_pcColorfunctionsDependencies[i]->key;

					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL

						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}

				}
			}
			//TODO add observers
			for (unsigned int i = 0; i < l_pcObserverDependencies.size(); i++)
			{
				if (!l_pcObserverDependencies[i]->isUsed)
				{
					l_nObservers++;
					wxString l_sLabel = l_pcObserverDependencies[i]->key;
					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL
						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}

				}
			}

			if (l_nCountDec == 0)
			{
				wxString s_warningMsg = wxT("no unused declarations");
				SP_LOGMESSAGE(s_warningMsg);// add the message to log window
			}
			else
			{
				wxString l_sFoundReport;
				l_sFoundReport<<wxT("the number of unused declaration is:") << l_nCountDec << wxT("\n");
				l_sFoundReport << wxT("number of unused constants is:") << l_nConst << wxT("\n");
				l_sFoundReport << wxT("number of unused functions is:") << l_nFun << wxT("\n");
				l_sFoundReport << wxT("number of unused observers is:") << l_nObservers << wxT("\n");
				SP_LOGMESSAGE(l_sFoundReport);
			}
		}

	}


}
 /**
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
*/
/**
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
*/
/**
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
*/

/**
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
*/
/**
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
*/
/**
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

					m_unUsedDec.DeleteUnusedColorSet(it_toBedel->first);
					m_mDeclarations2Dependencies.erase(it_toBedel->first);

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
*/
/**
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
*/
/**
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

						}
					}
					wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), it_toBedel->first, true, true);

					if (foundId.IsOk())
					{

					}
				}
				m_mDeclarations2Dependencies.erase(it_mapCs->first);


			}

		}

	}
}
*/
void SP_WDG_DeclarationTreectrl::OnCleanUnusedItems(wxCommandEvent& p_cEvent)//Added by G.Assaf
{

	if (m_sSelected == wxT("Constants") || m_sSelected == wxT("Declarations"))
	{
		for (unsigned int i = 0; i < l_pcConstantsDependencies.size(); i++)
		{
			if (!l_pcConstantsDependencies[i]->isUsed)
			{
				wxTreeItemId foundIdSelected = findTreeItem(this, this->GetRootItem(), l_pcConstantsDependencies[i]->key, true, true);
				wxColour l_nColorRed(wxT("Red"));
				wxColour l_itemColour = this->GetItemTextColour(foundIdSelected);
				if (l_itemColour == l_nColorRed)
				{
					if (m_CheckDecDep.DeleteConstant(l_pcConstantsDependencies[i]->key))
					{
						Delete(foundIdSelected);
						SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
					}

				}

			}
		}
	}

	if (m_sSelected == wxT("Functions")|| m_sSelected==wxT("Declarations"))
	{
		for (unsigned int i = 0; i < l_pcColorfunctionsDependencies.size(); i++)
		{
			if (!l_pcColorfunctionsDependencies[i]->isUsed)
			{
				wxTreeItemId foundIdSelected = findTreeItem(this, this->GetRootItem(), l_pcColorfunctionsDependencies[i]->key, true, true);
				wxColour l_nColorRed(wxT("Red"));
				wxColour l_itemColour = this->GetItemTextColour(foundIdSelected);
				if (l_itemColour == l_nColorRed)
				{
					if (m_CheckDecDep.DeleteFunction(l_pcColorfunctionsDependencies[i]->key))
					{
						Delete(foundIdSelected);
						SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
					}

				}

			}
		}
	}

	if (m_sSelected == wxT("Variables") || m_sSelected == wxT("Declarations"))
	{
		for (unsigned int i = 0; i < l_pcVariableDependencies.size(); i++)
		{
			if (!l_pcVariableDependencies[i]->isUsed)
			{
				wxTreeItemId foundIdSelected = findTreeItem(this, this->GetRootItem(), l_pcVariableDependencies[i]->key, true, true);
				wxColour l_nColorRed(wxT("Red"));
				wxColour l_itemColour = this->GetItemTextColour(foundIdSelected);
				if (l_itemColour == l_nColorRed)
				{
					if (m_CheckDecDep.DeleteVariable(l_pcVariableDependencies[i]->key))
					{
						Delete(foundIdSelected);
						SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
					}

				}

			}
		}
	}

	if (m_sSelected == wxT("Color Sets")|| m_sSelected == wxT("Declarations"))
	{
		for (unsigned int i = 0; i < l_pcColorSetDependencies.size(); i++)
		{
			if (!l_pcColorSetDependencies[i]->isUsed)
			{
				wxTreeItemId foundIdSelected = findTreeItem(this, this->GetRootItem(), l_pcColorSetDependencies[i]->key, true, true);
				wxColour l_nColorRed(wxT("Red"));
				wxColour l_itemColour = this->GetItemTextColour(foundIdSelected);
				if (l_itemColour == l_nColorRed)
				{
					if (m_CheckDecDep.DeleteColorSet(l_pcColorSetDependencies[i]->key))
					{
						Delete(foundIdSelected);
						SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
					}

				}

			}
		}
	}

	if (m_sSelected == wxT("Observers")|| m_sSelected == wxT("Declarations"))
	{
		for (unsigned int i = 0; i < l_pcObserverDependencies.size(); i++)
		{
			if (!l_pcObserverDependencies[i]->isUsed)
			{
				wxTreeItemId foundIdSelected = findTreeItem(this, this->GetRootItem(), l_pcObserverDependencies[i]->key, true, true);
				wxColour l_nColorRed(wxT("Red"));
				wxColour l_itemColour = this->GetItemTextColour(foundIdSelected);
				if (l_itemColour == l_nColorRed)
				{
					if (m_CheckDecDep.DeleteObserver(l_pcObserverDependencies[i]->key))
					{
						Delete(foundIdSelected);
						SP_Core::Instance()->GetRootDocument()->Modify(TRUE);
					}

				}

			}
		}
	}


	/**
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
	*/
}
void SP_WDG_DeclarationTreectrl::OnSelecting(wxCommandEvent& p_cEvent)
{
	set<wxString> l_setDependencies;

	wxTreeCtrl* pTreeCtrl = this;// m_pcGraph->GetDeclarationTree();
	wxTreeItemId l_root = pTreeCtrl->GetRootItem();

	//catch the selected Item from the tree
	wxTreeItemId foundIdSelected = findTreeItem(this, this->GetRootItem(), m_sSelected, true, true);
	wxTreeItemId l_itemParent;
	wxString l_sParentText;
	wxString l_sItem;

	if (foundIdSelected.IsOk())//determine parent node type
	{
		l_itemParent = this->GetItemParent(foundIdSelected);

		l_sParentText = GetItemText(l_itemParent);
	}
	//process the node name in the tree
	if (m_sSelected.Contains(":"))
	{//constnats
		l_sItem = m_sSelected.BeforeFirst(':');
		l_sItem=l_sItem.Trim();
		l_sItem = l_sItem.Trim(false);
	}
	else if (m_sSelected.Contains("("))
	{//functions
		l_sItem = m_sSelected.BeforeFirst('(');
		l_sItem = l_sItem.Trim();
		l_sItem = l_sItem.Trim(false);
	}
	else if (m_sSelected.Contains("="))
	{//observers
		l_sItem = m_sSelected.BeforeFirst('=');
		l_sItem = l_sItem.Trim();
		l_sItem = l_sItem.Trim(false);
	}
	else
	{//color sets
		l_sItem=m_sSelected;
	}

		if (l_sParentText.Contains(_T("Constants")))
		{
			for (unsigned int i = 0; i < l_pcConstantsDependencies.size(); i++)
			{
				if (!l_pcConstantsDependencies[i]->isUsed && l_pcConstantsDependencies[i]->key == l_sItem)
				{
					wxString l_sLabel = l_pcConstantsDependencies[i]->key;
					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL

						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}
					//mark all its dependencies with red as well
					MarkTree(l_pcConstantsDependencies[i], pTreeCtrl, l_root, l_nColorRed);
				}
			}
		}

		if (l_sParentText.Contains(_T("Color Sets")))
		{
			for (unsigned int i = 0; i < l_pcColorSetDependencies.size(); i++)
			{
				if (!l_pcColorSetDependencies[i]->isUsed&& l_pcColorSetDependencies[i]->key != (_T("Dot")) && l_pcColorSetDependencies[i]->key == l_sItem)
				{
					wxString l_sLabel = l_pcColorSetDependencies[i]->key;
					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL

						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}

					MarkTree(l_pcColorSetDependencies[i], pTreeCtrl, l_root, l_nColorRed);
				}
			}
		}
		if (l_sParentText.Contains(_T("Functions")))
		{
			for (unsigned int i = 0; i < l_pcColorfunctionsDependencies.size(); i++)
			{
				if (!l_pcColorfunctionsDependencies[i]->isUsed && l_pcColorfunctionsDependencies[i]->key == l_sItem)
				{
					wxString l_sLabel = l_pcColorfunctionsDependencies[i]->key;
					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL

						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}

					MarkTree(l_pcColorfunctionsDependencies[i], pTreeCtrl, l_root, l_nColorRed);
				}
			}
		}

		if (l_sParentText.Contains(_T("Variables")))
		{
			for (unsigned int i = 0; i < l_pcVariableDependencies.size(); i++)
			{
				if (!l_pcVariableDependencies[i]->isUsed && l_pcVariableDependencies[i]->key == l_sItem)
				{
					wxString l_sLabel = l_pcVariableDependencies[i]->key;
					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL

						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}

					MarkTree(l_pcVariableDependencies[i], pTreeCtrl, l_root, l_nColorRed);
				}
			}
		}

		if (l_sParentText.Contains(_T("Observers")))
		{
			for (unsigned int i = 0; i < l_pcObserverDependencies.size(); i++)
			{
				if (!l_pcObserverDependencies[i]->isUsed && l_pcObserverDependencies[i]->key == l_sItem)
				{
					wxString l_sLabel = l_pcObserverDependencies[i]->key;
					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL

						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}
				}
			}

		}
}

void  SP_WDG_DeclarationTreectrl::OnSelectAll(wxCommandEvent& p_cEvent)
{

	set<wxString> l_setDependencies;

	wxTreeCtrl* pTreeCtrl = this;// m_pcGraph->GetDeclarationTree();
	wxTreeItemId l_root = pTreeCtrl->GetRootItem();

	//get the parent of the selected node
	wxTreeItemId foundIdSelected = findTreeItem(this, this->GetRootItem(), m_sSelected, true, true);
	wxTreeItemId l_itemParent;
	wxString l_sParentText;
	wxString l_sItem;
	if (foundIdSelected.IsOk())
	{
		l_itemParent = this->GetItemParent(foundIdSelected);

		l_sParentText = GetItemText(l_itemParent);
	}
	else
	{
		return;
	}

		if (m_sSelected== _T("Constants") || m_sSelected == _T("Declarations"))
		{
			for (unsigned int i = 0; i < l_pcConstantsDependencies.size(); i++)
			{
				if (!l_pcConstantsDependencies[i]->isUsed)
				{
					wxString l_sLabel = l_pcConstantsDependencies[i]->key;
					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL

						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}
					//mark all its dependencies with red as well
					MarkTree(l_pcConstantsDependencies[i], pTreeCtrl, l_root, l_nColorRed);
				}
			}
		}

		if (m_sSelected == (_T("Color Sets")) || m_sSelected == _T("Declarations"))
		{
			for (unsigned int i = 0; i < l_pcColorSetDependencies.size(); i++)
			{
				if (!l_pcColorSetDependencies[i]->isUsed&& l_pcColorSetDependencies[i]->key != (_T("Dot")))
				{
					wxString l_sLabel = l_pcColorSetDependencies[i]->key;
					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL

						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}

					MarkTree(l_pcColorSetDependencies[i], pTreeCtrl, l_root, l_nColorRed);
				}
			}
		}
		if (m_sSelected ==  _T("Functions")|| m_sSelected == _T("Declarations"))
		{
			for (unsigned int i = 0; i < l_pcColorfunctionsDependencies.size(); i++)
			{
				if (!l_pcColorfunctionsDependencies[i]->isUsed)
				{
					wxString l_sLabel = l_pcColorfunctionsDependencies[i]->key;
					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL

						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}

					MarkTree(l_pcColorfunctionsDependencies[i], pTreeCtrl, l_root, l_nColorRed);
				}
			}
		}

		if (m_sSelected == _T("Variables") || m_sSelected == _T("Declarations"))
		{
			for (unsigned int i = 0; i < l_pcVariableDependencies.size(); i++)
			{
				if (!l_pcVariableDependencies[i]->isUsed)
				{
					wxString l_sLabel = l_pcVariableDependencies[i]->key;
					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL

						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}

					MarkTree(l_pcVariableDependencies[i], pTreeCtrl, l_root, l_nColorRed);
				}
			}
		}

		if (m_sSelected == _T("Observers") || m_sSelected == _T("Declarations"))
		{
			for (unsigned int i = 0; i < l_pcObserverDependencies.size(); i++)
			{
				if (!l_pcObserverDependencies[i]->isUsed)
				{
					wxString l_sLabel = l_pcObserverDependencies[i]->key;
					wxTreeItemId foundId = findTreeItem(pTreeCtrl, l_root, l_sLabel, true, true);
					wxColour l_nColorRed(wxT("RED"));
					if (foundId.IsOk()) {//!=NULL

						pTreeCtrl->SetItemTextColour(foundId, l_nColorRed);
					}
				}
			}
		}
}

void  SP_WDG_DeclarationTreectrl::OnUnSelectAll(wxCommandEvent& p_cEvent)
{
	wxString l_sGroup = m_sSelected;
	std::set<wxString> l_setSelectedGroup;

		m_bisSecond = true;
		set<wxString> l_setDependenciesUnselected;
		//colored class
		//do unselect all on Constants
		l_pcItem2[1]->Enable(false);
		l_pcItem2[2]->Enable(true);
		if (l_sGroup == wxT("Constants"))
		{

			for (unsigned int i = 0; i < l_pcConstantsDependencies.size(); i++)
			{
				if (!l_pcConstantsDependencies[i]->isUsed)
				{
					wxString l_sConstant = l_pcConstantsDependencies[i]->key;
					m_CheckDecDep.ComputeBackwardDependency(l_sConstant, NODE_TYPE::CONSTANT, l_setDependenciesUnselected);

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

				}
			}
		}

		else if (l_sGroup == wxT("Variables"))
		{//do unselect all only on variables

			for (unsigned int i = 0; i < l_pcVariableDependencies.size(); i++)
			{
				if (!l_pcVariableDependencies[i]->isUsed)
				{
					wxString l_svariable = l_pcVariableDependencies[i]->key;
					m_CheckDecDep.ComputeBackwardDependency(l_svariable, NODE_TYPE::VAR, l_setDependenciesUnselected);

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

				}
			}
		}
		else if (l_sGroup == wxT("Functions"))
		{//do unselect all only on functions
			for (unsigned int i = 0; i < l_pcColorfunctionsDependencies.size(); i++)
			{
				if (!l_pcColorfunctionsDependencies[i]->isUsed)
				{
					wxString l_sfun = l_pcColorfunctionsDependencies[i]->key;
					m_CheckDecDep.ComputeBackwardDependency(l_sfun, NODE_TYPE::FUN_COLOOR, l_setDependenciesUnselected);

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

				}
			}
		}


		else if (l_sGroup == wxT("Simple Color Sets"))
		{//do unselect all only on Simple cs

		}


		else if (l_sGroup == wxT("Compound Color Sets"))
		{//do unselect all only on Simple cs

		}

		else if (l_sGroup == wxT("Alias Color Sets"))
		{//do unselect all only on Simple cs

		}

		else if (l_sGroup == wxT("Color Sets"))
		{//do unselect all only on cs

			for (unsigned int i = 0; i < l_pcColorSetDependencies.size(); i++)
			{
				if (!l_pcColorSetDependencies[i]->isUsed)
				{
					wxString l_sColorSet = l_pcColorSetDependencies[i]->key;
					m_CheckDecDep.ComputeBackwardDependency(l_sColorSet, NODE_TYPE::COLOR_SET, l_setDependenciesUnselected);

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

				}
			}

		}

		else if (l_sGroup == wxT("Observers"))
		{
			for (unsigned int i = 0; i < l_pcObserverDependencies.size(); i++)
			{
				if (!l_pcObserverDependencies[i]->isUsed)
				{
					wxString l_sfun = l_pcObserverDependencies[i]->key;
					m_CheckDecDep.ComputeBackwardDependency(l_sfun, NODE_TYPE::FUN_COLOOR, l_setDependenciesUnselected);

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

				}
			}
		}
		if (l_sGroup == wxT("Declarations"))
		{
			//constants sets
			for (unsigned int i = 0; i < l_pcConstantsDependencies.size(); i++)
			{
				if (!l_pcConstantsDependencies[i]->isUsed)
				{
					wxString l_sConstant = l_pcConstantsDependencies[i]->key;
					m_CheckDecDep.ComputeBackwardDependency(l_sConstant, NODE_TYPE::CONSTANT, l_setDependenciesUnselected);

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

				}
			}
			//color sets
			for (unsigned int i = 0; i < l_pcColorSetDependencies.size(); i++)
			{
				if (!l_pcColorSetDependencies[i]->isUsed)
				{
					wxString l_sColorSet = l_pcColorSetDependencies[i]->key;
					m_CheckDecDep.ComputeBackwardDependency(l_sColorSet, NODE_TYPE::COLOR_SET, l_setDependenciesUnselected);

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

				}
			}

				//variables
				for (unsigned int i = 0; i < l_pcVariableDependencies.size(); i++)
				{
					if (!l_pcVariableDependencies[i]->isUsed)
					{
						wxString l_svariable = l_pcVariableDependencies[i]->key;
						m_CheckDecDep.ComputeBackwardDependency(l_svariable, NODE_TYPE::VAR, l_setDependenciesUnselected);

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

					}
				}

				//functions
				for (unsigned int i = 0; i < l_pcColorfunctionsDependencies.size(); i++)
				{
					if (!l_pcColorfunctionsDependencies[i]->isUsed)
					{
						wxString l_sfun = l_pcColorfunctionsDependencies[i]->key;
						m_CheckDecDep.ComputeBackwardDependency(l_sfun, NODE_TYPE::FUN_COLOOR, l_setDependenciesUnselected);

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

					}
				}

				//observers
				for (unsigned int i = 0; i < l_pcObserverDependencies.size(); i++)
				{
					if (!l_pcObserverDependencies[i]->isUsed)
					{
						wxString l_sfun = l_pcObserverDependencies[i]->key;
						m_CheckDecDep.ComputeBackwardDependency(l_sfun, NODE_TYPE::FUN_COLOOR, l_setDependenciesUnselected);

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

					}
				}

		}//colse all declarations

}
void  SP_WDG_DeclarationTreectrl::OnUnSelecting(wxCommandEvent& p_cEvent)
{
	NODE_TYPE l_nType;
	wxTreeItemId foundIdSelected = findTreeItem(this, this->GetRootItem(), m_sSelected, true, true);
	wxTreeItemId l_itemParent;
	if (foundIdSelected.IsOk())
	{
		l_itemParent=this->GetItemParent(foundIdSelected);

	wxString l_sParentText=	GetItemText(l_itemParent);
	if (l_sParentText.Contains(_T("Constants")))
	{
		l_nType = NODE_TYPE::CONSTANT;
	}
	else if (l_sParentText.Contains(_T("Color Set")))
	{
		l_nType = NODE_TYPE::COLOR_SET;
	}
	else if (l_sParentText.Contains(_T("Variables")))
	{
		l_nType = NODE_TYPE::VAR;
	}
	else if (l_sParentText.Contains(_T("Observers")))
	{
		l_nType = NODE_TYPE::OBSERVER;
	}
	else
	{
		l_nType = NODE_TYPE::FUN_COLOOR;
	}
	}

	set<wxString> l_setDependenciesUnselected;
	if (!m_pcGraph->GetNetclass()->GetDisplayName().Contains("Colored"))
	{
		//uncolored pn
		//l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(m_sSelected);

		m_CheckDecDep.ComputeBackwardDependency(m_sSelected, l_nType, l_setDependenciesUnselected);

		for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
		{
			wxColour l_nColor1(wxT("Blue"));
			wxColour l_nColorRed(wxT("Red"));

			wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
			if (foundId.IsOk())
			{
				if(this->GetItemTextColour(foundId) == l_nColorRed){
				this->SetItemTextColour(foundId, l_nColor1);

				l_pcItem1[0]->Enable(true);
				l_pcItem1[1]->Enable(false);
				}

			}
		}


	}
	else {
		//	colored PN
		//individual colored item un-selection
		m_CheckDecDep.ComputeBackwardDependency(m_sSelected, l_nType, l_setDependenciesUnselected);

		//set<wxString> l_setDependenciesUnselected = ObtainItemDependenciesForUnSelect(m_sSelected);
		for (auto itset = l_setDependenciesUnselected.begin(); itset != l_setDependenciesUnselected.end(); ++itset)
		{
			wxColour l_nColor1(wxT("Blue"));
			wxColour l_nColorRed(wxT("Red"));
			wxTreeItemId foundId = findTreeItem(this, this->GetRootItem(), *itset, true, true);
			if (foundId.IsOk())
			{
				if(this->GetItemTextColour(foundId) == l_nColorRed){
				this->SetItemTextColour(foundId, l_nColor1);

				l_pcItem1[0]->Enable(true);
				l_pcItem1[1]->Enable(false);

				}
			}
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


	}

	return l_mResult;
}

void SP_WDG_DeclarationTreectrl::OnRMouseClick(wxTreeEvent& p_cEvent)
{


	wxTreeItemId itemSelected = p_cEvent.GetItem();
	m_sSelected = GetItemText(itemSelected);
	if (!this->HasChildren(itemSelected))
	{
		wxColour l_itemColor = this->GetItemTextColour(itemSelected);
		wxColour l_colorRed(wxT("Red"));
		wxColour l_colorBlue(wxT("Blue"));
		if (l_itemColor == l_colorRed)
		{
			l_pcItem1[0]->Enable(false);
			l_pcItem1[1]->Enable(true);
		}
		else if(l_itemColor== l_colorBlue){
			l_pcItem1[0]->Enable(true);
			l_pcItem1[1]->Enable(false);
		}
		else
		{
			l_pcItem1[0]->Enable(false);
			l_pcItem1[1]->Enable(false);
		}
		PopupMenu(&l_cMenuOptions);
		l_pcItem1 = l_cMenuOptions.GetMenuItems();
		return;
	}
	//////////////////////////////////////////////////
	wxString l_netType = m_pcGraph->GetNetclass()->GetDisplayName();

	bool l_bIsColoured = false;
	map<wxString, int> l_mSelection2Num;

	if (l_netType.Contains(wxT("Colored")))
	{
		l_bIsColoured = true;
	}
	wxTreeItemId item = p_cEvent.GetItem();
	m_sSelected = GetItemText(item);



	map<wxString, int> l_mSelection2NumSimple;
	map<wxString, int> l_mSelection2NumCompound;
	map<wxString, int> l_mSelection2NumAlias;
	map<wxString, int> l_mSelection2NumConst;
	map<wxString, int> l_mSelection2NumFun;
	map<wxString, int> l_mSelection2NumVar;
	map<wxString, int> l_mSelection2NumObservers;

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


		PopupMenu(&l_cMenu);
	}

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
		bool l_bIsFound = false;
		/**
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
		*/
		if (!l_bIsFound)
		{/**
			auto f_it = m_vtrimeddecFunctions.find(m_sSelected);
			if (f_it != m_vtrimeddecFunctions.end())
			{
				l_mapTrimedDec = m_vtrimeddecFunctions;
				l_setUnusedList = m_setUnUsedFunctionsList;
				l_bIsFound = true;
				l_sSearch = (f_it->second).Trim();
			}
			*/
		}

		if (!l_bIsFound)
		{
			/**
			auto f_it = m_vtrimeddecObservers.find(m_sSelected);
			if (f_it != m_vtrimeddecObservers.end())
			{
				l_mapTrimedDec = m_vtrimeddecObservers;
				l_setUnusedList = m_setUnUsedObserverList;
				l_bIsFound = true;
				l_sSearch = (f_it->second).Trim();
			}
			*/
		}
		if (!l_bIsFound)
		{
			/**
			auto f_it = m_vtrimeddecVariables.find(m_sSelected);
			if (f_it != m_vtrimeddecVariables.end())
			{
				l_mapTrimedDec = m_vtrimeddecVariables;
				l_setUnusedList = m_setUnUsedVariablesList;
				l_bIsFound = true;
				l_sSearch = (f_it->second).Trim();
			}
			*/
		}

		if (!l_bIsFound)
		{/**
			auto f_it = m_vtrimeddecSimplCS.find(m_sSelected);
			if (f_it != m_vtrimeddecSimplCS.end())
			{
				l_mapTrimedDec = m_vtrimeddecSimplCS;
				l_setUnusedList = m_setUnUsedSimpleCS;
				l_bIsFound = true;
				l_sSearch = (f_it->second).Trim();
			}
			*/
		}

		if (!l_bIsFound)
		{/**
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
			*/
		}
		if (l_bIsFound) {
			//auto it_found = l_setUnusedList.find(l_sSearch);
			/**
			if (it_found == l_setUnusedList.end())
			{
				l_pcItem1[0]->Enable(false);
				l_pcItem1[1]->Enable(false);

			}
			else {
				l_pcItem1[0]->Enable(true);
				l_pcItem1[1]->Enable(true);

				wxTreeItemId item = p_cEvent.GetItem();
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
			*/
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
			if (l_sNetClassName.Contains(wxT("Fuzzy")))
			{
				SP_DLG_FpnConstantDefinition l_cConstantDlg(NULL);
				l_cConstantDlg.ShowModal();
			}
			else
			{
				SP_DLG_ConstantDefinition l_cConstantDlg(NULL);
				l_cConstantDlg.ShowModal();
			}
		
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

//	m_pcCheckDecDep->operator();

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
				else if (l_sMetadataType.Cmp(wxT("BFN")) == 0 && (netClass == SP_DS_FUZZYSPN_CLASS || netClass == SP_DS_FUZZYHPN_CLASS || netClass == SP_DS_FUZZYCPN_CLASS || netClass==SP_DS_FUZZY_ColCPN_CLASS|| netClass == SP_DS_FUZZY_ColSPN_CLASS|| netClass == SP_DS_FUZZY_ColHPN_CLASS))
				{
					l_sMetadataValue << wxT("(") << l_pcColList->GetCell(i, 1) << wxT(");");

				}
				else if (l_sMetadataType.Cmp(wxT("TZN")) == 0 && (netClass == SP_DS_FUZZYSPN_CLASS || netClass == SP_DS_FUZZYHPN_CLASS || netClass == SP_DS_FUZZYCPN_CLASS || netClass == SP_DS_FUZZY_ColCPN_CLASS || netClass == SP_DS_FUZZY_ColSPN_CLASS || netClass == SP_DS_FUZZY_ColHPN_CLASS))
				{
					l_sMetadataValue << wxT("(") << l_pcColList->GetCell(i, 1) << wxT(");");

				}
				else
				{
					l_sMetadataValue << l_pcColList->GetCell(i, 1) << wxT(";");

				}
			}
			////////////////
			if ((l_sMetadataType.Cmp(wxT("TFN")) == 0 ) && (netClass == SP_DS_FUZZYSPN_CLASS
					|| netClass == SP_DS_FUZZYHPN_CLASS || netClass == SP_DS_FUZZYCPN_CLASS
					|| netClass == SP_DS_FUZZY_ColCPN_CLASS || netClass == SP_DS_FUZZY_ColSPN_CLASS
					|| netClass == SP_DS_FUZZY_ColHPN_CLASS))
			{
				m_ColorsMap[l_sMetadataName] = l_sMetadataGroup + wxT(" : ") + l_sMetadataValue;
				AppendFileItem(l_cSecondChild, l_sMetadataName + l_sMetadataValue, NULL);
				wxString msg = " a new constant (TFN) is defined  " + l_sMetadataName + ":" + l_sMetadataValue;
				SP_LOGMESSAGE(msg);
			}
			/////////////////

			else {
				if ((netClass != SP_DS_FUZZYSPN_CLASS || netClass != SP_DS_FUZZYHPN_CLASS || netClass != SP_DS_FUZZYCPN_CLASS || netClass != SP_DS_FUZZY_ColCPN_CLASS || netClass != SP_DS_FUZZY_ColSPN_CLASS || netClass != SP_DS_FUZZY_ColHPN_CLASS) && !l_sMetadataValue.Contains(","))
				{

					m_ColorsMap[l_sMetadataName] = l_sMetadataGroup + wxT(" : ") + l_sMetadataValue;
					AppendFileItem(l_cSecondChild, l_sMetadataName + wxT(" : ") + l_sMetadataValue, NULL);
				}
				else if (netClass == SP_DS_FUZZYSPN_CLASS || netClass == SP_DS_FUZZYHPN_CLASS || netClass == SP_DS_FUZZYCPN_CLASS || netClass == SP_DS_FUZZY_ColCPN_CLASS || netClass == SP_DS_FUZZY_ColSPN_CLASS || netClass == SP_DS_FUZZY_ColHPN_CLASS)
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

///////////////////////////////////////////////////////////////////

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
	//wxTreeItemId l_cFirstChild_FirstChild = GetFirstChild(l_cFirstChild, l_nCookie);
	DeleteChildren(l_cFirstChild);
	SP_DS_Graph* l_pcGraph = SP_Core::Instance()->GetRootDocument()->GetGraph();
	if (!l_pcGraph)
		return;
	SP_DS_Metadataclass* l_pcMetadataclass = NULL;
	SP_DS_Metadata* l_pcNewMetadata = NULL;

	wxString l_sMetaClass;
	if (l_pcGraph->GetNetclass()->GetName().Contains(wxT("Fuzzy")))
	{
		l_sMetaClass = SP_DS_META_CONSTANT;
	}
	else
	{
		l_sMetaClass = SP_DS_CPN_CONSTANT_HARMONIZING;
	}
	SP_DS_Metadataclass* m_pcConstants = l_pcGraph->GetMetadataclass(l_sMetaClass);
	if (m_pcConstants) {
		wxString netClass = l_pcGraph->GetNetclass()->GetName();
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
				wxString l_sMetadataValue;
				if (l_sMetadataType == wxT("TFN"))//
				{
					if (l_sMetadataType.Cmp(wxT("TFN")) == 0 && (netClass == SP_DS_FUZZY_ColCPN_CLASS || netClass == SP_DS_FUZZY_ColSPN_CLASS || netClass == SP_DS_FUZZY_ColHPN_CLASS))
					{


						l_sMetadataValue << wxT("(") << l_pcColList->GetCell(i, 1) << wxT(");");
					}
					else if (l_sMetadataType.Cmp(wxT("BFN")) == 0 && (netClass == SP_DS_FUZZYSPN_CLASS || netClass == SP_DS_FUZZYHPN_CLASS || netClass == SP_DS_FUZZYCPN_CLASS || netClass == SP_DS_FUZZY_ColCPN_CLASS || netClass == SP_DS_FUZZY_ColSPN_CLASS || netClass == SP_DS_FUZZY_ColHPN_CLASS))
					{
						l_sMetadataValue << wxT("(") << l_pcColList->GetCell(i, 1) << wxT(");");

					}
					else if (l_sMetadataType.Cmp(wxT("TZN")) == 0 && (netClass == SP_DS_FUZZYSPN_CLASS || netClass == SP_DS_FUZZYHPN_CLASS || netClass == SP_DS_FUZZYCPN_CLASS || netClass == SP_DS_FUZZY_ColCPN_CLASS || netClass == SP_DS_FUZZY_ColSPN_CLASS || netClass == SP_DS_FUZZY_ColHPN_CLASS))
					{
						l_sMetadataValue << wxT("(") << l_pcColList->GetCell(i, 1) << wxT(");");

					}
					else
					{
						l_sMetadataValue << l_pcColList->GetCell(i, 1) << wxT(";");

					}
				}//
				if ((l_sMetadataType.Cmp(wxT("TFN")) == 0 || l_sMetadataType.Cmp(wxT("BFN")) == 0 || l_sMetadataType.Cmp(wxT("TZN")) == 0) && (netClass == SP_DS_FUZZYSPN_CLASS || netClass == SP_DS_FUZZYHPN_CLASS || netClass == SP_DS_FUZZYCPN_CLASS || netClass == SP_DS_FUZZY_ColCPN_CLASS || netClass == SP_DS_FUZZY_ColSPN_CLASS || netClass == SP_DS_FUZZY_ColHPN_CLASS))
				{
					m_ColorsMap[l_sMetadataName] = l_sMetadataGroup + wxT(" : ") + l_sMetadataValue;
					AppendFileItem(l_cFirstChild, l_sMetadataName + l_sMetadataValue, NULL);
					wxString msg = "new" + l_sMetadataType + " Fuzzy Number is defined  " + l_sMetadataName + ":" + l_sMetadataValue;
					SP_LOGMESSAGE(msg);
					break;
				}
				if (l_sMetadataType == wxT("int") && i == 0)
				{
					m_ColorsMap[l_sMetadataName] = l_sMetadataType + wxT(":") + l_sValue;
				}
				l_sVsets << l_sValue << ";";
				if (i == l_pcColList->GetRowCount() - 1)
				{
					AppendFileItem(l_cFirstChild, l_sMetadataName + wxT(":") + l_sVsets, NULL);
					break;
				}
			}

		}
	}


	////////////////////ccolor sets///////////////////////////
	l_cRootId = GetRootItem();
	GetFirstChild(l_cRootId, l_nCookie);//const
	wxTreeItemId l_cSecondChild = GetNextChild(l_cRootId, l_nCookie);//color sets
	wxString itemtext = this->GetItemText(l_cSecondChild);

	wxTreeItemId l_l_cSecondChild_l_cSecondChild = GetFirstChild(l_cSecondChild, l_nCookie);//simple sets

	wxString itemtext1 = this->GetItemText(l_l_cSecondChild_l_cSecondChild);

	DeleteChildren(l_l_cSecondChild_l_cSecondChild);


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

		AppendFileItem(l_l_cSecondChild_l_cSecondChild, l_sName, NULL);
	}


	wxTreeItemId l_csecondChild_thirdChild = GetNextChild(l_cSecondChild, l_nCookie);
	wxString itemtextaprod = this->GetItemText(l_csecondChild_thirdChild);

	DeleteChildren(l_csecondChild_thirdChild);

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

		AppendFileItem(l_csecondChild_thirdChild, l_sName, NULL);
	}


	wxTreeItemId l_csecondChild_fourth = GetNextChild(l_cSecondChild, l_nCookie);
	wxString itemtextal= this->GetItemText(l_csecondChild_fourth);

	DeleteChildren(l_csecondChild_fourth);


	///////////////////////////
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
		AppendFileItem(l_csecondChild_fourth, l_sName, NULL);
	}



	l_cRootId = GetRootItem();
	GetFirstChild(l_cRootId, l_nCookie);
	GetNextChild(l_cRootId, l_nCookie);
	wxTreeItemId l_cSecondChildfun3 = GetNextChild(l_cRootId, l_nCookie);

	DeleteChildren(l_cSecondChildfun3);
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
		AppendFileItem(l_cSecondChildfun3, l_sName, NULL);
	}

	GetFirstChild(l_cRootId,l_nCookie);
	GetNextChild(l_cRootId,l_nCookie);
	GetNextChild(l_cRootId,l_nCookie);
	wxTreeItemId l_cFourthChildvar=GetNextChild(l_cRootId,l_nCookie);
	DeleteChildren(l_cFourthChildvar);


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
		AppendFileItem(l_cFourthChildvar, l_sName, NULL);
	}

    //observers parent node

	    GetFirstChild(l_cRootId,l_nCookie);
		GetNextChild(l_cRootId,l_nCookie);
		GetNextChild(l_cRootId,l_nCookie);
		GetNextChild(l_cRootId,l_nCookie);
		wxTreeItemId l_cFifthChildobserver1=GetNextChild(l_cRootId,l_nCookie);
		DeleteChildren(l_cFifthChildobserver1);
		 //SP_MESSAGEBOX(this->GetItemText(l_cFifthChildobserver1));


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
				AppendFileItem(l_cFifthChildobserver1, l_sName + wxT(" = ") + l_sBody, NULL);
			}
		}

	/*******************************************************/

	//update the real color of a place
	SP_GUI_Canvas *l_cCanvas = NULL;
	wxString l_sNetClassName = l_pcGraph->GetNetclass()->GetName();
	wxString l_sPlaceNodeClassName;

	if (l_sNetClassName == SP_DS_COLCPN_CLASS || l_sNetClassName == SP_DS_FUZZY_ColCPN_CLASS)//by george
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



	//m_unUsedDec = {};//reset the object SP_DS_UnUsedDeclaration();
	//ResetCheckingResults();
	//this->ExpandAll();
}


///////////////////////////////////////////////////////////////////
 /**
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

	wxString l_sMetaClass;
	if (l_pcGraph->GetNetclass()->GetName().Contains(wxT("Fuzzy")))
	{
		l_sMetaClass = SP_DS_META_CONSTANT;
	}
	else
	{
		l_sMetaClass = SP_DS_CPN_CONSTANT_HARMONIZING;
	}
	SP_DS_Metadataclass* m_pcConstants = l_pcGraph->GetMetadataclass(l_sMetaClass);
	if (m_pcConstants) {
		wxString netClass = l_pcGraph->GetNetclass()->GetName();
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
				wxString l_sMetadataValue;
				if (l_sMetadataType==wxT("TFN"))//
				{
					if (l_sMetadataType.Cmp(wxT("TFN")) == 0 && (netClass == SP_DS_FUZZY_ColCPN_CLASS || netClass == SP_DS_FUZZY_ColSPN_CLASS || netClass == SP_DS_FUZZY_ColHPN_CLASS))
					{


						l_sMetadataValue << wxT("(") << l_pcColList->GetCell(i, 1) << wxT(");");
					}
					else if (l_sMetadataType.Cmp(wxT("BFN")) == 0 && (netClass == SP_DS_FUZZYSPN_CLASS || netClass == SP_DS_FUZZYHPN_CLASS || netClass == SP_DS_FUZZYCPN_CLASS || netClass == SP_DS_FUZZY_ColCPN_CLASS || netClass == SP_DS_FUZZY_ColSPN_CLASS || netClass == SP_DS_FUZZY_ColHPN_CLASS))
					{
						l_sMetadataValue << wxT("(") << l_pcColList->GetCell(i, 1) << wxT(");");

					}
					else if (l_sMetadataType.Cmp(wxT("TZN")) == 0 && (netClass == SP_DS_FUZZYSPN_CLASS || netClass == SP_DS_FUZZYHPN_CLASS || netClass == SP_DS_FUZZYCPN_CLASS || netClass == SP_DS_FUZZY_ColCPN_CLASS || netClass == SP_DS_FUZZY_ColSPN_CLASS || netClass == SP_DS_FUZZY_ColHPN_CLASS))
					{
						l_sMetadataValue << wxT("(") << l_pcColList->GetCell(i, 1) << wxT(");");

					}
					else
					{
						l_sMetadataValue << l_pcColList->GetCell(i, 1) << wxT(";");

					}
				}//
				if ((l_sMetadataType.Cmp(wxT("TFN")) == 0 || l_sMetadataType.Cmp(wxT("BFN")) == 0 || l_sMetadataType.Cmp(wxT("TZN")) == 0) && (netClass == SP_DS_FUZZYSPN_CLASS || netClass == SP_DS_FUZZYHPN_CLASS || netClass == SP_DS_FUZZYCPN_CLASS || netClass == SP_DS_FUZZY_ColCPN_CLASS || netClass == SP_DS_FUZZY_ColSPN_CLASS || netClass == SP_DS_FUZZY_ColHPN_CLASS))
				{
					m_ColorsMap[l_sMetadataName] = l_sMetadataGroup + wxT(" : ") + l_sMetadataValue;
					AppendFileItem(l_cSecondChild, l_sMetadataName + l_sMetadataValue, NULL);
					wxString msg = "new" + l_sMetadataType + " Fuzzy Number is defined  " + l_sMetadataName + ":" + l_sMetadataValue;
					SP_LOGMESSAGE(msg);
					break;
				}
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

	if (l_sNetClassName == SP_DS_COLCPN_CLASS|| l_sNetClassName==SP_DS_FUZZY_ColCPN_CLASS)//by george
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

 */
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

void SP_WDG_DeclarationTreectrl::MarkTreeWithDefaultColour(wxTreeCtrl* pTreeCtrl, const wxTreeItemId& root)
{
	wxTreeItemId item = root, child;
	wxTreeItemIdValue cookie;
	wxString  itemtext;
	bool bFound = false;;
	//if (!bCaseSensitive) findtext.MakeLower();

	while (item.IsOk())
	{
		itemtext = pTreeCtrl->GetItemText(item);

		if (!(itemtext == wxT("Constants") || itemtext == wxT("Declarations") ||
			itemtext == wxT("Functions") || itemtext == wxT("Color Sets")
			|| itemtext == wxT("Simple Color Sets") || itemtext == wxT("Compound Color Sets") || itemtext == wxT("Alias Color Sets") || itemtext == wxT("Observers")
			))
		{
			wxColor l_cCpolor(wxT("Black"));
			pTreeCtrl->SetItemTextColour(item, l_cCpolor);
		}

		//if (!bCaseSensitive) itemtext.MakeLower();
		//bFound = bExactMatch ? (itemtext == findtext) : itemtext.Contains(findtext);
		//if (bFound) return item;
		child = pTreeCtrl->GetFirstChild(item, cookie);
		if (child.IsOk()) MarkTreeWithDefaultColour(pTreeCtrl, child);
		//if (child.IsOk()) return child;
		item = pTreeCtrl->GetNextSibling(item);
	} // while(item.IsOk())

}

wxTreeItemId SP_WDG_DeclarationTreectrl::findTreeItem(wxTreeCtrl* pTreeCtrl, const wxTreeItemId& root, const wxString& text, bool bCaseSensitive, bool bExactMatch)
{
	wxTreeItemId item = root, child;
	wxTreeItemIdValue cookie;
	wxString findtext(text), itemtext;
	bool bFound = false;;
	//if (!bCaseSensitive) findtext.MakeLower();

	while (item.IsOk())
	{
		itemtext = pTreeCtrl->GetItemText(item);
		if (itemtext.Contains(":"))
		{
			wxString l_sLabel = itemtext.BeforeFirst(':');
			l_sLabel=l_sLabel.Trim();//right trim
			l_sLabel=l_sLabel.Trim(false);//left trim
			if(l_sLabel==text)
			{
				bFound = true;
			}
		 }

		if (itemtext.Contains("=") && itemtext.Contains("("))
		{//uncolored functions
			wxString l_sLabel = itemtext.BeforeFirst('(');
			l_sLabel = l_sLabel.Trim();
			l_sLabel = l_sLabel.Trim(false);
			if (l_sLabel == text)
			{
				bFound = true;
			}
		}

		if (itemtext.Contains("="))
		{
			wxString l_sLabel = itemtext.BeforeFirst('=');
			l_sLabel = l_sLabel.Trim();
			l_sLabel = l_sLabel.Trim(false);
			if (l_sLabel == text)
			{
				bFound = true;
			}
		}//observers
		if(itemtext==text){ bFound = true; }
		//if (!bCaseSensitive) itemtext.MakeLower();
		//bFound = bExactMatch ? (itemtext == findtext) : itemtext.Contains(findtext);
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
void SP_WDG_DeclarationTreectrl::FindUnusedConstantGroups(std::map<wxString, wxString>& l_mConstants2Group, std::set<wxString>&  p_vUnUsedConstantsList,std::set<wxString>& l_setNominatedGroups)
{
	 
	for (auto itSet = p_vUnUsedConstantsList.begin(); itSet != p_vUnUsedConstantsList.end(); ++itSet)
	{
		std::string l_sConst = (*itSet);

		auto itFound = l_mConstants2Group.find(l_sConst);

		if (itFound != l_mConstants2Group.end())
		{
			l_setNominatedGroups.insert(itFound->second);
		}

	}

	for (auto itMap = l_mConstants2Group.begin(); itMap != l_mConstants2Group.end(); ++itMap)
	{
		std::string l_sTofind = itMap->first.ToStdString();
		auto itFound = p_vUnUsedConstantsList.find(l_sTofind);

		if (itFound == p_vUnUsedConstantsList.end())
		{
			std::string l_sTofind = itMap->second.ToStdString();
			auto itSetFoud = l_setNominatedGroups.find(l_sTofind);
			//the group is used by an used constant, so exclude it from the nominate list
			if (itSetFoud != l_setNominatedGroups.end())
			{
				l_setNominatedGroups.erase(itSetFoud);
			}
		}

	}
}

void  SP_WDG_DeclarationTreectrl::MarkTree(sp_node * root, wxTreeCtrl* decTree, const wxTreeItemId& DecTreeRoot, wxColour& p_color)
{
	if (root == NULL)
		return;

	// Standard level order traversal code
	// using queue
	queue<sp_node *> q;  // Create a queue
	q.push(root); // Enqueue root
	while (!q.empty())
	{
		int n = q.size();

		// If this node has children
		while (n > 0)
		{
			// Dequeue an item from queue and print it
			sp_node * p = q.front();
			q.pop();
		//	l_mapResultKey[p->type].insert(p->key);


			wxTreeItemId foundId = findTreeItem(decTree, DecTreeRoot, p->key, true, true);

			if (foundId.IsOk()&& p->key!=wxT("Dot")) {//!=NULL
				wxColour l_nColor1(wxT("RED"));
				decTree->SetItemTextColour(foundId, l_nColor1);
			}
			// Enqueue all children of the dequeued item
			for (int i = 0; i<p->child.size(); i++)
				q.push(p->child[i]);
			n--;
		}
	}
}

void  SP_WDG_DeclarationTreectrl:: ClearAll()
{
	for (auto p : l_pcConstantsDependencies)
	{
		delete p;
	}
	l_pcConstantsDependencies.clear();
	//clear and delete fun
	for (auto p : l_pcColorfunctionsDependencies)
	{
		delete p;
	}
	l_pcColorfunctionsDependencies.clear();
	 //color sets
	for (auto p : l_pcColorSetDependencies)
	{
		delete p;
	}
	 l_pcColorSetDependencies.clear();
	 //variables
	 for (auto p : l_pcVariableDependencies)
	 {
		 delete p;
	 }
	 l_pcVariableDependencies.clear();
	 //observers
	 for (auto p : l_pcObserverDependencies)
	 {
		 delete p;
	 }

	 l_pcObserverDependencies.clear();
}
