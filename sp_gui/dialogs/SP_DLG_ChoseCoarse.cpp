//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#include <wx/valgen.h>

#include "sp_gui/dialogs/SP_DLG_ChoseCoarse.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/mdi/SP_MDI_View.h"

IMPLEMENT_CLASS(SP_DLG_ChoseCoarse, wxDialog)

BEGIN_EVENT_TABLE(SP_DLG_ChoseCoarse, wxDialog)
EVT_BUTTON(wxID_OK, SP_DLG_ChoseCoarse::OnDlgOk)
EVT_BUTTON(wxID_CANCEL, SP_DLG_ChoseCoarse::OnDlgCancel)
END_EVENT_TABLE()

SP_DLG_ChoseCoarse::SP_DLG_ChoseCoarse(SP_GUI_Mainframe* p_pcParent,
                                       const wxString& p_sTitle,
                                       long p_nStyle)
:wxDialog(p_pcParent, -1, p_sTitle, wxDefaultPosition, wxDefaultSize, 
          p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
          m_sResultClass(wxT(""))
{
    /* top level sizer */
    m_pcSizer = new wxBoxSizer(wxVERTICAL);

    /* Buttons in the lower right hand corner */
    wxBoxSizer *l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxALL, 5);

    m_pcSizer->Add(l_pcButtonSizer, 0);

    SetSizerAndFit(m_pcSizer);
    
    Centre(wxBOTH);
}

bool
SP_DLG_ChoseCoarse::SetChoices(SP_DS_Graph* p_pcGraph)
{
    if (!p_pcGraph || !p_pcGraph->GetNodeclasses() || !m_pcSizer)
        return FALSE;

    wxArrayString l_casCoarses;
    SP_ListNodeclass::const_iterator l_Iter;
	for (l_Iter = p_pcGraph->GetNodeclasses()->begin(); l_Iter != p_pcGraph->GetNodeclasses()->end(); ++l_Iter)
	{
			if ((*l_Iter)->GetPrototype() && (*l_Iter)->GetPrototype()->GetCoarse())
				l_casCoarses.Add((*l_Iter)->GetName());
	}

    if (l_casCoarses.GetCount() == 0)
        return FALSE;

    wxString l_asChoices[100];
    for (size_t i = 0; i < l_casCoarses.GetCount(); i++)
        l_asChoices[i] = l_casCoarses[i];

    m_pcChoice = new wxRadioBox(
        this,
        -1,
        wxT("Choose target node"),
        wxDefaultPosition,
        wxDefaultSize,
        l_casCoarses.GetCount(),
        l_asChoices,
        1,
        wxRA_SPECIFY_COLS);

    m_pcSizer->Prepend(m_pcChoice, 1, wxEXPAND | wxALL, 5);

    m_pcSizer->Fit(this);
    m_pcSizer->SetSizeHints(this);
	Layout();

    return TRUE;
}

//------------------------------------------------------------------------
void
SP_DLG_ChoseCoarse::OnDlgOk(wxCommandEvent& p_cEvent)
{
    m_sResultClass = m_pcChoice->GetStringSelection();
    EndModal(wxID_OK);
}

void
SP_DLG_ChoseCoarse::OnDlgCancel(wxCommandEvent& p_cEvent)
{
    EndModal(wxID_CANCEL);
}

