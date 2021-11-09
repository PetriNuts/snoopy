//////////////////////////////////////////////////////////////////////
// $Source: SP_DLG_GlobalPreferences.cpp
// $Author: MD $
// $Version: $
// $Revision: $
// $Date: 2005/09/19 $
// Short Description:  The dialog for the global preferences
//////////////////////////////////////////////////////////////////////

#include "snoopy.h"
#include "sp_core/SP_Core.h"
#include "sp_core/SP_GPR_Canvas.h"
#include "sp_core/SP_GPR_Animation.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_core/SP_GPR_Fonts.h"

#include "sp_gui/dialogs/SP_DLG_GlobalPreferences.h"
#include "sp_gui/windows/SP_GUI_Mainframe.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/mdi/SP_MDI_View.h"

#include <wx/fileconf.h>

#include "sp_utilities.h"


IMPLEMENT_CLASS(SP_DLG_GlobalPreferences, wxDialog)
class SP_DS_Animation;
enum { //control id's
	SP_ID_RADIOBOX_NETCLASSES
};

BEGIN_EVENT_TABLE(SP_DLG_GlobalPreferences, wxDialog)
EVT_BUTTON(wxID_OK, SP_DLG_GlobalPreferences::OnDlgOk)
EVT_BUTTON(wxID_CANCEL, SP_DLG_GlobalPreferences::OnDlgCancel)

EVT_BUTTON(SP_ID_BUTTON_SOUND, SP_DLG_GlobalPreferences::OnUpdateSound)

EVT_RADIOBOX(SP_ID_RADIOBOX_FTREE_ANALYSIS, SP_DLG_GlobalPreferences::OnKlick)

EVT_RADIOBOX(SP_ID_RADIOBOX_NETCLASSES, SP_DLG_GlobalPreferences::OnUpdateNetClass)
EVT_RADIOBOX(SP_ID_RADIOBOX_TRANSSHAPE, SP_DLG_GlobalPreferences::OnUpdateTransShape)
EVT_SPINCTRL(SP_ID_SPINCTRL_TRANSWIDTH, SP_DLG_GlobalPreferences::OnUpdateTransWidth)
EVT_SPINCTRL(SP_ID_SPINCTRL_TRANSHEIGHT, SP_DLG_GlobalPreferences::OnUpdateTransHeight)
EVT_COMMAND_RANGE(SP_ID_BUTTON_FONT, SP_ID_BUTTON_FONT + 9,
				  wxEVT_COMMAND_BUTTON_CLICKED , SP_DLG_GlobalPreferences::OnUpdateFont)
END_EVENT_TABLE()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


SP_DLG_GlobalPreferences::SP_DLG_GlobalPreferences(wxWindow* p_pcParent)
: wxDialog(p_pcParent, -1, wxT("Global Preferences"), wxDefaultPosition, wxDefaultSize,
		   wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER | wxMAXIMIZE_BOX)
{

	m_pcCanvasPrefs = wxGetApp().GetCanvasPrefs();
	m_pcAnimationPrefs = wxGetApp().GetAnimationPrefs();
	m_pcElementPrefs = wxGetApp().GetElementPrefs();
	m_pcFontPrefs = wxGetApp().GetFontPrefs();

	int l_nNCCount = SP_Core::Instance()->GetNetclassCount();
	for(int i = 0; i < l_nNCCount; i++) {
		m_sNetClasses.Add(SP_Core::Instance()->GetNetclassName(i));
	}

	//preselect currently active netclass
	if (SP_Core::Instance()->GetRootDocument()) {
		m_sCurrentNetClass = SP_Core::Instance()->GetRootDocument()->GetNetclassName();
	} else {
		m_sCurrentNetClass = m_sNetClasses[0];
	}

    //dialog's main sizer and sizer for top section
    m_pcMainSizer = new wxBoxSizer(wxVERTICAL);
	m_pcTopSizer = new wxBoxSizer(wxHORIZONTAL);

	//left side: netclass selection
	m_rbNetClasses = new wxRadioBox(this, SP_ID_RADIOBOX_NETCLASSES, wxT("Net class"), wxDefaultPosition,
									wxDefaultSize, m_sNetClasses, 1, wxRA_SPECIFY_COLS);
	m_rbNetClasses->SetStringSelection(m_sCurrentNetClass);
	m_pcTopSizer->Add(m_rbNetClasses, 0, wxALL |  wxEXPAND, 5);

	//add some space between netclass selector and preference pages
	m_pcTopSizer->Add(10, 0, 0);

	//right side: preferences according to selected netclass
    m_pcNotebook = new SP_WDG_Notebook(this, wxT("Properties"), wxDefaultPosition, wxSize(400,400));

	CreateNotebookPages();

	m_pcTopSizer->Add(m_pcNotebook, 1, wxALL | wxEXPAND, 5);

    /* Buttons in the lower right hand corner */
    wxBoxSizer *l_pcButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	l_pcButtonSizer->Add(this->CreateButtonSizer(wxOK|wxCANCEL), 0, wxEXPAND | wxALL, 5);

	m_pcMainSizer->Add(m_pcTopSizer, 1, wxEXPAND);
    m_pcMainSizer->Add(l_pcButtonSizer, 0, wxEXPAND);

    SetSizerAndFit(m_pcMainSizer);

    Centre(wxBOTH);

	SP_LOGDEBUG(wxT("Config path: ") + wxFileConfig::GetLocalFileName(SP_APP_SHORT_NAME));
}

