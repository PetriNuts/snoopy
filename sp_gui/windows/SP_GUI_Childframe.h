//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GUI_CHILDFRAME_H__
#define __SP_GUI_CHILDFRAME_H__

#include <wx/wx.h>
#include <wx/docmdi.h>
#include "sp_core/base/SP_Error.h"

#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/windows/SP_GUI_Canvas.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"
#include "sp_gui/widgets/SP_WDG_GraphTreectrl.h"
#include "sp_gui/widgets/SP_WDG_CoarseTreectrl.h"
#include "sp_gui/dialogs/dia_CPN/SP_WDG_DeclarationTreectrl.h" // liu
#include "sp_ds/SP_DS_Graph.h"
#include "sp_core/base/SP_Type.h"

/**	\brief	The SP_GUI_Childframe: class

  Class to represent the different opened views to graphs and such.
  Part of the DocView/MDI and used to displays views on documents
*/

#ifdef __WXMSW__
	typedef wxDocMDIChildFrame SP_GUI_ChildframeBase;
#else
	typedef wxDocChildFrame SP_GUI_ChildframeBase;
#endif

class SP_GUI_Childframe: public SP_GUI_ChildframeBase, public SP_Error
{
private:
    /**	\brief Pointer to the treecontrol

      Pointer to the treecontrol associated with the graph shown
      in this windows canvas and displayed in the sashwindow on the left
    */
    SP_WDG_GraphTreectrl* m_pcGraphTreectrl;
    SP_WDG_CoarseTreectrl* m_pcCoarseTreectrl;
	SP_WDG_DeclarationTreectrl* m_pcColorSetTreectrl;
    wxMenu* m_pcEditMenu;
    wxMenu* m_pcViewMenu;
    wxMenu* m_pcElementsMenu;
	wxMenu* m_pcExtrasMenu;

protected:
    void Init();

    /**	\brief	The CreateMenuBar function

	    \return	void

	    Creates a new menubar with new menus as needed for the actual view.
        E.g. file-menu (save,close,print etc.) and edit-menu (undo,redo)
    */
    wxMenuBar* CreateMenuBar(wxMenu** p_pcEditMenu);

    void OnActivate( wxActivateEvent& p_cEvent );

    void DoMenuItemElements(int id);

#if wxABI_VERSION > 30000
    // hook the child view into event handlers chain here
    virtual bool TryBefore(wxEvent& event);
#endif

public:
    SP_GUI_Childframe(SP_MDI_Doc* p_pcDoc,
        SP_MDI_View* p_pcView,
        wxMenu** p_ppcEditMenu,
        const wxSize& p_Size = wxDefaultSize);

    virtual ~SP_GUI_Childframe();

    /**	\brief	The OnToolBar function

	    \param	p_nToolId	Command ID of the tool, chosen

	    \return	bool returns TRUE on success, otherwise false

	    Called from the mainframe event-handler for the child-toolbar
        just passing the tool ID in, so this child can decide, what to do.
    */
    bool OnToolBar(int p_nToolId);

    //-----------------------------------------------------
    bool SetGraphTreectrl(SP_WDG_GraphTreectrl* p_pcVal);
    bool SetCoarseTreectrl(SP_WDG_CoarseTreectrl* p_pcVal);
	bool SetColorSetTreectrl(SP_WDG_DeclarationTreectrl* p_pcVal);//liu
    bool SetElement(SP_Type* p_pcElement);

    virtual void SetTitle( const wxString& title);

    void Activate();

    bool AddGraphToMenu(SP_DS_Graph* p_pcGraph);

    //! simple Destroy the frame and empty the tree window.
    bool CloseWindow();

    void OnHierarchyGoUp(wxCommandEvent& p_cEvent);
    void OnHierarchyGoToFirstChild(wxCommandEvent& p_cEvent);
    void OnHierarchyGoToNextSibling(wxCommandEvent& p_cEvent);
    void OnHierarchyGoToPrevSibling(wxCommandEvent& p_cEvent);

    void OnMenuEvent(wxCommandEvent& p_event);

	inline wxMenu *GetVisualizationMenu() { return m_pcExtrasMenu; }
#if wxABI_VERSION < 30000
#ifdef __WXGTK__
	//need to be overidden, because of bug in wx itself
	virtual bool ProcessEvent(wxEvent& event);
#endif
#endif
};

#endif // __SP_GUI_CHILDFRAME_H__
