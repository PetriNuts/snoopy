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
class SP_WDG_DeclarationTreectrl: public SP_WDG_Treectrl
{
private:
    DECLARE_DYNAMIC_CLASS(SP_WDG_DeclarationTreectrl)
    DECLARE_EVENT_TABLE()
    
    SP_DS_Graph* m_pcGraph;

	SP_MapString2String m_ColorsMap;

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
    SP_WDG_DeclarationTreectrl():m_pcGraph(NULL) { }

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

#ifdef __WXMSW__
    void OnCollapsingExpanding(wxTreeEvent& p_cEvent);
#endif  

    // selects an item


};

#endif // __SP_WDG_DeclarationTreectrl_H__