SP_DLG_GlobalPreferences::~SP_DLG_GlobalPreferences()
{
}


//////////////////////////////////////////////////////////////////////
// private help methods
//////////////////////////////////////////////////////////////////////

void
SP_DLG_GlobalPreferences::CreateNotebookPages()
{
	SP_WDG_NotebookPage* l_pcNotebookPage;
	m_pcNotebook->DeleteAllPages();

	l_pcNotebookPage = m_pcNotebook->AddOrGetPage(wxT("Canvas"));
	m_pcCanvasPrefs->AddToDialogPage(m_sCurrentNetClass, l_pcNotebookPage);

	l_pcNotebookPage = m_pcNotebook->AddOrGetPage(wxT("Animation"));
	m_pcAnimationPrefs->AddToDialogPage(m_sCurrentNetClass, l_pcNotebookPage);

	l_pcNotebookPage = m_pcNotebook->AddOrGetPage(wxT("Elements"));
	m_pcElementPrefs->AddToDialogPage(m_sCurrentNetClass, l_pcNotebookPage);

	l_pcNotebookPage = m_pcNotebook->AddOrGetPage(wxT("Fonts"));
	m_pcFontPrefs->AddToDialogPage(m_sCurrentNetClass, l_pcNotebookPage);

}

void
SP_DLG_GlobalPreferences::UpdateCanvas()
{
	SP_GUI_Childframe *l_pcFrame = wxGetApp().GetMainframe()->GetActiveChildframe();
    if (l_pcFrame)
    {
    	SP_GUI_Canvas* canvas = dynamic_cast<SP_MDI_View*>(l_pcFrame->GetView())->GetCanvas();
    	wxClientDC dc(canvas);
		this->UpdateAnimator();
    	canvas->DoPrepareDC(dc);
    	SP_Core::Instance()->GetRootDocument()->GetGraph()->ShowOnCanvas(canvas);

    }
}

