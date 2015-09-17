//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/windows/SP_GUI_DevbarContainer.h"
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/widgets/SP_WDG_Toolbar.h"

#include "sp_core/SP_GPR_Canvas.h"

#include "sp_core/SP_Core.h"

#include "sp_defines.h"

#include <wx/cmdproc.h>

#include "snoopy.h"

// under Windows the icons are in the .rc file
// else, we have to include them manually
#ifndef __WXMSW__
    #include "bitmaps/snoopy.xpm"
#endif

SP_GUI_Childframe::SP_GUI_Childframe(SP_MDI_Doc* p_pcDoc,
                                     SP_MDI_View* p_pcView,
                                     wxMenu** p_ppcEditMenu,
                                     const wxSize& p_Size)
: SP_GUI_ChildframeBase
  (p_pcDoc, p_pcView, wxGetApp().GetMainframe(), -1, wxT("Child"),
		   wxDefaultPosition, p_Size, wxDEFAULT_FRAME_STYLE),
    m_pcGraphTreectrl(NULL),
    m_pcCoarseTreectrl(NULL),
	m_pcColorSetTreectrl(NULL),//liu
    m_pcEditMenu(NULL),
    m_pcViewMenu(NULL),
    m_pcElementsMenu(NULL),
    m_pcExtrasMenu(NULL),
	m_pcDoc(p_pcDoc)
{
    Bind(wxEVT_MENU, &SP_GUI_Childframe::OnHierarchyGoUp, this, SP_MENU_ITEM_GO_UP);
    Bind(wxEVT_MENU, &SP_GUI_Childframe::OnHierarchyGoToFirstChild, this, SP_MENU_ITEM_GO_FIRST_CHILD);
    Bind(wxEVT_MENU, &SP_GUI_Childframe::OnHierarchyGoToNextSibling, this, SP_MENU_ITEM_GO_NEXT_SIBLING);
    Bind(wxEVT_MENU, &SP_GUI_Childframe::OnHierarchyGoToPrevSibling, this, SP_MENU_ITEM_GO_PREV_SIBLING);

	Bind(wxEVT_MENU, &SP_GUI_Childframe::OnMenuEvent, this, SP_MENU_ITEM_ELEMENTS_FIRST, SP_MENU_ITEM_ELEMENTS_LAST);

#if wxABI_VERSION < 30000
    Connect(wxEVT_ACTIVATE, wxActivateEventhandler(SP_GUI_Childframe::OnActivate));
#endif

    //Connect(wxEVT_SIZE, wxSizeEventHandler(SP_GUI_Childframe::OnSize));

    //// Associate the menu bar with the frame
    SetMenuBar(CreateMenuBar(p_ppcEditMenu));

    p_pcDoc->GetCommandProcessor()->SetEditMenu(*p_ppcEditMenu);

    Init();

    // accelerators are used for shortcuts to menu commands
    wxAcceleratorEntry l_acAccelerators[1];
    l_acAccelerators[0].Set(wxACCEL_NORMAL, WXK_DELETE, SP_ID_DELETE);
    wxAcceleratorTable l_cTable(1, l_acAccelerators);
    SetAcceleratorTable(l_cTable);

    wxString l_sTitle;
#if wxABI_VERSION > 30000
   	l_sTitle = p_pcDoc->GetUserReadableName();
#else
   	p_pcDoc->GetPrintableName(l_sTitle);
#endif
    SetTitle(l_sTitle);

}

SP_GUI_Childframe::~SP_GUI_Childframe()
{
	//PopEventHandler(true);
}

