//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 2 classes for the tree items and the tree ctrl
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_COARSETREECTRL_H__
#define __SP_WDG_COARSETREECTRL_H__

#include "sp_gui/widgets/SP_WDG_Treectrl.h"

class SP_DS_Coarse;
class SP_MDI_CoarseDoc;

/**	\brief	The SP_CoarseTreeItemdata class

  This class represents an entry in the tree control.
*/
class SP_CoarseTreeItemdata : public SP_TreeItemdata
{
private:
protected:
    //! the coarse data
    SP_DS_Coarse* m_pcCoarse;
    //! the net number, this entry stands for
    unsigned int m_nNetnumber;
public:
    SP_CoarseTreeItemdata(const wxString& p_sDesc, SP_DS_Coarse* p_pcCoarse = NULL);
    ~SP_CoarseTreeItemdata() { }

    SP_DS_Coarse* GetCoarse() const { return m_pcCoarse; }
    unsigned int GetNetnumber() const { return m_nNetnumber; }
};

//---------------------------------------------------------------
//---------------------------------------------------------------

/**	\brief	The SP_WDG_CoarseTreectrl: class

  This class is used for displaying the structure of a net or similiar as a tree
*/
class SP_WDG_CoarseTreectrl: public SP_WDG_Treectrl
{
private:
    DECLARE_DYNAMIC_CLASS(SP_WDG_CoarseTreectrl)
    DECLARE_EVENT_TABLE()

    wxTreeItemId GetNewParentItem(wxTreeItemId p_nId, unsigned int p_nNr, wxTreeItemIdValue p_nCookie);
    SP_DS_Graph* m_pcGraph;

    wxTreeItemId FindDocRec(const wxTreeItemId& p_Id,
			     const SP_MDI_CoarseDoc* p_pcDoc);

    // p_show == FALSE means we have to update our tree only, the
    // document was already shown somehow
    void ShowCoarseDoc(const wxTreeItemId& p_Id, bool p_show=TRUE);
    
protected:
    wxTreeItemId m_curTreeItem;

    // on msw dbl click collapses/expands an item, we will check
    // the situation and reject it
#ifdef __WXMSW__
    wxTreeItemId m_ActivatedItem;
#endif  

public:
    /**	\brief	The SP_WDG_CoarseTreectrl constructor

	    Class needs an empty ctor, because of the IMPLEMENT_DYNAMIC_CLASS macro
    */
    SP_WDG_CoarseTreectrl():m_pcGraph(NULL) { }

    /**	\brief	The SP_WDG_CoarseTreectrl constructor

	    \param	p_pcParent	a parameter of type SP_GUI_DevbarContainer *
	    \param	p_nId	a parameter of type const wxWindowID
	    \param	p_cPos	a parameter of type const wxPoint&
	    \param	p_cSize	a parameter of type const wxSize&
	    \param	p_nStyle	a parameter of type long

        Constructor takes the parent window, window id,
        position, size and style param.
    */
    SP_WDG_CoarseTreectrl(SP_GUI_DevbarContainer *p_pcParent,
        const wxWindowID p_nId = -1,
        const wxPoint& p_cPos = wxDefaultPosition,
        const wxSize& p_cSize = wxDefaultSize,
        long p_nStyle = 0);

    virtual ~SP_WDG_CoarseTreectrl();
    
    wxTreeItemId AppendFileItem(unsigned int p_nNetnumber, SP_DS_Coarse* p_pcCoarse);

    void UpdateEntry(wxTreeItemId& p_nId, SP_DS_Coarse* p_pcCoarse);
    bool InvalidateAllChildren(wxTreeItemId& p_nId);
    bool SetGraph(SP_DS_Graph* p_pcVal) { m_pcGraph = p_pcVal; return TRUE; }


    void OnDoubleClick(wxTreeEvent& p_cEvent);

#ifdef __WXMSW__
    void OnCollapsingExpanding(wxTreeEvent& p_cEvent);
#endif  

    // selects an item
    void SetCurrentDoc(const SP_MDI_CoarseDoc* p_pcDoc, bool p_show=TRUE);

    // hierarchy navigation
    void HierarchyGoUp();
    void HierarchyGoToFirstChild();
    void HierarchyGoToNextSibling();
    void HierarchyGoToPrevSibling();

};

#endif // __SP_WDG_COARSETREECTRL_H__

