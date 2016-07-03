//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dscheibl $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/08/29 11:20:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#include <wx/valgen.h>

#include "sp_gui/dialogs/SP_DLG_SimulationParamterSelection.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_ds/extensions/SP_DS_ContinuousPedAnimation.h"

IMPLEMENT_CLASS(SP_DLG_SimulationParamterSelection, wxDialog)

enum
{
  SP_ID_PARAMSELECTIONADDCOL = SP_ID_LAST_ID + 1,
};

BEGIN_EVENT_TABLE(SP_DLG_SimulationParamterSelection, wxDialog)
EVT_BUTTON(SP_ID_PARAMSELECTIONADDCOL, SP_DLG_SimulationParamterSelection::OnDlgAddCol)
EVT_BUTTON(wxID_OK, SP_DLG_SimulationParamterSelection::OnDlgOk)
EVT_BUTTON(wxID_CANCEL, SP_DLG_SimulationParamterSelection::OnDlgCancel)
END_EVENT_TABLE()

SP_DLG_SimulationParamterSelection::SP_DLG_SimulationParamterSelection(SP_DS_Animation* p_pcAnim,
                                                                       wxWindow* p_pcParent,
                                                                       const wxString& p_sTitle,
                                                                       long p_nStyle)
: wxDialog(p_pcParent, 
           -1, 
           p_sTitle, 
           wxDefaultPosition, 
           wxDefaultSize, 
           p_nStyle | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX),
  m_pcError(NULL),
  m_pcAnimation(NULL)
{
  m_pcAnimation = dynamic_cast<SP_DS_ContinuousPedAnimation*>(p_pcAnim);

  /* top level sizer */
  m_pcSizer = new wxBoxSizer(wxVERTICAL);
    
  // animation properties come from the animation, of course


  /* Buttons in the lower right hand corner */
  wxBoxSizer* l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );
  m_pcError = new wxStaticText(this, -1, wxT("                                                  "));
  m_pcError->SetForegroundColour(*wxRED);
  l_pcButtonSizer->Add(m_pcError, 0, wxLEFT | wxRIGHT | wxTOP, 5);
  l_pcButtonSizer->Add(new wxButton(this, SP_ID_PARAMSELECTIONADDCOL, wxT("Add Column")), 0, wxALL, 5);
  l_pcButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxALL, 5);
  m_pcSizer->Add(l_pcButtonSizer, 0);

  SetSizerAndFit(m_pcSizer);
}

//------------------------------------------------------------------------
void
SP_DLG_SimulationParamterSelection::OnDlgOk(wxCommandEvent& p_cEvent)
{
  if ( Validate() && TransferDataFromWindow() )
  {


    if ( IsModal() )
    {
      EndModal(wxID_OK);
    } else
    {
      SetReturnCode(wxID_OK);
      this->Show(FALSE);
    }
  }
}

void
SP_DLG_SimulationParamterSelection::OnDlgCancel(wxCommandEvent& p_cEvent)
{
	if (IsModal())
	{
		EndModal(wxID_CANCEL);
	}
	else
	{
		SetReturnCode(wxID_CANCEL);
		this->Show(FALSE);
	}
}

void
SP_DLG_SimulationParamterSelection::OnDlgAddCol(wxCommandEvent& p_cEvent)
{

}

