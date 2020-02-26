//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_WDG_DeclarationTreectrl_H__
#define __SP_WDG_DeclarationTreectrl_H__

#include "sp_gui/widgets/SP_WDG_Treectrl.h"
#include "sp_ds/extensions/unusedDeclarationsFeature/SP_DS_UnUsedDeclaration.h"

class SP_DeclarationTreeItemdata : public SP_TreeItemdata
{
private:
protected:
	SP_Type* m_pcElement;

public:
	SP_DeclarationTreeItemdata(const wxString& p_sDesc, SP_Type* p_pcElem = NULL)
		:SP_TreeItemdata(p_sDesc), m_pcElement(p_pcElem) { }
	~SP_DeclarationTreeItemdata() { }

	SP_Type* GetElement() { return m_pcElement; }
};

//---------------------------------------------------------------
//---------------------------------------------------------------

/**	\brief	The SP_WDG_DeclarationTreectrl: class

This class is used for displaying the decalration as a tree
*/
class SP_WDG_DeclarationTreectrl : public SP_WDG_Treectrl
{
private:
	DECLARE_DYNAMIC_CLASS(SP_WDG_DeclarationTreectrl)

	DECLARE_EVENT_TABLE()

	SP_DS_Graph* m_pcGraph;

	SP_MapString2String m_ColorsMap;

	SP_DS_UnUsedDeclaration m_unUsedDec = {};
	wxString        m_sSelected;
	set<wxString> m_setSelectedItems;
	set<wxString> m_setSelectedConstants;
	set<wxString> m_setSelectedfunctions;
	set<wxString> m_setSelectedObservers;
	set<wxString> m_setUnSelectedItems;
	set<wxString> m_setUnSelectedContants;
	set<wxString> m_setUnSelectedFunctions;
	set<wxString> m_setUnSelectedObservers;
	std::set<wxString> m_vUnUsedDecList;
	std::set<wxString> m_vUnUsedConstantsList;
	std::set<wxString> m_setUnUsedFunctionsList;
	std::set<wxString> m_setUnUsedVariablesList;
	std::set<wxString> m_setUnUsedColorSetsList;
	std::set<wxString> m_setUnUsedObserverList;
	std::set<wxString> m_setUnUsedSimpleCS;
	std::set<wxString> m_setUnUsedCompoundCS;
	std::set<wxString> m_setUnUsedAliasCS;
	std::set<wxString> m_setFinalUnusedDec;
	std::vector<ConstantDependencyInfo> m_vConstantsInfo;
	std::vector<SimpleCsDependencyInfo> m_vSimpleCsInfo;
	std::vector<CompoundCsDependencyInfo> m_vCompoundCsInfo;
	std::vector<AliasCsDependencyInfo> m_vAliasCsInfo;
	std::vector<FunctionDependencyInfo> m_vFunctionsInfo;
	std::vector<SubSetCsDependencyInfo> m_vSubCsInfo;
	std::vector<VariableDependencyInfo> m_vVariablesInfo;
	std::vector<ObserverDependencyInfo> m_vObserversInfo;
	std::map<wxString, set<wxString>> m_mGroup2SelectedSet;
	std::map<wxString, map<wxString, wxString>>  m_mDeclarations2Dependencies;
	//std::map<wxString, map<wxString,wxString>>  m_mDeclarations2Depend;
	std::map<wxString, wxString> m_vtrimeddecConstants;
	std::map<wxString, wxString> m_vtrimeddecFunctions;
	std::map<wxString, wxString> m_vtrimeddecVariables;
	std::map<wxString, wxString> m_vtrimeddecObservers;
	std::map<wxString, wxString> m_vtrimeddecSimplCS;
	std::map<wxString, wxString> m_vtrimeddecCompoundCS;
	std::map<wxString, wxString> m_vtrimeddecAliasCS;
	std::map<wxString, wxString> m_vtrimeddecAll;
	std::map<wxString, wxString> m_vtrimeddecALLCs;
protected:
	wxTreeItemId m_curTreeItem;
	wxMenuItemList l_pcItem1;
	wxMenuItemList l_pcItem2;
	wxMenu l_cMenuOptions;
	wxMenu l_cMenu;
	bool m_bisIsFirst;
	bool m_bisSecond;
	// on msw dbl click collapses/expands an item, we will check
	// the situation and reject it
#ifdef __WXMSW__
	wxTreeItemId m_ActivatedItem;
#endif  

public:
	/**	\brief	The SP_WDG_CoarseTreectrl constructor

	Class needs an empty ctor, because of the IMPLEMENT_DYNAMIC_CLASS macro
	*/
	SP_WDG_DeclarationTreectrl() :m_pcGraph(NULL) { }

