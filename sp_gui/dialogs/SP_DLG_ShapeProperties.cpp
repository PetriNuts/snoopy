//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_gui/dialogs/SP_DLG_ShapeProperties.h"
#include "sp_core/SP_Core.h"

IMPLEMENT_CLASS(SP_DLG_ShapeProperties, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_ShapeProperties, wxDialog)
EVT_BUTTON(wxID_APPLY, SP_DLG_ShapeProperties::OnDlgApply)
EVT_BUTTON(wxID_OK, SP_DLG_ShapeProperties::OnDlgOk)
EVT_BUTTON(wxID_CANCEL, SP_DLG_ShapeProperties::OnDlgCancel)
EVT_RADIOBOX(SP_ID_RADIOBOX_SELECT, SP_DLG_ShapeProperties::OnRadio)
END_EVENT_TABLE()

SP_DLG_ShapeProperties::SP_DLG_ShapeProperties(SP_GUI_Mainframe* p_pcParent,
                                               const wxString& p_sTitle,
                                               bool p_bShowDifferent,
                                               long p_nStyle)
:wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxDefaultSize,
          p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
m_bShowsDifferent(p_bShowDifferent),
m_bShowOverview(false)
{
	Init();
}

void
SP_DLG_ShapeProperties::Init()
{
    /* top level sizer */
    m_pcSizer = new wxBoxSizer(wxVERTICAL);

    /* notebook to show multiple pages on */
    if(m_bShowsDifferent)
    {
    	wxArrayString choices;
    	choices.Add(wxT("Standard        "));
    	choices.Add(wxT("Overview"));

    	wxRadioBox* rb = new wxRadioBox(this, SP_ID_RADIOBOX_SELECT, wxT("Select Dialog Mode"),
    				wxDefaultPosition, wxDefaultSize, choices);
    	m_pcSizer->Add(rb, 0, wxCENTER | wxALL, 5);

    	m_pcNotebook = new SP_WDG_Treebook(this, wxT("Properties"), wxDefaultPosition, wxSize(600,500));
        m_pcNotebook->SetMinSize(wxSize(600,500));
    }
    else
    {
    	m_pcNotebook = new SP_WDG_Notebook(this, wxT("Properties"), wxDefaultPosition, wxSize(400,500));
        m_pcNotebook->SetMinSize(wxSize(400,500));
    }
    m_pcSizer->Add(m_pcNotebook, 1, wxEXPAND | wxALL, 5);

    /* Buttons in the lower right hand corner */
    wxSizer* l_pcButtonSizer = CreateButtonSizer(wxOK|wxCANCEL|wxAPPLY);

    m_pcSizer->Add(l_pcButtonSizer, 0, wxEXPAND);

    SetSizerAndFit(m_pcSizer);
    Layout();
    Centre(wxBOTH);
}

SP_WDG_NotebookPage*
SP_DLG_ShapeProperties::AddPage(const wxString& p_sLabel, unsigned int p_nOrdering)
{
    if (m_pcNotebook)
    {
    	if(m_bShowsDifferent)
    	{
            return static_cast<SP_WDG_Treebook*>(m_pcNotebook)->AddOrGetPage(p_sLabel, p_nOrdering);
    	}
    	else
    	{
            return static_cast<SP_WDG_Notebook*>(m_pcNotebook)->AddOrGetPage(p_sLabel, p_nOrdering);
    	}
    }

    return NULL;
}

bool
SP_DLG_ShapeProperties::AddDialogGraphic(SP_Graphic* p_pcVal)
{
    if (!p_pcVal)
        return FALSE;

    // we store only edges oder elements, no attributes or something else
    if (p_pcVal->GetGraphicType() != SP_GRAPHIC_NODE &&
        p_pcVal->GetGraphicType() != SP_GRAPHIC_EDGE &&
        p_pcVal->GetGraphicType() != SP_GRAPHIC_METADATA)
        return FALSE;

    if (p_pcVal->GetParent() && p_pcVal->GetParent()->GetClassObject() &&
        p_pcVal->GetParent()->GetClassObject()->GetParentGraph())
    {
        wxString l_sKey;
        // TODO!
        l_sKey << p_pcVal->GetParent()->GetClassObject()->GetParentGraph()->GetName() << _T("|")
            << p_pcVal->GetParent()->GetClassName();
        m_mlGraphics[l_sKey].push_back(p_pcVal);
    }
    return SP_Core::Instance()->AddDialogGraphic(p_pcVal);
}

bool
SP_DLG_ShapeProperties::ShowDialogAttributes(SP_MapString2ListAttribute* p_ptmAttributes)
{
		bool l_bReturn = FALSE;
    SP_Core* l_pcCore = SP_Core::Instance();
    SP_WDG_DialogBase* l_pcWidget;
    SP_MapString2ListAttribute::iterator l_IterMap;
    for (l_IterMap = p_ptmAttributes->begin(); l_IterMap != p_ptmAttributes->end(); ++l_IterMap)
    {
        l_pcWidget = l_pcCore->GetDialogWidget(l_IterMap->first);
        if (!l_pcWidget)
            continue;

        // remember the widget to get the changes back in OnDlgOk
        m_tlWidgets.push_back(l_pcWidget);

        // if any of the attribute lists has any element, that needs to be shown
        // l_bReturn gets TRUE and the dialog is shown to the user
        l_bReturn |= l_pcWidget->AddToDialog(&(l_IterMap->second), this, m_bShowOverview);
    }
    map<wxString, SP_ListGraphic >::iterator l_Iter;
    for (l_Iter = m_mlGraphics.begin(); l_Iter != m_mlGraphics.end(); ++l_Iter)
    {
        l_pcWidget = l_pcCore->GetDialogWidget(l_Iter->first);
        if (!l_pcWidget)
            continue;

        m_tlWidgets.push_back(l_pcWidget);
        l_bReturn |= l_pcWidget->AddToDialog(&(l_Iter->second), this);
    }

    //TODO: ugly hack to let wxTreebook appear ok
   	SetSize(GetSize() + wxSize(1,1));
   	SetSize(GetSize() - wxSize(1,1));

    return l_bReturn;
}

