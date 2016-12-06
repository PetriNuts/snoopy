//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_utilities.h"


#include "sp_gui/windows/SP_GUI_DevbarContainer.h"
#include "sp_gui/widgets/SP_WDG_Toolbar.h"
#include "sp_gui/dialogs/SP_DLG_GlobalPreferences.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/dialogs/SP_DLG_About.h"
#include "sp_gui/dialogs/SP_DLG_CheckForUpdate.h"
#include "sp_gui/dialogs/dia_SteeringGUI/SP_GUI_ConnectToServer.h"
#include "sp_gui/windows/SP_GUI_LogWindow.h"

#include "export/SP_ExportManager.h"
#include "import/SP_ImportManager.h"

#include "sp_core/SP_Core.h"
#include "snoopy.h"

#include "sp_revision.h"
#include "sp_build.h"
#include"spsim/simulator.h"

// under Windows the icons are in the .rc file
// else, we have to include them manually
#ifndef __WXMSW__
    #include "bitmaps/snoopy.xpm"
    #include "bitmaps/bitmap_select.xpm"
    #include "bitmaps/bitmap_new.xpm"
    #include "bitmaps/bitmap_open.xpm"
    #include "bitmaps/bitmap_save.xpm"
#endif

SP_GUI_Mainframe::SP_GUI_Mainframe(SP_GM_Docmanager* p_pcDocmanager,
                                   wxFrame* p_pcParent,
                                   const wxString &p_sTitle,
                                   const wxPoint& p_cPos,
                                   const wxSize& p_cSize,
                                   const long p_nStyle)
	:SP_GUI_MainframeBase
		  (p_pcDocmanager, p_pcParent, -1, p_sTitle,
		       p_cPos, p_cSize, p_nStyle),
			   m_pcActiveChild(NULL),
			   m_pcGraphContainer(NULL),
			   m_pcHierarchyContainer(NULL),
			   m_pcDeclarationContainer(NULL),
			   m_pcToolbar(NULL),
			   //m_pcLogWindow(NULL),
			   //m_pcOldLogtarget(NULL),
			   m_pcExpLogWindow(NULL)
{
	m_frameManager.SetManagedWindow(this);

    CreateToolbar();

    CreateDevbar();

	// tell the manager to wxT("commit") all the changes just made
    m_frameManager.Update();

    m_pcExpLogWindow = new SP_GUI_LogWindow(this, wxT("Log window"), false);

    // create the menubar
    SetMenuBar(CreateMenuBar());

    CreateStatusBar(1);
    SetStatusText(wxT("Welcome."));

    // TODO!
    SetIcon(wxICON(snoopy));

	m_pHelpController = new wxHtmlHelpController(wxHF_CONTENTS | wxHF_INDEX | wxHF_SEARCH, this);
	wxString l_sAppDir = wxStandardPaths::Get().GetDataDir() + wxFileName::GetPathSeparator() + wxT("help");
	m_pHelpController->Initialize(l_sAppDir);

    Bind(wxEVT_MENU, &SP_GUI_Mainframe::OnExit, this, wxID_EXIT);
    Bind(wxEVT_MENU, &SP_GUI_Mainframe::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &SP_GUI_Mainframe::OnHelp, this, wxID_HELP);

    Bind(wxEVT_MENU, &SP_GUI_Mainframe::OnCheckForUpdate, this, SP_MENU_ITEM_CHECKFORUPDATE);

    Bind(wxEVT_MENU, &SP_GUI_Mainframe::OnToggleBar, this, SP_MENU_ITEM_TOGGLE_DEV_ID, SP_MENU_ITEM_TOGGLE_LOG_ID);

    Bind(wxEVT_UPDATE_UI, &SP_GUI_Mainframe::OnUpdateUI, this, SP_TOOL_EDIT_ID);
    Bind(wxEVT_UPDATE_UI, &SP_GUI_Mainframe::OnUpdateUI, this, SP_MENU_ITEM_EXPORT_MGR);
    Bind(wxEVT_UPDATE_UI, &SP_GUI_Mainframe::OnUpdateUI, this, SP_MENU_ITEM_SEARCHID);
    Bind(wxEVT_UPDATE_UI, &SP_GUI_Mainframe::OnUpdateUI, this, SP_MENU_ITEM_TOGGLEANIM);
    Bind(wxEVT_UPDATE_UI, &SP_GUI_Mainframe::OnUpdateUI, this, SP_MENU_ITEM_HIGHMARKING);
    Bind(wxEVT_UPDATE_UI, &SP_GUI_Mainframe::OnUpdateUI, this, SP_ID_GRAPHTREE_CTRL_ID);
    Bind(wxEVT_UPDATE_UI, &SP_GUI_Mainframe::OnUpdateUI, this, wxID_EXIT);
    // log window toggling
    Bind(wxEVT_MENU, &SP_GUI_Mainframe::OnToggleExpLog, this, SP_MENU_ITEM_TOGGLE_EXP_LOG_ID);
    // export manager
    Bind(wxEVT_MENU, &SP_GUI_Mainframe::OnExportManager, this, SP_MENU_ITEM_EXPORT_MGR);
	Bind(wxEVT_MENU, &SP_GUI_Mainframe::OnImportManager, this, SP_MENU_ITEM_IMPORT);
	Bind(wxEVT_MENU, &SP_GUI_Mainframe::OnPreferences, this, wxID_PREFERENCES);
    Bind(wxEVT_MENU, &SP_GUI_Mainframe::OnToolbarEvent, this, SP_TOOL_SELECT_ID, SP_TOOL_EDIT_ID);

    Bind(wxEVT_MENU, &SP_GUI_Mainframe::OnSteeringMode, this, SP_MENU_ITEM_TOGGLESTEERING);
    Bind(wxEVT_CLOSE_WINDOW, &SP_GUI_Mainframe::OnCloseWindow, this);

}

