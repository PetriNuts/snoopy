//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 2 classes for the tree items and the tree ctrl
//////////////////////////////////////////////////////////////////////
#ifndef __SP_WDG_TREECTRL_H__
#define __SP_WDG_TREECTRL_H__

#include <wx/treectrl.h>
#include <wx/imaglist.h>

#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/base/SP_Type.h"

//! include not possible
class SP_GUI_DevbarContainer;
class SP_WDG_Treectrl;
class SP_GUI_Childframe;

/**	\brief	The SP_TreeItemdata class

  This class represents an entry in the tree control.
*/
class SP_TreeItemdata : public wxTreeItemData
{
private:
    wxString m_sDesc;
protected:
public:
    SP_TreeItemdata(const wxString& p_sDesc)
        :m_sDesc(p_sDesc) { }
    virtual ~SP_TreeItemdata() { }

    virtual void OnDoubleClick(SP_WDG_Treectrl* p_pcCtrl) { }
    const wxString GetDesc() const { return m_sDesc; }
};

//---------------------------------------------------------------
//---------------------------------------------------------------

/**	\brief	The SP_WDG_Treectrl: class

  This class is used for displaying the structure of a net or similiar as a tree
*/
class SP_WDG_Treectrl: public wxTreeCtrl
{
private:
    DECLARE_DYNAMIC_CLASS(SP_WDG_Treectrl)
    DECLARE_EVENT_TABLE()

    wxImageList* m_pcImageList;
    // the frame, this treectrl belongs to
    SP_GUI_Childframe* m_pcOwner;
    // the parentframe, derived from wxPanel and just
    // acting as container for the functions (add/remove, etc.)
    SP_GUI_DevbarContainer* m_pcParent;
protected:
    wxTreeItemId FindItemRec(const wxTreeItemId& p_Id,
			     const wxString& p_label);

public:
    enum
    {
        TREE_CTRL_ICON_File,
        TREE_CTRL_ICON_FileSelected,
        TREE_CTRL_ICON_Folder,
        TREE_CTRL_ICON_FolderSelected,
        TREE_CTRL_ICON_FolderOpened,
        TREE_CTRL_ICON_Free
    };
    /**	\brief	The SP_WDG_Treectrl constructor

	    Class needs an empty ctor, because of the IMPLEMENT_DYNAMIC_CLASS macro
    */
    SP_WDG_Treectrl() { }

    /**	\brief	The SP_WDG_Treectrl constructor

	    \param	p_pcParent	a parameter of type SP_GUI_DevbarContainer *
	    \param	p_nId	a parameter of type const wxWindowID
	    \param	p_cPos	a parameter of type const wxPoint&
	    \param	p_cSize	a parameter of type const wxSize&
	    \param	p_nStyle	a parameter of type long

        Constructor takes the parent window, window id,
        position, size and style param.
    */
    SP_WDG_Treectrl(SP_GUI_DevbarContainer *p_pcParent,
        const wxWindowID p_nId = -1,
        const wxPoint& p_cPos = wxDefaultPosition,
        const wxSize& p_cSize = wxDefaultSize,
        long p_nStyle = 0);

    virtual ~SP_WDG_Treectrl();

    /**	\brief	The CreateImageList function

	    \param	p_nSize	(int) size of the images

        This function reads in the icons and fills the member variable,
        used during creation of the treectrl.
    */
    void CreateImageList(int p_nSize = 16);
    bool SetOwner(SP_GUI_Childframe* p_pcVal);
    bool BringToFront();
    bool Remove();

    bool DeselectAll();
    bool SetFolderIcons(wxTreeItemId p_cId)
    {
        if (! p_cId.IsOk())
            return FALSE;

        SetItemImage(p_cId,
            TREE_CTRL_ICON_FolderOpened,
            wxTreeItemIcon_Expanded);
        return TRUE;
    }

    /* EVENTS */
    void OnBeginDrag(wxTreeEvent& p_cEvent);
    void OnBeginRDrag(wxTreeEvent& p_cEvent);
    void OnEndDrag(wxTreeEvent& p_cEvent);
    void OnBeginLabelEdit(wxTreeEvent& p_cEvent);
    void OnEndLabelEdit(wxTreeEvent& p_cEvent);
    void OnDeleteItem(wxTreeEvent& p_cEvent);
    void OnGetInfo(wxTreeEvent& p_cEvent);
    void OnSetInfo(wxTreeEvent& p_cEvent);
    void OnSelChanging(wxTreeEvent& p_cEvent);
    void OnTreeKeyDown(wxTreeEvent& p_cEvent);
    void OnDoubleClick(wxTreeEvent& p_cEvent);
    void OnRMouseClick(wxMouseEvent& p_cEvent);

    void DoToggleIcon(const wxTreeItemId& p_cId);

    wxTreeItemData* GetItemData(const wxTreeItemId& p_cItem) const;

	void DeleteChildren(const wxTreeItemId& p_cItem);
};

#endif // __SP_WDG_TREECTRL_H__

