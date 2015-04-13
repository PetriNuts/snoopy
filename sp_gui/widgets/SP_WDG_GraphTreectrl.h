//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 2 classes for the tree items and the tree ctrl
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_GRAPHTREECTRL_H__
#define __SP_WDG_GRAPHTREECTRL_H__

#include "sp_gui/widgets/SP_WDG_Treectrl.h"

/**	\brief	The SP_GraphTreeItemdata class

  This class represents an entry in the tree control.
*/
class SP_GraphTreeItemdata : public SP_TreeItemdata
{
private:
protected:
    SP_Type* m_pcElement;
public:
    SP_GraphTreeItemdata(const wxString& p_sDesc, SP_Type* p_pcElem = NULL)
        :SP_TreeItemdata(p_sDesc), m_pcElement(p_pcElem) { }
    ~SP_GraphTreeItemdata() { }
    
    SP_Type* GetElement() { return m_pcElement; }
};

//---------------------------------------------------------------
//---------------------------------------------------------------

/**	\brief	The SP_WDG_GraphTreectrl: class

  This class is used for displaying the structure of a net or similiar as a tree
*/
class SP_WDG_GraphTreectrl: public SP_WDG_Treectrl
{
private:
    DECLARE_DYNAMIC_CLASS(SP_WDG_GraphTreectrl)
    DECLARE_EVENT_TABLE()
    //! trick to pop up the context dialog
    bool m_bRightWasDown;
protected:
public:
    /**	\brief	The SP_WDG_GraphTreectrl constructor

	    Class needs an empty ctor, because of the IMPLEMENT_DYNAMIC_CLASS macro
    */
    SP_WDG_GraphTreectrl() { }

    /**	\brief	The SP_WDG_GraphTreectrl constructor

	    \param	p_pcParent	a parameter of type SP_GUI_DevbarContainer *
	    \param	p_nId	a parameter of type const wxWindowID
	    \param	p_cPos	a parameter of type const wxPoint&
	    \param	p_cSize	a parameter of type const wxSize&
	    \param	p_nStyle	a parameter of type long

        Constructor takes the parent window, window id,
        position, size and style param.
    */
    SP_WDG_GraphTreectrl(SP_GUI_DevbarContainer *p_pcParent,
        const wxWindowID p_nId = -1,
        const wxPoint& p_cPos = wxDefaultPosition,
        const wxSize& p_cSize = wxDefaultSize,
        long p_nStyle = 0);

    ~SP_WDG_GraphTreectrl();
    
    bool Initialise(SP_DS_Graph* p_pcGraph);
    
    wxTreeItemId AppendFileItem(wxTreeItemId p_cId, const wxString& p_pchLabel, SP_Type* p_pcElem);

    wxTreeItemId AppendFolderItem(wxTreeItemId p_cId, const wxString& p_pchLabel)
    {
        return AppendItem(p_cId, 
            p_pchLabel, 
            TREE_CTRL_ICON_Folder, 
            TREE_CTRL_ICON_FolderSelected, 
            new SP_GraphTreeItemdata(p_pchLabel, NULL));
    }

    void SelectItemByLabel(const wxString& p_label);
    
    void OnSelChanged(wxTreeEvent& p_cEvent);

    // context menu
    void OnRMouseClick(wxMouseEvent& p_cEvent);
};

#endif // __SP_WDG_GRAPHTREECTRL_H__