SP_GUI_Mainframe::~SP_GUI_Mainframe()
{
	//TODO: find out why this doesn't work
	m_pHelpController->Quit();
	delete m_pHelpController;

	m_frameManager.UnInit();
}

wxMenuBar*
SP_GUI_Mainframe::CreateMenuBar(wxMenu** p_ppcFilemenu, wxMenu** p_ppcView, wxMenu** p_ppcAbout)
{
    wxMenuBar *l_pcMenubar = new wxMenuBar();
    wxMenu* l_pcFilemenu;

    if (!p_ppcFilemenu)
        l_pcFilemenu = new wxMenu;
    else
        l_pcFilemenu = *p_ppcFilemenu;

    l_pcFilemenu->Append(wxID_NEW, wxT("&New...\tCtrl+N"),
			 wxT("Create a new document"));
    l_pcFilemenu->Append(wxID_OPEN, wxT("&Open...\tCtrl+O"),
			 wxT("Load an existing document"));
    l_pcFilemenu->Append(wxID_CLOSE, wxT("&Close Window\tCtrl+W"),
			 wxT("Close the current document"));
    l_pcFilemenu->Append(wxID_SAVE, wxT("&Save\tCtrl+S"),
			 wxT("Save the current document"));
    l_pcFilemenu->Append(wxID_SAVEAS, wxT("Save &As...\tF11"),
			 wxT("Save the current document under a new name"));
    l_pcFilemenu->Append(SP_MENU_ITEM_SAVE_SIGN, wxT("Save As and Sign...\tF12"),
		wxT("Save the current document and sign it"));

	l_pcFilemenu->AppendSeparator();
	l_pcFilemenu->Append(wxID_PAGE_SETUP, wxT("Page Setup...\tShift+Ctrl+P"));
    l_pcFilemenu->Append(wxID_PRINT, wxT("&Print...\tCtrl+P"));
    l_pcFilemenu->Append(wxID_PREVIEW, wxT("Print Preview..."));
    l_pcFilemenu->AppendSeparator();


    l_pcFilemenu->Append(SP_MENU_ITEM_EXPORT_MGR,
		       wxT("&Export\tCtrl+Shift+E"));

	l_pcFilemenu->Append(SP_MENU_ITEM_IMPORT,
		       wxT("&Import\tCtrl+Shift+I"));

    l_pcFilemenu->AppendSeparator();

	l_pcFilemenu->Append(wxID_PREFERENCES, wxT("Preferences..."),
			wxT("Modify default properties such as shapes, colors, etc."));

	l_pcFilemenu->AppendSeparator();

	l_pcFilemenu->Append(wxID_EXIT, wxT("E&xit"), wxT("Exit the application"));

    wxMenu *l_pcAboutmenu;
    if (!p_ppcAbout)
	{
    	l_pcAboutmenu = new wxMenu();
	}
    else
    {
    	l_pcAboutmenu = *p_ppcAbout;
    }

	l_pcAboutmenu->Append(wxID_HELP,
			  wxT("&Help\tF1"), wxT("How to use Snoopy"));

	l_pcAboutmenu->Append(wxID_ABOUT,
			  wxT("&About"), wxT("Display some informations"));

	l_pcAboutmenu->Append(SP_MENU_ITEM_CHECKFORUPDATE,
			  wxT("&Check Update"), wxT("Check for new Version"));

    wxMenu *l_pcViewmenu;
    if (!p_ppcView)
        l_pcViewmenu = new wxMenu();
    else
        l_pcViewmenu = *p_ppcView;

    l_pcViewmenu->Append(SP_MENU_ITEM_TOGGLE_DEV_ID,
			 wxT("Toggle &Graphelements\tAlt+0"),
			 wxT("Toggle display of graph elements"));
    l_pcViewmenu->Append(SP_MENU_ITEM_TOGGLE_FMEN_ID,
			 wxT("Toggle &Filebar\tAlt+1"),
			 wxT("Toggle display of the file menu button bar"));


    l_pcViewmenu->Append(SP_MENU_ITEM_TOGGLE_HIERARCHY_ID,
			 wxT("Toggle &Hierarchy browser\tAlt+2"),
			 wxT("Toggle display of the hierarchy browser"));

    l_pcViewmenu->Append(SP_MENU_ITEM_TOGGLE_EXP_LOG_ID,
			 wxT("Toggle &Log window\tAlt+3"),
			 wxT("Toggle display of the log window"));

#if 0
//#ifdef __WXDEBUG__
    l_pcViewmenu->Append(SP_MENU_ITEM_TOGGLE_LOG_ID,
			 wxT("Toggle &Logwindow\tAlt+4"),
			 wxT("Toggle display of the log window"));
#endif // __WXDEBUG__

        //Steering mode
        l_pcViewmenu->AppendSeparator();

        l_pcViewmenu->Append(SP_MENU_ITEM_TOGGLESTEERING,
        			         wxT("Start Steering-Mode\tF7"),
        			         wxT("Toggle Steering mode"),
        			         wxITEM_CHECK
        			        );

    if (!p_ppcFilemenu) {
        l_pcMenubar->Append(l_pcFilemenu, wxT("&File"));
    }
    if ( !p_ppcView) {
        l_pcMenubar->Append(l_pcViewmenu, wxT("&View"));
    }
    if (!p_ppcAbout) {
        l_pcMenubar->Append(l_pcAboutmenu, wxT("&Help"));
    }

    wxGetApp().GetDocmanager()->FileHistoryUseMenu(l_pcFilemenu);
    wxGetApp().GetDocmanager()->FileHistoryAddFilesToMenu(l_pcFilemenu);

    return l_pcMenubar;
}