	/**	\brief	The SP_WDG_CoarseTreectrl constructor

	\param	p_pcParent	a parameter of type SP_GUI_DevbarContainer *
	\param	p_nId	a parameter of type const wxWindowID
	\param	p_cPos	a parameter of type const wxPoint&
	\param	p_cSize	a parameter of type const wxSize&
	\param	p_nStyle	a parameter of type long

	Constructor takes the parent window, window id,
	position, size and style param.
	*/
	SP_WDG_DeclarationTreectrl(SP_GUI_DevbarContainer *p_pcParent,
		const wxWindowID p_nId = -1,
		const wxPoint& p_cPos = wxDefaultPosition,
		const wxSize& p_cSize = wxDefaultSize,
		long p_nStyle = 0);

	virtual ~SP_WDG_DeclarationTreectrl();

	bool Initialise(SP_DS_Graph* p_pcGraph);

	bool SetGraph(SP_DS_Graph* p_pcVal) { m_pcGraph = p_pcVal; return TRUE; }

	wxTreeItemId AppendFileItem(wxTreeItemId p_cId, const wxString& p_pchLabel, SP_Type* p_pcElem);

	wxTreeItemId AppendFolderItem(wxTreeItemId p_cId, const wxString& p_pchLabel)
	{
		return AppendItem(p_cId,
			p_pchLabel,
			TREE_CTRL_ICON_Folder,
			TREE_CTRL_ICON_FolderSelected,
			new SP_DeclarationTreeItemdata(p_pchLabel, NULL));
	}

	void UpdateColorSetTree();

	//by sl
	void UpdateOtherTree();


	void OnDoubleClick(wxTreeEvent& p_cEvent);
	void OnToolTip(wxTreeEvent& p_cEvent);
	void  OnRMouseClick(wxTreeEvent& p_cEvent);//Added by G.Assaf

											   //void OnFirsMentItem(wxMouseEvent& p_cEvent);//Added by G.Assaf
	void OnCheckForUnusedItems(wxCommandEvent& p_cEvent);//Added by G.Assaf
	void OnCleanUnusedItems(wxCommandEvent& p_cEvent); //Added by G.Assaf
	void  ReadTreeItems(wxTreeCtrl* pTreeCtrl, const wxTreeItemId& root, SP_VectorString &);//Added By G.Assaf
	wxTreeItemId  findTreeItem(wxTreeCtrl* pTreeCtrl, const wxTreeItemId& root, const wxString& text, bool bCaseSensitive, bool bExactMatch);//Added by G.Assaf
	void  OnSelecting(wxCommandEvent& p_cEvent);//Added by G.Assaf
	void  OnSelectAll(wxCommandEvent& p_cEvent);//Added by G.Assaf
	void  OnUnSelectAll(wxCommandEvent& p_cEvent);//Added by G.Assaf
	void  OnUnSelecting(wxCommandEvent& p_cEvent);//Added by G.Assaf
	std::map<wxString, wxString> ObtainDeclarationGroup(wxString &p_sGroup);
	void UpdateSetODeclarations(std::map<wxString, wxString>);
	set<wxString>  SetSelectItems(std::map<wxString, wxString> &p_mdec, set<wxString> &p_setUnused);
	set<wxString> ObtainItemDependencies(wxString p_sItem);
	set<wxString> ObtainItemDependenciesForUnSelect(wxString p_sItem);
	map<wxString, wxString> ObtainItemDependenciesForSelect(wxString p_sItem, bool &p_bIsFound);
	void CheckAllDeclarations();
	void ResetCheckingResults();
	void DeleteConstants();
	void DeleteFunctions();
	void DeleteObservers();
	void DeleteConstantsForColPN();
	void DeleteFunctionsForColPN();
	void DeleteVariablesForColPN();
	void DeleteSimpleColorSet();
	void DeleteCompoundColorSet();
	map<wxString, int> ComputeSelection(wxTreeItemId p_TreeITem);

#ifdef __WXMSW__
	void OnCollapsingExpanding(wxTreeEvent& p_cEvent);
#endif  

	// selects an item


};

#endif // __SP_WDG_DeclarationTreectrl_H__