void
SP_GUI_Childframe::Init()
{
    // TODO!
    SetIcon(wxICON(snoopy));

    wxASSERT_MSG(GetParent(), wxT("No main frame found in child frame creation!"));
    wxASSERT_MSG(GetView(), wxT("No view parameter in child frame creation!"));
    wxASSERT_MSG(GetDocument(), wxT("No document on view parameter in child frame creation!"));

    SP_GUI_Mainframe* l_pcMainframe = wxGetApp().GetMainframe();
    SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());

    l_pcMainframe->Freeze();

    // get the treectrl in the left hand window right
    SetGraphTreectrl(new SP_WDG_GraphTreectrl(
        l_pcMainframe->GetGraphContainer(),
        SP_ID_GRAPHTREE_CTRL_ID,
        wxDefaultPosition,
        wxDefaultSize,
        wxTR_DEFAULT_STYLE));

    // associate the graph to the elements in the treectrl
    l_pcDoc->SetGraphTreectrl(m_pcGraphTreectrl);

    // more or less useless, because the document doesn't have a graph actually
    m_pcGraphTreectrl->Initialise(l_pcDoc->GetGraph());

    l_pcMainframe->Thaw();
    l_pcMainframe->SetActiveChildframe(this);
    l_pcMainframe->GetGraphContainer()->Refresh();
    l_pcMainframe->GetHierarchyContainer()->Refresh();
	l_pcMainframe->GetDeclarationContainer()->Refresh();
}

bool
SP_GUI_Childframe::SetGraphTreectrl(SP_WDG_GraphTreectrl* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    if (!m_pcGraphTreectrl)
    {
        m_pcGraphTreectrl = p_pcVal;
        return m_pcGraphTreectrl->SetOwner(this);
    }
    return FALSE;
}

bool
SP_GUI_Childframe::SetCoarseTreectrl(SP_WDG_CoarseTreectrl* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    if (!m_pcCoarseTreectrl)
    {
    	//awkward location for calling this function, but it works here for coarse frames too
    	wxGetApp().GetCanvasPrefs()->LoadWindowOptions(this);
        m_pcCoarseTreectrl = p_pcVal;
        return m_pcCoarseTreectrl->SetOwner(this);
    }
    return FALSE;
}

bool
SP_GUI_Childframe::SetColorSetTreectrl(SP_WDG_DeclarationTreectrl* p_pcVal)
{
    CHECK_POINTER(p_pcVal, return FALSE);

    if (!m_pcColorSetTreectrl)
    {
        m_pcColorSetTreectrl = p_pcVal;
        return m_pcColorSetTreectrl->SetOwner(this);
    }
    return FALSE;
}

bool
SP_GUI_Childframe::SetElement(SP_Type* p_pcElement)
{
    if (!GetDocument())
        return FALSE;

    SP_ElementClass* t = static_cast<SP_ElementClass*>(p_pcElement);

    SP_WDG_Toolbar* tb = wxGetApp().GetMainframe()->GetToolbar();
    wxMenuItem* item;
    int id = wxNOT_FOUND;
    if (t) {
      id = m_pcElementsMenu->FindItem(t->GetDisplayName());
    }
    if (id == wxNOT_FOUND) {
      item = m_pcElementsMenu->FindItem(SP_MENU_ITEM_ELEMENTS_FIRST);
      if (tb) tb->EnableTool(SP_TOOL_SELECT_ID, FALSE);
    } else {
      item = m_pcElementsMenu->FindItem(id);
      if (tb) tb->EnableTool(SP_TOOL_SELECT_ID, TRUE);
    }
    if (item) item->Check();

    return dynamic_cast<SP_MDI_Doc*>(GetDocument())->SetEditElement(p_pcElement);
}

void
SP_GUI_Childframe::SetTitle( const wxString& title)
{
	SP_GUI_ChildframeBase::SetTitle(title);
}