void
SP_DLG_GlobalPreferences::UpdateAnimator()
{

		int tempInt;
		double tempDouble;
		wxString tempString;
		SP_SetString::iterator itNC;
		SP_SetString m_lsNCNames;
		int l_nNCCount = SP_Core::Instance()->GetNetclassCount();
		for (int i = 0; i < l_nNCCount; i++) {
			m_lsNCNames.insert(SP_Core::Instance()->GetNetclassName(i));
		}

		for (itNC = m_lsNCNames.begin(); itNC != m_lsNCNames.end(); ++itNC) {
			const wxString l_rString = *itNC;
			if (l_rString.CmpNoCase(SP_DS_PN_CLASS) == 0 ||
				l_rString.CmpNoCase(SP_DS_EXTPN_CLASS) == 0 ||
				l_rString.CmpNoCase(SP_DS_MUSICPN_CLASS) == 0 ||
				l_rString.CmpNoCase(SP_DS_MODULOPN_CLASS) == 0)


			{
				tempInt = m_pcAnimationPrefs->GetRefresh(l_rString);
				int tempDouble1= m_pcAnimationPrefs->GetDuration(l_rString);
				SP_Core::Instance()->ActivateAnimBAsedOnOptionSet(SP_Core::Instance()->GetRootDocument()->GetGraph(), tempInt, tempDouble1);
				SP_ANIM_STEP_T stepping= m_pcAnimationPrefs->GetStepping(l_rString);

				m_pcAnimationPrefs->SetStepping(l_rString, stepping);
				SP_Core::Instance()->SetAnimRunning(true);

			}
			else if (l_rString.CmpNoCase(SP_DS_COLPN_CLASS) == 0 ||
				l_rString.CmpNoCase(SP_DS_COLSPN_CLASS) == 0 ||
				l_rString.CmpNoCase(SP_DS_COLEPN_CLASS) == 0)
			{
				tempInt = m_pcAnimationPrefs->GetRefresh(l_rString);
				int tempDouble1 = m_pcAnimationPrefs->GetDuration(l_rString);
				SP_Core::Instance()->ActivateAnimBAsedOnOptionSet(SP_Core::Instance()->GetRootDocument()->GetGraph(), tempInt, tempDouble1);
				SP_ANIM_STEP_T stepping = m_pcAnimationPrefs->GetStepping(l_rString);
				SP_Core::Instance()->SetActivatedRefreshDuration(l_rString,tempInt, tempDouble1);
				//SP_Core::Instance()->SetActiv
				m_pcAnimationPrefs->SetStepping(l_rString, stepping);
			}
		}
}
//////////////////////////////////////////////////////////////////////
// Dialog events
//////////////////////////////////////////////////////////////////////

void
SP_DLG_GlobalPreferences::OnDlgOk(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
		m_pcCanvasPrefs->OnDialogOk(m_sCurrentNetClass);
		m_pcAnimationPrefs->OnDialogOk(m_sCurrentNetClass);
		m_pcElementPrefs->OnDialogOk(m_sCurrentNetClass);
		m_pcFontPrefs->OnDialogOk(m_sCurrentNetClass);

        if ( IsModal() )
            EndModal(wxID_OK);
        else
        {
		    SetReturnCode(wxID_OK);
		    this->Show(FALSE);
        }
	}
	UpdateCanvas();
}

void
SP_DLG_GlobalPreferences::OnDlgCancel(wxCommandEvent& p_cEvent)
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
	UpdateCanvas();
}

void
SP_DLG_GlobalPreferences::OnKlick(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
		m_pcAnimationPrefs->OnKlick(m_sCurrentNetClass);
	}
}

void
SP_DLG_GlobalPreferences::OnUpdateSound(wxCommandEvent& p_cEvent)
{
	if ( Validate() && TransferDataFromWindow() )
	{
		m_pcAnimationPrefs->OnUpdateSound();
	}
}

void
SP_DLG_GlobalPreferences::OnUpdateNetClass(wxCommandEvent& p_cEvent) {
	int sel = m_pcNotebook->GetSelection();

	if ( Validate() && TransferDataFromWindow() ) {
		m_pcCanvasPrefs->OnDialogOk(m_sCurrentNetClass);
		m_pcAnimationPrefs->OnDialogOk(m_sCurrentNetClass);
		m_pcElementPrefs->OnDialogOk(m_sCurrentNetClass);
		m_pcFontPrefs->OnDialogOk(m_sCurrentNetClass);
	}
	m_sCurrentNetClass = m_rbNetClasses->GetStringSelection();

	CreateNotebookPages();
	m_pcNotebook->SetSelection(sel);

	Layout();
}

void
SP_DLG_GlobalPreferences::OnUpdateTransShape(wxCommandEvent& p_cEvent) {

	if ( Validate() && TransferDataFromWindow() ) {
		m_pcElementPrefs->OnUpdateTransShape();
	}
}

void
SP_DLG_GlobalPreferences::OnUpdateTransWidth(wxSpinEvent& p_cEvent) {

	if ( Validate() && TransferDataFromWindow() ) {
		m_pcElementPrefs->OnUpdateTransWidth();
	}
}

void
SP_DLG_GlobalPreferences::OnUpdateTransHeight(wxSpinEvent& p_cEvent) {

	if ( Validate() && TransferDataFromWindow() ) {
		m_pcElementPrefs->OnUpdateTransHeight();
	}
}

void
SP_DLG_GlobalPreferences::OnUpdateFont(wxCommandEvent& p_cEvent) {
	m_pcFontPrefs->ChooseFont(this, m_sCurrentNetClass, p_cEvent.GetId() - SP_ID_BUTTON_FONT);
}
