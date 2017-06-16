//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_GRAPH_H__
#define __SP_DS_GRAPH_H__

#include "sp_ds/SP_DS_Netclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Metadataclass.h"

//! include not possible
class SP_WDG_GraphTreectrl;
class SP_WDG_CoarseTreectrl;
class SP_WDG_DeclarationTreectrl;//by liu
class SP_DS_Graph;
class SP_DLG_GraphProperties;
class SP_DS_Animation;
class wxMenu;
class SP_MDI_Doc;

class SP_DS_FunctionRegistry;

class  SP_DS_Graph: public SP_Error, public SP_Type, public SP_Name
{
private:
    bool m_bDestruction;

protected:
	
    SP_DS_Netclass* m_pcNetclass;

    SP_ListMetadataclass m_lMetadataclass;
    SP_ListNodeclass m_lNodeclass;
    SP_ListEdgeclass m_lEdgeclass;

    SP_WDG_CoarseTreectrl* m_pcCoarseTreectrl;
    SP_WDG_DeclarationTreectrl* m_pcDeclarationTreectrl;//by liu

    SP_DS_FunctionRegistry* m_pcFunctionRegistry;

    unsigned int m_nNetnumber;

    bool m_bShowInGlobalDialog;
    bool m_bHasAnimators;
    bool m_bIsCopy;
public:
    SP_DS_Graph(SP_DS_Netclass* p_pcNetclass = NULL);
    virtual ~SP_DS_Graph();

    SP_DS_Graph* CloneDefinition();

    SP_DS_Metadataclass* AddMetadataclass(SP_DS_Metadataclass* p_pcClass);
	SP_DS_Metadataclass* RemoveMetadataclass(SP_DS_Metadataclass* p_pcClass);
    SP_DS_Metadataclass* GetMetadataclass(const wxString& p_pchName);
    SP_DS_Metadataclass* GetMetadataclassByDisplayedName(const wxString& p_pchName);
    SP_DS_Metadataclass* RenameMetadataclass(const wxString& p_pchFrom, const wxString& p_pchTo,bool p_bRenameDisplyName=true);
    inline const SP_ListMetadataclass* GetMetadataclasses() { return &m_lMetadataclass; }

    SP_DS_Nodeclass* AddNodeclass(SP_DS_Nodeclass* p_pcClass);
    SP_DS_Nodeclass* RemoveNodeclass(SP_DS_Nodeclass* p_pcClass);
    SP_DS_Nodeclass* GetNodeclass(const wxString& p_pchName);
    SP_DS_Nodeclass* GetNodeclassByDisplayedName(const wxString& p_pchName);
    SP_DS_Nodeclass* RenameNodeclass(const wxString& p_pchFrom, const wxString& p_pchTo,bool p_bRenameDisplyName=true);
    inline const SP_ListNodeclass* GetNodeclasses() { return &m_lNodeclass; }

    SP_DS_Edgeclass* AddEdgeclass(SP_DS_Edgeclass* p_pcClass);
    SP_DS_Edgeclass* GetEdgeclass(const wxString& p_pchName);
    SP_DS_Edgeclass* GetEdgeclassByDisplayedName(const wxString& p_pchName);
    SP_DS_Edgeclass* RenameEdgeclass(const wxString& p_pchFrom, const wxString& p_pchTo,bool p_bRenameDisplyName=true);
    inline const SP_ListEdgeclass* GetEdgeclasses() { return &m_lEdgeclass; }

    bool AddElement(SP_Data* p_pcElement);

    SP_DS_Netclass* GetNetclass();
    SP_DS_Netclass* SetNetclass(SP_DS_Netclass* p_pcVal) { m_pcNetclass = p_pcVal; return m_pcNetclass; }
	

    bool SetGlobalShow(bool p_bVal = TRUE);

    /**	\brief	The AddToTreectrl function

	    \param	p_pcCtrl	a parameter of type SP_WDG_GraphTreectrl*

	    \return	FALSE in case of any error, TRUE otherwise

	    This function is called from the Init function of the childframe
        upon creation. Just to get the tree control filled with the right
        content (name of the net, the node- and edge-classes)
    */
    bool AddToTreectrl(SP_WDG_GraphTreectrl* p_pcCtrl);