bool
SP_GUI_Mainframe::CreateDevbar()
{
	m_pcGraphContainer = new SP_GUI_DevbarContainer(this, -1);
	m_frameManager.AddPane(m_pcGraphContainer,	wxLEFT,   wxT("Graphelements"));
    m_pcHierarchyContainer = new SP_GUI_DevbarContainer(this, -1);
	m_frameManager.AddPane(m_pcHierarchyContainer,	wxLEFT,   _("Hierarchy"));
    m_pcDeclarationContainer = new SP_GUI_DevbarContainer(this, -1);
	m_frameManager.AddPane(m_pcDeclarationContainer,	wxLEFT,   _("Declarations"));
	m_pcDeclarationContainer->Show(false);
						 
#if defined(__WXMAC__) || defined(__WXGTK__)
	m_frameManager.GetPane(m_pcGraphContainer).MinSize(250, -1);
	m_frameManager.GetPane(m_pcHierarchyContainer).MinSize(250, -1);
	m_frameManager.GetPane(m_pcHierarchyContainer).Row(1);
	m_frameManager.GetPane(m_pcDeclarationContainer).MinSize(250, -1);
	m_frameManager.GetPane(m_pcDeclarationContainer).Row(1);
#endif
	return true;
}

bool
SP_GUI_Mainframe::CreateToolbar()
{
	m_pcToolbar	 = new SP_WDG_Toolbar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
   // m_pcToolbar->SetToolBitmapSize(wxSize(24,24));

	wxBitmap l_apcBitmaps[4];

#if defined(__WIN32__)
    l_apcBitmaps[0] = wxBitmap( wxT("bitmap_select"), wxBITMAP_TYPE_RESOURCE);
    l_apcBitmaps[1] = wxBitmap( wxT("bitmap_new"), wxBITMAP_TYPE_RESOURCE);
    l_apcBitmaps[2] = wxBitmap( wxT("bitmap_open"), wxBITMAP_TYPE_RESOURCE);
    l_apcBitmaps[3] = wxBitmap( wxT("bitmap_save"), wxBITMAP_TYPE_RESOURCE);
#elif defined(__WXGTK__) || defined(__WXMAC__) // !__WIN32__
    l_apcBitmaps[0] = wxBitmap(bitmap_select_xpm);
    l_apcBitmaps[1] = wxBitmap(bitmap_new_xpm);
    l_apcBitmaps[2] = wxBitmap(bitmap_open_xpm);
    l_apcBitmaps[3] = wxBitmap(bitmap_save_xpm);
#else
#error wxT("No icons for plattform.")
#endif


	m_pcToolbar->AddTool(	wxID_NEW,
							wxT("Open new document"),
							l_apcBitmaps[1],
							wxT("Open a new document"));

	m_pcToolbar->AddTool(	wxID_OPEN,
							wxT("Load document"),
							l_apcBitmaps[2],
							wxT("Load a document"));

	m_pcToolbar->AddTool(	wxID_SAVE,
							wxT("Save document"),
							l_apcBitmaps[3],
							wxT("Save a document"));

	m_pcToolbar->Realize();


	 m_frameManager.AddPane(m_pcToolbar, wxAuiPaneInfo().
                  Name(wxT("m_pcToolbar")).Caption(wxT("Toolbar")).
                  ToolbarPane().Top().
                  LeftDockable(false).RightDockable(false));


	// the children toolbar
    m_pcChildToolbar = new SP_WDG_Toolbar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_NODIVIDER);
	//m_pcChildToolbar->SetToolBitmapSize(wxSize(24,24));

	m_pcChildToolbar->AddTool( SP_TOOL_SELECT_ID,	wxT("Select"),l_apcBitmaps[0], wxT("Select an Element"));

	m_pcChildToolbar->EnableTool(SP_TOOL_SELECT_ID, FALSE);

	m_pcChildToolbar->Realize();


	m_frameManager.AddPane(m_pcChildToolbar, wxAuiPaneInfo().
                  Name(wxT("m_pcChildToolbar")).Caption(wxT("ChildToolbar")).
                  ToolbarPane().Top().
                  LeftDockable(false).RightDockable(false));