wxMenuBar*
SP_GUI_Childframe::CreateMenuBar(wxMenu** p_ppcEditMenu)
{
    wxMenu* l_pcFileMenu = new wxMenu();
    m_pcViewMenu = new wxMenu();
    m_pcViewMenu->Append(wxID_ZOOM_100, wxT("Zoom 100%\tCtrl+F10"));
    //m_pcViewMenu->Append(wxID_ZOOM_FIT, wxT("Fit"));
    m_pcViewMenu->Append(wxID_ZOOM_IN, wxT("Zoom In\tCtrl+F11"));
    m_pcViewMenu->Append(wxID_ZOOM_OUT, wxT("Zoom Out\tCtrl+F12"));
    m_pcViewMenu->AppendSeparator();
    m_pcViewMenu->Append(wxID_REFRESH, wxT("Refresh\tCtrl+R"));
    m_pcViewMenu->AppendSeparator();
    m_pcViewMenu->Append(SP_MENU_ITEM_NETINFORMATIONS, wxT("Net Informations..."));
	m_pcViewMenu->AppendSeparator();
    wxMenu* l_pcAboutmenu = new wxMenu();

    wxMenuBar *l_pcMenuBar =
        wxGetApp().GetMainframe()->CreateMenuBar(&l_pcFileMenu,
        &m_pcViewMenu, &l_pcAboutmenu);

    m_pcEditMenu = new wxMenu();
	m_pcEditMenu->Append(wxID_UNDO, _T("&Undo"));
	m_pcEditMenu->Append(wxID_REDO, _T("&Redo"));
	m_pcEditMenu->AppendSeparator();
    m_pcEditMenu->Append(SP_EDIT_SELECT_ALL_ID, wxT("Select a&ll\tCtrl+A"));
    m_pcEditMenu->AppendSeparator();
    m_pcEditMenu->Append(wxID_CUT, wxT("Cu&t\tCtrl+X"));
    m_pcEditMenu->Append(wxID_COPY, wxT("&Copy\tCtrl+C"));
    m_pcEditMenu->Append(wxID_PASTE, wxT("&Paste\tCtrl+V"));
    m_pcEditMenu->Append(SP_EDIT_COPY_IN_NEW_NET, wxT("Copy in new net\tShift+Ctrl+C"));
    // m_pcEditMenu->Append(wxID_DUPLICATE, wxT("&Duplicate\tCtrl+D"));
    m_pcEditMenu->AppendSeparator();
    m_pcEditMenu->Append(SP_ID_DELETE, wxT("&Delete\tDel"));
    m_pcEditMenu->Append(SP_MENU_ITEM_DELETE_ALL, wxT("Delete all"),
			 wxT("Delete all graphics of the selected elements"));
    m_pcEditMenu->AppendSeparator();
    m_pcEditMenu->Append(SP_EDIT_HIDE_ID, wxT("Hide\tCtrl+H"));
    m_pcEditMenu->Append(SP_EDIT_HIDEALL_ID, wxT("Hide all"));
    m_pcEditMenu->Append(SP_EDIT_UNHIDE_ID, wxT("UnHide\tShift+Ctrl+H"));
    m_pcEditMenu->Append(SP_EDIT_UNHIDEALL_ID, wxT("UnHide all"));
    m_pcEditMenu->AppendSeparator();
    m_pcEditMenu->Append(SP_EDIT_PROPERTIES_ID,
			 wxT("&Edit selected elements\tCtrl+E"));
    m_pcEditMenu->Append(SP_EDIT_TRANSFORM_ID, wxT("&Transform Shapes...\tCtrl+T"));
    m_pcEditMenu->AppendSeparator();
	m_pcEditMenu->Append(SP_MENU_ITEM_LAYOUT, wxT("&Layout...\tCtrl+L"));
    m_pcEditMenu->AppendSeparator();
	m_pcEditMenu->Append(SP_MENU_ITEM_SORTNODES, wxT("Sort Nodes..."));
    m_pcEditMenu->AppendSeparator();
	m_pcEditMenu->Append(SP_MENU_ITEM_CHECKDUPLICATENODES, wxT("Check Net..."));
	*p_ppcEditMenu = m_pcEditMenu;


    //wxGetApp().GetDocmanager()->FileHistoryUseMenu(l_pcFileMenu);
    //wxGetApp().GetDocmanager()->FileHistoryAddFilesToMenu(l_pcFileMenu);

    m_pcElementsMenu = new wxMenu();

    wxMenu *hierarchyMenu = new wxMenu();
    hierarchyMenu->Append(SP_MENU_ITEM_COARSE, wxT("&Coarse\tCtrl+G"));
    hierarchyMenu->Append(SP_MENU_ITEM_FLATTEN, wxT("&Flatten\tShift+Ctrl+G"));
    hierarchyMenu->AppendSeparator();

    hierarchyMenu->Append(SP_MENU_ITEM_GO_UP,
		       wxT("Go &Up in Hierarchy\tCtrl+U"));
    hierarchyMenu->Append(SP_MENU_ITEM_GO_FIRST_CHILD,
			  wxT("Go To &First Child in Hierarchy\tCtrl+D"));
    hierarchyMenu->Append(SP_MENU_ITEM_GO_NEXT_SIBLING,
			  wxT("Go To &Next Sibling in Hierarchy\tCtrl+F"));
    hierarchyMenu->Append(SP_MENU_ITEM_GO_PREV_SIBLING,
			  wxT("Go To &Previous Sibling in Hierarchy\tCtrl+B"));

    wxMenu *searchMenu = new wxMenu();
    searchMenu->Append(SP_MENU_ITEM_SEARCHID,
		       wxT("&Search nodes...\tCtrl+Shift+F"));


    l_pcMenuBar->Append(l_pcFileMenu, wxT("&File"));
    l_pcMenuBar->Append(m_pcEditMenu, wxT("&Edit"));
    l_pcMenuBar->Append(m_pcViewMenu, wxT("&View"));
    l_pcMenuBar->Append(m_pcElementsMenu, wxT("E&lements"));
    l_pcMenuBar->Append(hierarchyMenu, wxT("&Hierarchy"));
    l_pcMenuBar->Append(searchMenu, wxT("&Search"));

    m_pcExtrasMenu = new wxMenu();
	l_pcMenuBar->Append(m_pcExtrasMenu, wxT("Extras"));	

    l_pcMenuBar->Append(l_pcAboutmenu, wxT("&Help"));

    return l_pcMenuBar;
}

