//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_GUI_MAINFRAME_H__
#define __SP_GUI_MAINFRAME_H__

#include <wx/wx.h>
#include <wx/docmdi.h>
#include <wx/laywin.h>

#include <wx/html/helpctrl.h>
#include <wx/image.h>

#include <wx/toolbar.h>

#include <wx/filename.h>
#include <wx/stdpaths.h>

#include <wx/aui/aui.h>
#include <wx/artprov.h>

#include "sp_gui/management/SP_GM_Docmanager.h"

//! include not possible
class SP_GUI_DevbarContainer;
class SP_WDG_Toolbar;
class SP_GUI_Childframe;
class SP_GUI_LogWindow;

/**	\brief	The SP_GUI_Mainframe: class

  Master window for the whole application. containing the menubar,
  window caption, toolbar, navigator and all MDI child frames.

  The layout is realised using the wxAUI -Library
*/
#if defined(__WXMSW__) || defined(__WXMAC__)
#define USE_MDI
#else
#undef USE_MDI
#endif

#ifdef USE_MDI
	typedef wxDocMDIParentFrame SP_GUI_MainframeBase;
#else
	typedef wxDocParentFrame SP_GUI_MainframeBase;
#endif

class SP_GUI_Mainframe: public SP_GUI_MainframeBase
{
private:

    /**	\brief	The OnExit function

	    \param	p_cEvent	the event object, generated by wx'

	    Called by the event system of wx' on any close-request for
        the whole application. Only calls Close on this for now.
    */
    void OnExit(wxCommandEvent &p_cEvent);

    /**	\brief	The OnAbout function

	    \param	p_cEvent	the event object, generated by wx'

	    Display some about box. Called by the event system ov wx' if
        the user hits 'About' in the help menu.
    */
    void OnAbout(wxCommandEvent& p_cEvent);

    /**	\brief	The OnCheckForUpdate function

	    \param	p_cEvent	the event object, generated by wx'

	    Checks for new Version on snoopy webpage. Called by the event system ov wx' if
        the user hits 'Check Update' in the help menu.
    */
    void OnCheckForUpdate(wxCommandEvent& p_cEvent);

	/** \brief opens HTML Help

		\param	p_cEvent	the event object, generated by wx'
	*/
	void OnHelp(wxCommandEvent& p_cEvent);

	/** \brief show ds export manager */
    void OnExportManager(wxCommandEvent& p_cEvent);
	/** \brief show ds import manager */
	void OnImportManager(wxCommandEvent& p_cEvent);

	void OnPreferences(wxCommandEvent& p_cEvent);

    void OnToggleExpLog(wxCommandEvent& p_cEvent);


    /**	\brief	The OnToggleBar function

	    \param	p_cEvent	the event object, generated by wx'

	    Event handler for switching the different bars in the frame-layout
        on and off.
    */
    void OnToggleBar(wxCommandEvent& p_cEvent);
    /**	\brief	The OnCloseWindow function

	    \param	p_cEvent	the event object, generated by wx'

	    Called by the wx' event system after a call to this->Close.
        The event can be vetoed by the user or the application.
    */
    void OnCloseWindow( wxCloseEvent &p_cEvent );

    /**	\brief	The CreateToolbar function

	    \return	TRUE in case of success, FALSE otherwise

	    Reads the images and generates and displays the toolbars
        for the app. This function is called only once from within the
        creation process of the mainframe.
    */
    bool CreateToolbar();
    /**	\brief	The CreateDevbar function

	    \return	TRUE in case of success, FALSE otherwise

	    Prepares the laefthand control, used for displaying the tree-ctrl
        for choosing what to draw. This function is called only once from
        within the creation process of the mainframe.
    */
    bool CreateDevbar();

    /*
     * toggle steering mode
     */
    void OnSteeringMode(wxCommandEvent& p_cEvent);


	wxAuiManager m_frameManager;

    // pointer to the active childframe, if any.
    // can be NULL
    SP_GUI_Childframe* m_pcActiveChild;

    // container for the tree control, associated to the childframe
    SP_GUI_DevbarContainer* m_pcGraphContainer;
    // container for the hierarchy browser
    SP_GUI_DevbarContainer* m_pcHierarchyContainer;

	SP_GUI_DevbarContainer* m_pcDeclarationContainer; //liu

    // the toolbar with special tools for the actual view
	SP_WDG_Toolbar* m_pcToolbar;
	SP_WDG_Toolbar* m_pcChildToolbar;

    //! Log Window
    //wxTextCtrl* m_pcLogWindow;
    //! old log target to be reset on delete in DEBUG
    //wxLog* m_pcOldLogtarget;

    // log window for export messages
    SP_GUI_LogWindow* m_pcExpLogWindow;