#ifdef USE_MDI
    m_frameManager.AddPane(this->GetClientWindow(), wxCENTER, _(""));
#endif

    return TRUE;
}


void
SP_GUI_Mainframe::Refresh(bool p_bEraseBackground, const wxRect* p_pcRect)
{
    SP_GUI_MainframeBase::Refresh(p_bEraseBackground, p_pcRect);
}

void
SP_GUI_Mainframe::ActivateNextFrame(bool p_bMaximized)
{
#ifdef USE_MDI
	SP_GUI_MainframeBase::ActivateNext();
#endif
    SP_GUI_Childframe* l_pcChild = GetActiveChildframe();

    if (l_pcChild && p_bMaximized)
        l_pcChild->Maximize(TRUE);
}

void
SP_GUI_Mainframe::OnAbout(wxCommandEvent& p_cEvent)
{

	SP_DLG_About(this, 10000);

}

void
SP_GUI_Mainframe::OnCheckForUpdate(wxCommandEvent& p_cEvent)
{
	SP_DLG_CheckForUpdate l_cDlg(this);
	l_cDlg.CheckForUpdate();
}

void
SP_GUI_Mainframe::OnHelp(wxCommandEvent& p_cEvent)
{
	m_pHelpController->DisplayContents();
}

void
SP_GUI_Mainframe::OnToggleBar(wxCommandEvent& p_cEvent)
{
    switch (p_cEvent.GetId())
    {
    case SP_MENU_ITEM_TOGGLE_DEV_ID:
		if (  m_frameManager.GetPane(m_pcGraphContainer).IsShown()){
			m_frameManager.GetPane(m_pcGraphContainer).Show(false);
		} else {
			m_frameManager.GetPane(m_pcGraphContainer).Show(true);
		}
		  m_frameManager.Update();

        break;
    case SP_MENU_ITEM_TOGGLE_FMEN_ID:
		 if (  m_frameManager.GetPane(m_pcToolbar).IsShown()){
			m_frameManager.GetPane(m_pcToolbar).Show(false);
			m_frameManager.GetPane(m_pcChildToolbar).Show(false);

		} else {
			m_frameManager.GetPane(m_pcToolbar).Show(true);
			m_frameManager.GetPane(m_pcChildToolbar).Show(true);
		}
		  m_frameManager.Update();
		break;
    case SP_MENU_ITEM_TOGGLE_HIERARCHY_ID:
 		  	if (  m_frameManager.GetPane(m_pcHierarchyContainer).IsShown()){
			m_frameManager.GetPane(m_pcHierarchyContainer).Show(false);
		} else {
			m_frameManager.GetPane(m_pcHierarchyContainer).Show(true);
		}
		  m_frameManager.Update();
        break;

    }
}