	bool AddToDeclarationTreectrl(SP_WDG_DeclarationTreectrl* p_pcCtrl);

    bool AddToViewMenu(wxMenu* p_pcMenu);
    bool AddToElementsMenu(wxMenu* p_pcMenu);
    bool AddToEditMenu(wxMenu* p_pcMenu);
	bool AddToExtrasMenu(wxMenu* p_pcMenu);

	void UpdateIASubMenu(wxMenu *p_pcMenu);
	void UpdateIASubMenuSelection(wxMenu *p_pcVisMenu, wxCommandEvent &p_cEvent);

    bool ShowGlobalOptions(SP_DLG_GraphProperties* p_pcDlg);
    bool OnGlobalOptions();
    bool SqueezeIdAttributes();

    bool HasAttributeType(SP_ATTRIBUTE_TYPE p_eVal);

    inline unsigned int GetNetnumber() { return m_nNetnumber; }
    inline unsigned int GetNewNetnumber() { return ++m_nNetnumber; }
    inline void TestNetnumber(unsigned int p_nVal) { if (p_nVal > m_nNetnumber) m_nNetnumber = p_nVal; }

    bool RemoveElements(unsigned int p_nNetnumber);

    SP_Data* GetCoarseNode(unsigned int p_nNetnumber);
    bool SetNetnumbers(unsigned int p_nNewVal, unsigned int p_nOldVal);
    bool ResetNetnumbers(unsigned int p_nFromNr, unsigned int p_nToNr, SP_DS_Graph* p_pcToGraph);

    bool JoinElements(SP_Data* p_pcRefElem, const wxString& p_pchRefAttribute);

    bool ShowOnCanvas(SP_GUI_Canvas* p_pcCanvas, bool p_bLocalOnly = FALSE);

    bool CheckIntegrity();

    bool GetGraphicsInNet(SP_ListGraphic* p_plGraphics, unsigned int p_nNet, const unsigned int p_eFilter = SP_ELEMENT_ALL);
    bool OnFlatten(unsigned int p_nToNet, unsigned int p_nFromNet, SP_ListGraphic* p_plGraphics = NULL);

    /**	\brief	The AddToCopyMap function

        \param  p_ppcGraph  reference to the graph we are copied to
	    \param	p_nNetnumber	a parameter of type unsigned int
	    \param	p_pmGraphic2Graphic	a parameter of type SP_Graphic*>*
	    \param	p_pmData2Data	a parameter of type SP_Data*>*

	    \return	bool


    */
    bool AddToCopyMap(SP_DS_Graph** p_ppcGraph, unsigned int p_nNetnumber, SP_MapGraphic2Graphic* p_pmGraphic2Graphic, SP_MapData2Data* p_pmData2Data);

    bool Copy(unsigned int p_nNr, SP_ListGraphic* p_plShapes);
    bool Paste(SP_GUI_Canvas* p_pcCanvas);
    bool SetIsCopy(bool p_bVal) { m_bIsCopy = p_bVal; return TRUE; }
    bool GetIsCopy() const { return m_bIsCopy; }

    SP_WDG_CoarseTreectrl* CreateCoarseTree();

	SP_WDG_DeclarationTreectrl* CreateDeclarationTree(); //by liu

    bool ResetCoarseTree();

    void SetHasAnimators(bool p_bVal) { m_bHasAnimators = p_bVal; }
    bool GetHasAnimators() const { return m_bHasAnimators; }
    bool OnToggleAnim(bool p_bVal);
    bool SetAnimation(SP_DS_Animation* p_pcAnim);

    bool ShowDebug();

    bool OnSaveDocument(const wxString& p_sFile);
    bool OnOpenDocument(const wxString& p_sFile);

	SP_MDI_Doc *GetParentDoc();

	//Sets hide to @param and all graphics
	void SetHide(SP_Data* p_pcData, bool p_bHide, bool p_bIncludeGraphics = true);

	//update the whole graph
	bool Update();

	SP_DS_FunctionRegistry* GetFunctionRegistry();
};

#endif // __SP_DS_GRAPH_H__