bool
SP_GUI_Childframe::AddGraphToMenu(SP_DS_Graph* p_pcGraph)
{
    CHECK_POINTER(p_pcGraph, return FALSE);

    bool res = p_pcGraph->AddToViewMenu(m_pcViewMenu);
    res &= p_pcGraph->AddToElementsMenu(m_pcElementsMenu);
    res &= p_pcGraph->AddToEditMenu(m_pcEditMenu);

	//wxString l_sNetClassName = m_pcDoc->GetGraph()->GetNetclass()->GetName();
	//if( l_sNetClassName== SP_DS_COLEPN_CLASS || l_sNetClassName == SP_DS_COLSPN_CLASS )
	{
		//res &= p_pcGraph->AddToFoldingMenu(m_pcFoldingMenu);
	}

	res &= p_pcGraph->AddToExtrasMenu(m_pcExtrasMenu);
    return res;
}

bool
SP_GUI_Childframe::CloseWindow()
{
    if (wxGetApp().GetMainframe())
    {
        if (wxGetApp().GetMainframe()->GetActiveChildframe() == this)
            wxGetApp().GetMainframe()->SetActiveChildframe(NULL);
    }

    if (m_pcGraphTreectrl)
    {
        //wxGetApp().GetMainframe()->Freeze();
        m_pcGraphTreectrl->Remove();
        //wxGetApp().GetMainframe()->Thaw();
    }

    wxGetApp().GetCanvasPrefs()->SaveWindowOptions(this);

    return TRUE;
}

void
SP_GUI_Childframe::Activate()
{
	if (wxGetApp().GetMainframe())
    {
        wxGetApp().GetMainframe()->SetActiveChildframe(this);

#if wxABI_VERSION < 30000
  #ifndef __WXGTK__
        SP_GUI_ChildframeBase::Activate();
  #endif
#endif
        // show the corresponding treectrl
        if (m_pcGraphTreectrl)
            m_pcGraphTreectrl->BringToFront();
        if (m_pcCoarseTreectrl)
            m_pcCoarseTreectrl->BringToFront();
        if (m_pcColorSetTreectrl)
            m_pcColorSetTreectrl->BringToFront();
    }
}

void
SP_GUI_Childframe::OnActivate( wxActivateEvent& p_cEvent )
{
	if( SP_Core::Instance()->GetDocumentReactivatingFlag() )
	{
		SP_Core::Instance()->SetDocumentReactivatingFlag( false );
		return;
	}

#if wxABI_VERSION < 30000
	SP_GUI_ChildframeBase::OnActivate(p_cEvent);
#endif

    if (!GetView() || !wxGetApp().GetMainframe())
        return;

    if (wxGetApp().GetMainframe()->GetActiveChildframe() == this)
        return;

    SP_MDI_Doc* l_pcDoc = dynamic_cast<SP_MDI_Doc*>(GetDocument());

    if (l_pcDoc)
    {
        if (l_pcDoc->GetCommandProcessor() &&
            l_pcDoc->GetCommandProcessor()->GetEditMenu() != m_pcEditMenu)
        {
            l_pcDoc->GetCommandProcessor()->SetEditMenu( m_pcEditMenu );
        }
        // activated a new document, check, whether we are the root?
        while (l_pcDoc->GetType() != SP_DOC_SIMPLE)
            l_pcDoc = l_pcDoc->GetParentDoc();

		SP_Core::Instance()->ManageDocChange(l_pcDoc);
    }

    if(p_cEvent.GetActive())
    {
    	Activate();
    }
}