void
SP_GUI_Mainframe::OnUpdateUI(wxUpdateUIEvent& p_cEvent)
{
    switch (p_cEvent.GetId())
    {
    case SP_TOOL_MOVE_ID:
    case SP_TOOL_EDIT_ID:
    case SP_MENU_ITEM_EXPORT_MGR:
    //by sl
    case SP_MENU_ITEM_HIGHMARKING:
    case SP_MENU_ITEM_SEARCHID:
      p_cEvent.Enable(m_pcActiveChild != NULL);
      break;
    case SP_MENU_ITEM_TOGGLEANIM:
        p_cEvent.Check(SP_Core::Instance()->GetAnimMode());
        p_cEvent.SetText((SP_Core::Instance()->GetAnimMode())?wxT("End Anim-Mode\tF5"):wxT("Start Anim-Mode\tF5"));
        p_cEvent.Enable(!SP_Core::Instance()->GetAnimRunning());
        break;
    case wxID_EXIT:
    case SP_ID_GRAPHTREE_CTRL_ID:
        p_cEvent.Enable(!(SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode()));
        break;
    case SP_ID_COARSETREE_CTRL_ID:
        p_cEvent.Enable(!SP_Core::Instance()->GetAnimRunning());
        break;
    };

	if (m_pcExpLogWindow->NewMessagesAdded()) 
	{
		m_pcExpLogWindow->Show();
		m_pcExpLogWindow->ResetNewMessages();
	}
}

void
SP_GUI_Mainframe::OnExit( wxCommandEvent &p_cEvent )
{
    Close( TRUE );
}

void
SP_GUI_Mainframe::OnCloseWindow( wxCloseEvent &p_cEvent )
{
    if (SP_Core::Instance()->GetAnimMode() || SP_Core::Instance()->GetSimulationMode())
    {
        p_cEvent.Veto();
        return;
    }

    wxGetApp().SetMainPos(GetPosition().x, GetPosition().y);
    wxGetApp().SetMainDim(GetSize().GetWidth(), GetSize().GetHeight());
    wxGetApp().SetMainMax(IsMaximized());

    // Clear and CloseDocuments are overloaded in my wxDocManager derived class
    // but I have to cast the wx' member here, to ever reach my own implementations.
    if (dynamic_cast<SP_GM_Docmanager*>(m_docManager)->Clear(!p_cEvent.CanVeto()))
    {
        this->Destroy();
    }
    else
        p_cEvent.Veto();

}

void
SP_GUI_Mainframe::OnToolbarEvent(wxCommandEvent& p_cEvent)
{
  if (!m_pcActiveChild) return;

  // pass the id to the child window,
  // returns bool
  m_pcActiveChild->OnToolBar(p_cEvent.GetId());
}


void
SP_GUI_Mainframe::OnExportManager(wxCommandEvent& p_cEvent)
{
  if (! m_docManager->GetCurrentDocument()) return;

  SP_MDI_Doc* doc =
    dynamic_cast<SP_MDI_Doc*>(m_docManager->GetCurrentDocument());
  SP_ExportManager* em = wxGetApp().GetExportManager();
  em->DoExport(doc);
}

void
SP_GUI_Mainframe::OnImportManager(wxCommandEvent& p_cEvent)
{
  SP_ImportManager* import = wxGetApp().GetImportManager();
  import->DoImport();
}

void
SP_GUI_Mainframe::OnPreferences(wxCommandEvent& p_cEvent) {
	SP_DLG_GlobalPreferences l_cDialog(this);
	l_cDialog.ShowModal();
}

void
SP_GUI_Mainframe::OnToggleExpLog(wxCommandEvent& p_cEvent) {
  m_pcExpLogWindow->Show(! m_pcExpLogWindow->GetFrame()->IsShown());
}

SP_WDG_Toolbar*
SP_GUI_Mainframe::GetToolbar()
{
  return m_pcChildToolbar;
}


SP_GUI_LogWindow*
SP_GUI_Mainframe::GetExportLogWindow()
{
  return m_pcExpLogWindow;
}
void SP_GUI_Mainframe::OnSteeringMode(wxCommandEvent& p_cEvent)
{
	SP_GUI_ConnectToServer* l_pcDlg=NULL;

	                    l_pcDlg=new SP_GUI_ConnectToServer(this);

	                    if(l_pcDlg!=NULL)
	                    {
	                       l_pcDlg->Show();
	                    }
}