    // html help controller
    wxHtmlHelpController* m_pHelpController;

protected:
public:
    /**	\brief	The SP_GUI_Mainframe constructor

        \param  p_pcDocmanager  doc manager, passed to wx'
	    \param	p_pcParent	    the parent pointer passed to wx'
                    this is usually NULL to have a top level window only parented
                    to the desktop
	    \param	p_sTitle	    title to display in the titlebar
	    \param	p_cPos          starting position (top, left)
	    \param	p_cSize	        initial window size (width, height)
	    \param	p_nStyle	    style to use for displaying the frame

        The mainframe constructor. The parent pointer is usually NULL, meaning the
        application is parented to the desktop.
    */
    SP_GUI_Mainframe(SP_GM_Docmanager* p_pcDocmanager,
        wxFrame* p_pcParent,
        const wxString &p_sTitle,
        const wxPoint& p_cPos = wxDefaultPosition,
        const wxSize& p_cSize = wxDefaultSize,
        const long p_nStyle = wxDEFAULT_FRAME_STYLE);

    /**	\brief	The SP_GUI_Mainframe destructor

    */
    ~SP_GUI_Mainframe();

    //-----------------------------------------------------
    // Event Handling

    /**	\brief	The OnUpdateUI function

	    \param	p_cEvent	the event object, generated by wx'

	    Called from within wx' main loop with every control-id, this
        function is registered for as update-ui-handler.
        For now, checks, whether an active childframe exists and
        enables/disables the buttons for the toolbars.
    */
    void OnUpdateUI(wxUpdateUIEvent& p_cEvent);

    /**	\brief	The CreateMenuBar function

        \param p_ppcFilemenu the menu used for filemanipulation, if NULL created
        \param p_ppcView    the viewmenu with all common settings, if NULL created
        \param p_ppcAbout   the about or helb menu, if NULL created
	    \return	the menubar

	    Creates the menu bar of the mainframe, stored in a protected
        member variable of the wx-baseclass, and fills all menus of
        childframes created with the common elements, to avoid
        repetitive code. In this case it is called from within
        SP_GUI_Childframe::CreateMenuBar.
    */
    wxMenuBar* CreateMenuBar(wxMenu** p_ppcFilemenu = NULL,
        wxMenu** p_ppcView = NULL, wxMenu** p_ppcAbout = NULL);
    /**	\brief	The OnToolbarEvent function

	    \param	p_cEvent	the event object, generated by wx'

	    Eventhandler for clicking any of the toolbar buttons. Just
        forwards the event to the actual childframe, if any - otherwise
        does nothing
    */
    void OnToolbarEvent(wxCommandEvent& p_cEvent);

    /**	\brief	The Refresh function

	    \param	p_bEraseBackground  Flag
	    \param	p_pcRect    rectangle area for the part of this window being updated

	    This method just calls the base classes implementation. This is subject to change.
    */
    void Refresh(bool p_bEraseBackground = TRUE, const wxRect *p_pcRect = (const wxRect*)NULL);

    /**	\brief	The SetActiveChild function

	    \param	p_pcVal	a parameter of type SP_GUI_Childframe*

	    \return	bool    TRUE in case of success, FALSE otherwise

	    From within SP_GUI_Childframe several methods set the application wide actually active
        child window via this method. The mainframe just holds this information for retrieval
        from other objects via GetActiveChild.
    */
    bool SetActiveChildframe(SP_GUI_Childframe* p_pcVal)
    {
    	m_pcActiveChild = p_pcVal;
    	return TRUE;
    }
    /**	\brief	The ActivateNext function

	    \param	p_bMaximized	flag

	    \return	void

	    Test method, extending the abilities of standard wxDocMDIParentFrame::ActivateNext what
        doesn't know about the state of maximize child windows. This method is subject to change
        in its implementation
    */
    void ActivateNextFrame(bool p_bMaximized);

    /**	\brief	The GetActiveChild function

	    \return	SP_GUI_Childframe*

	    Accessor
    */
    SP_GUI_Childframe* GetActiveChildframe() { return m_pcActiveChild; }
    /**	\brief	The GetGraphContainer function

	    \return	SP_GUI_DevbarContainer*

	    Accessor.
    */
    SP_GUI_DevbarContainer* GetGraphContainer() { return m_pcGraphContainer; }
    /**	\brief	The GetHierarchyContainer function

	    \return	SP_GUI_DevbarContainer*

	    Accessor.
    */
    SP_GUI_DevbarContainer* GetHierarchyContainer() { return m_pcHierarchyContainer; }
    SP_GUI_DevbarContainer* GetDeclarationContainer() { return m_pcDeclarationContainer; }
    SP_WDG_Toolbar* GetToolbar();

    SP_GUI_LogWindow* GetExportLogWindow();

};

#endif // __SP_GUI_MAINFRAME_H__