#if wxABI_VERSION > 30000
//TODO OnActive is not working anymore in wx3.0, we use this as work around
// hook the child view into event handlers chain here
bool SP_GUI_Childframe::TryBefore(wxEvent& event)
{
	if(event.IsKindOf(CLASSINFO(wxActivateEvent)))
	{
		OnActivate(dynamic_cast<wxActivateEvent&>(event));
	}
	return SP_GUI_ChildframeBase::TryBefore(event);
}
#endif

#if wxABI_VERSION < 30000
#ifdef __WXGTK__
// Extend event processing to search the view's event table
bool SP_GUI_Childframe::ProcessEvent(wxEvent& event)
{
    if ( !m_childView || ! m_childView->ProcessEvent(event) )
    {
        // Only hand up to the parent if it's a menu command
        if (!event.IsKindOf(CLASSINFO(wxCommandEvent)) || !GetParent() || !GetParent()->ProcessEvent(event))
            return wxEvtHandler::ProcessEvent(event);
        else
            return true;
    }
    else
        return true;
}
#endif
#endif

bool
SP_GUI_Childframe::OnToolBar(int p_nToolId)
{
    // reset the treectrl of the graph
    if (m_pcGraphTreectrl)
        m_pcGraphTreectrl->DeselectAll();

    wxMenuItem *mi;
    SP_WDG_Toolbar* tb;
    switch (p_nToolId) {
    case SP_TOOL_SELECT_ID:
      tb = wxGetApp().GetMainframe()->GetToolbar();
      if (tb)
      {
    	  tb->EnableTool(SP_TOOL_SELECT_ID, FALSE);
      }
      mi = m_pcElementsMenu->FindItem(SP_MENU_ITEM_ELEMENTS_FIRST);
      if (mi) mi->Check();
      dynamic_cast<SP_MDI_View*>(GetView())->GetCanvas()->SetFocus();
      break;
    case SP_TOOL_EDIT_ID:
        break;
    };
    return TRUE;
}

void
SP_GUI_Childframe::OnHierarchyGoUp(wxCommandEvent& p_cEvent)
{
  m_pcCoarseTreectrl->HierarchyGoUp();
}

void
SP_GUI_Childframe::OnHierarchyGoToFirstChild(wxCommandEvent& p_cEvent)
{
  m_pcCoarseTreectrl->HierarchyGoToFirstChild();
}

void
SP_GUI_Childframe::OnHierarchyGoToNextSibling(wxCommandEvent& p_cEvent)
{
  m_pcCoarseTreectrl->HierarchyGoToNextSibling();
}

void
SP_GUI_Childframe::OnHierarchyGoToPrevSibling(wxCommandEvent& p_cEvent)
{
  m_pcCoarseTreectrl->HierarchyGoToPrevSibling();
}

void
SP_GUI_Childframe::OnMenuEvent(wxCommandEvent& p_event)
{
  int id = p_event.GetId();
  SP_LOGDEBUG(wxString::Format(wxT("SP_GUI_Childframe::OnMenuEvent Event %d"), id));

  DoMenuItemElements(id);

  p_event.Skip();
}

void
SP_GUI_Childframe::DoMenuItemElements(int id)
{
  if (id >= SP_MENU_ITEM_ELEMENTS_FIRST &&
      id < SP_MENU_ITEM_ELEMENTS_LAST)
  {
    wxMenuItem* item = m_pcElementsMenu->FindItem(id);
    if (item)
    {
#if wxABI_VERSION > 30000
        wxString t = item->GetItemLabelText();
#else
        wxString t = item->GetLabelFromText(item->GetLabel());
#endif
      SP_LOGDEBUG(wxString::Format(wxT("Event %d, %s is selected"), id, t.c_str()));
      m_pcGraphTreectrl->SelectItemByLabel(t);
    }
  }
}