//------------------------------------------------------------------------
bool
SP_DLG_ShapeProperties::DoDlgApply()
{
    bool l_bReturn = TRUE;

    list<SP_WDG_DialogBase*> l_lFirstAttributes;
    list<SP_WDG_DialogBase*> l_lMiddleAttributes;
    list<SP_WDG_DialogBase*> l_lLastAttributes;

    list<SP_WDG_DialogBase*>::iterator l_Iter;
    for (l_Iter = m_tlWidgets.begin(); l_Iter != m_tlWidgets.end(); ++l_Iter)
    {
        if ((*l_Iter)->GetEvalState() == -1)
            l_lFirstAttributes.push_back((*l_Iter));
        else
            if ((*l_Iter)->GetEvalState() == 0)
                l_lMiddleAttributes.push_back((*l_Iter));
            else
                l_lLastAttributes.push_back((*l_Iter));
    }

    for (l_Iter = l_lFirstAttributes.begin(); l_Iter != l_lFirstAttributes.end(); ++l_Iter)
    {
        // simply set values
        l_bReturn &= (*l_Iter)->OnDlgOk();
    }
    for (l_Iter = l_lMiddleAttributes.begin(); l_Iter != l_lMiddleAttributes.end(); ++l_Iter)
    {
        // simply set values
        l_bReturn &= (*l_Iter)->OnDlgOk();
    }
    for (l_Iter = l_lLastAttributes.begin(); l_Iter != l_lLastAttributes.end(); ++l_Iter)
    {
			// simply set values
        l_bReturn &= (*l_Iter)->OnDlgOk();
    }
    return l_bReturn;
}

void
SP_DLG_ShapeProperties::OnDlgApply(wxCommandEvent& p_cEvent)
{
    while (GetEventHandler() != this)
        PopEventHandler();

    DoDlgApply();

    RebuildDialog();
}

void
SP_DLG_ShapeProperties::OnDlgOk(wxCommandEvent& p_cEvent)
{
    while (GetEventHandler() != this)
        PopEventHandler();

    bool l_bReturn = DoDlgApply();

    // clean up the widgets and
    // continue with the event handling
    if (l_bReturn)
    {
    	CleanUpWidgets();

        SP_Core::Instance()->ClearDialogGraphicList();

        p_cEvent.Skip();
    }
}

void
SP_DLG_ShapeProperties::OnDlgCancel(wxCommandEvent& p_cEvent)
{
    while (GetEventHandler() != this)
        PopEventHandler();

    // clean up the widgets and
    // continue with the event handling
	CleanUpWidgets();
    
	SP_Core::Instance()->ClearDialogGraphicList();

    p_cEvent.Skip();
}

void
SP_DLG_ShapeProperties::CleanUpWidgets()
{
    list<SP_WDG_DialogBase*>::iterator l_Iter = m_tlWidgets.begin();
    while (l_Iter != m_tlWidgets.end())
    {
        SP_WDG_DialogBase* l_pcWidget = (*l_Iter);
        m_tlWidgets.erase(l_Iter);
		l_pcWidget->CleanUpAttributes();
        wxDELETE(l_pcWidget);
        l_Iter = m_tlWidgets.begin();
    }	
}

void
SP_DLG_ShapeProperties::OnRadio(wxCommandEvent& p_cEvent)
{
    while (GetEventHandler() != this)
        PopEventHandler();

    if(p_cEvent.GetSelection() == 0)
	{
		m_bShowOverview = false;
	}
	else
	{
		m_bShowOverview = true;
	}
	RebuildDialog();
}

void
SP_DLG_ShapeProperties::RebuildDialog()
{
    // clean up the widgets and
	// display new widgets again, because of Logicattribute
	CleanUpWidgets();
	int l_nCurrentPage = m_pcNotebook->GetSelection();
	m_pcNotebook->DeleteAllPages();
	// map of list of attributes
	SP_MapString2ListAttribute l_tmAttributes;
	// get the attributes to show in the dialog
	// implemented in base class 'SP_Graphic'
	SP_ListGraphic* l_lGraphics = SP_Core::Instance()->GetDialogGraphicList();
	SP_ListGraphic::iterator l_Iter;
	for (l_Iter = l_lGraphics->begin(); l_Iter != l_lGraphics->end(); ++l_Iter)
	{
		(*l_Iter)->CollectDialogAttributes(&l_tmAttributes);
	}
	ShowDialogAttributes(&l_tmAttributes);
	m_pcNotebook->SetSelection(l_nCurrentPage);
}
