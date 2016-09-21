//////////////////////////////////////////////////////////////////////
// $Source: SP_GPR_Animation.cpp
// $Author: MD $
// $Version: $
// $Revision: $
// $Date: 2005/09/19 $
// Short Description: contains global animation preferences
//////////////////////////////////////////////////////////////////////

#include "sp_core/SP_GPR_Animation.h"

#include "sp_revision.h"

#include "sp_utilities.h"

#include "sp_core/SP_Core.h"

#include "sp_gui/dialogs/SP_DLG_GlobalPreferences.h"

#include <wx/filename.h>
#include <wx/config.h>
#include <wx/spinctrl.h>
#include <wx/dirdlg.h>

enum { //control id's
	SP_ID_SPINCTRL_REFRESH,
	SP_ID_SPINCTRL_DURATION,
	SP_ID_RADIOBOX_STEPPING,
	SP_ID_SPINCTRL_HISTORY,
  SP_ID_SPINCTRL_ROUNDSIZE,
  SP_ID_SPINCTRL_ROUNDS,
  SP_ID_TEXTCTRL_TOLERANCE,
  SP_ID_SPINCTRL_REFRESHFREQU,
  SP_ID_TEXTCTRL_SOUNDPATH
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SP_GPR_Animation::SP_GPR_Animation() {
	m_bKeepMarking = DEFAULT_KEEPMARKING;
}

SP_GPR_Animation::~SP_GPR_Animation() {

}


//////////////////////////////////////////////////////////////////////
// Setter / Getter
//////////////////////////////////////////////////////////////////////

void
SP_GPR_Animation::SetRefresh(const wxString& p_sNetClass, int p_nVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		if (p_nVal > 0) {
			m_mnRefresh[p_sNetClass] = p_nVal;
		} else m_mnRefresh[p_sNetClass] = DEFAULT_REFRESH;
	}
}

void
SP_GPR_Animation::SetDuration(const wxString& p_sNetClass, int p_nVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		if (p_nVal > 0) {
			m_mnDuration[p_sNetClass] = p_nVal;
		} else m_mnDuration[p_sNetClass] = DEFAULT_DURATION;
	}
}

void
SP_GPR_Animation::SetStepping(const wxString& p_sNetClass, SP_ANIM_STEP_T p_eVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		m_meStepping[p_sNetClass] = p_eVal;
	}
}

void
SP_GPR_Animation::SetHistorySize(const wxString& p_sNetClass, int p_nVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		if (p_nVal >= 0 && p_nVal <= HISTORY_MAX) {
			m_mnHistorySize[p_sNetClass] = p_nVal;
		} else m_mnHistorySize[p_sNetClass] = HISTORY_MAX;
	}
}


void
SP_GPR_Animation::SetRoundSize(const wxString& p_sNetClass, int p_nVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		if (p_nVal >= 0) {
			m_mnRoundSize[p_sNetClass] = p_nVal;
		}
	}
}

void
SP_GPR_Animation::SetRounds(const wxString& p_sNetClass, int p_nVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		if (p_nVal >= 0) {
			m_mnRounds[p_sNetClass] = p_nVal;
		}
	}
}

void
SP_GPR_Animation::SetTolerance(const wxString& p_sNetClass, double p_nVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		if (p_nVal >= 0) {
			m_mnTolerance[p_sNetClass] = p_nVal;
		}
	}
}

void
SP_GPR_Animation::SetRefreshFrequ(const wxString& p_sNetClass, int p_nVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		if (p_nVal >= 0) {
			m_mnRefreshFrequ[p_sNetClass] = p_nVal;
		}
	}
}

void
SP_GPR_Animation::SetAnalysisType(const wxString& p_sNetClass, SP_FT_ANALYSIS_TYPE p_eVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		m_analysisType[p_sNetClass] = p_eVal;
	}
}

void
SP_GPR_Animation::SetSystemType(const wxString& p_sNetClass, SP_FT_SYSTEM_TYPE p_eVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		m_systemType[p_sNetClass] = p_eVal;
	}
}


void
SP_GPR_Animation::SetSoundPath(const wxString& p_sNetClass, const wxString& p_sVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		m_mSoundPath[p_sNetClass] = p_sVal;
	}
}


int
SP_GPR_Animation::GetRefresh(const wxString& p_sNetClass) {

	if (m_mnRefresh[p_sNetClass]>0) {
		return m_mnRefresh[p_sNetClass];
	} else {
		return DEFAULT_REFRESH;
	}
}

int
SP_GPR_Animation::GetDuration(const wxString& p_sNetClass) {

	if (m_mnDuration[p_sNetClass]>0) {
		return m_mnDuration[p_sNetClass];
	} else {
		return DEFAULT_DURATION;
	}
}

SP_ANIM_STEP_T
SP_GPR_Animation::GetStepping(const wxString& p_sNetClass) {

	if (m_meStepping[p_sNetClass]>0) {
		return m_meStepping[p_sNetClass];
	} else {
		return DEFAULT_STEPPING;
	}
}

int
SP_GPR_Animation::GetHistorySize(const wxString& p_sNetClass) {

	if (m_mnHistorySize[p_sNetClass] >= 0 && m_mnHistorySize[p_sNetClass] <= HISTORY_MAX) {
		return m_mnHistorySize[p_sNetClass];
	} else {
		return HISTORY_MAX;
	}
}

int
SP_GPR_Animation::GetRoundSize(const wxString& p_sNetClass) {

	if (m_mnRoundSize[p_sNetClass]>0) {
		return m_mnRoundSize[p_sNetClass];
	} else {
		return DEFAULT_ROUNDSIZE;
	}
}

int
SP_GPR_Animation::GetRounds(const wxString& p_sNetClass) {

	if (m_mnRounds[p_sNetClass]>0) {
		return m_mnRounds[p_sNetClass];
	} else {
		return DEFAULT_ROUNDS;
	}
}

double
SP_GPR_Animation::GetTolerance(const wxString& p_sNetClass) {

	if (m_mnTolerance[p_sNetClass]>0) {
		return m_mnTolerance[p_sNetClass];
	} else {
		return DEFAULT_TOLERANCE;
	}
}

int
SP_GPR_Animation::GetRefreshFrequ(const wxString& p_sNetClass) {

	if (m_mnRefreshFrequ[p_sNetClass]>0) {
		return m_mnRefreshFrequ[p_sNetClass];
	} else {
		return DEFAULT_REFRESHFREQU;
	}
}

SP_FT_ANALYSIS_TYPE
SP_GPR_Animation::GetAnalysisType(const wxString& p_sNetClass) {

	if (m_analysisType[p_sNetClass]>0) {
		return m_analysisType[p_sNetClass];
	} else {
		return DEFAULT_ANALYSIS;
	}
}

SP_FT_SYSTEM_TYPE
SP_GPR_Animation::GetSystemType(const wxString& p_sNetClass) {

	if (m_systemType[p_sNetClass]>0) {
		return m_systemType[p_sNetClass];
	} else {
		return DEFAULT_SYSTEM;
	}
}

void
SP_GPR_Animation::SetShowPath(const wxString& p_sNetClass, bool p_bVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		m_mShowPath[p_sNetClass] = p_bVal;
	}
}


wxString
SP_GPR_Animation::GetSoundPath(const wxString& p_sNetClass) {

	if (m_mSoundPath.find(p_sNetClass) != m_mSoundPath.end()) {
		return m_mSoundPath[p_sNetClass];
	} else {
		return wxFileName::GetCwd();
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool
SP_GPR_Animation::AddToDialogPage(const wxString& p_sNetClass, SP_WDG_NotebookPage *p_pcPage)
{
	wxSizer* l_pcSizer = new wxBoxSizer(wxVERTICAL);
	//add some space at top

	l_pcSizer->Add(0, 20, 0);

	//the following two net classes will appear like this...
	//you can add your new net class to this list,
	//or you can create a new else-if-branch if you need a different appearance
	if (p_sNetClass.CmpNoCase(SP_DS_PN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_EXTPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_COLPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_COLEPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_MODULOPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_FAULTTREE_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_MUSICPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_EXTFAULTTREE_CLASS) == 0) {

		wxBoxSizer *helpSizer, *helpSizer2;
		wxString steppings[] = { wxT("Maximum"), wxT("Intermediate"), wxT("Single")};

		helpSizer = new wxBoxSizer(wxHORIZONTAL);

		helpSizer2 = new wxBoxSizer(wxVERTICAL);
		helpSizer2->Add(new wxStaticText(p_pcPage, -1, wxT("Refresh Frequency (ms): ")), 0,
										wxALL, 4);
		helpSizer2->Add(new wxStaticText(p_pcPage, -1, wxT("Step Duration (ms): ")), 0,
										wxALL, 4);
		helpSizer2->Add(new wxStaticText(p_pcPage, -1, wxT("Size of Step History: ")), 0,
										wxALL, 4);
		helpSizer->Add(helpSizer2);

		helpSizer2 = new wxBoxSizer(wxVERTICAL);
		m_scRefresh = new wxSpinCtrl(p_pcPage, SP_ID_SPINCTRL_REFRESH, wxString::Format(wxT("%d"), GetRefresh(p_sNetClass)),
						wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, INT_MAX, GetRefresh(p_sNetClass));
		helpSizer2->Add(m_scRefresh, 0, wxLEFT, 5);
		m_scDuration = new wxSpinCtrl(p_pcPage, SP_ID_SPINCTRL_DURATION, wxString::Format(wxT("%d"), GetDuration(p_sNetClass)),
						wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, INT_MAX, GetDuration(p_sNetClass));
		helpSizer2->Add(m_scDuration, 0, wxLEFT, 5);
		m_scHistorySize = new wxSpinCtrl(p_pcPage, SP_ID_SPINCTRL_HISTORY, wxString::Format(wxT("%d"), GetHistorySize(p_sNetClass)),
						wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, HISTORY_MAX, GetHistorySize(p_sNetClass));
		helpSizer2->Add(m_scHistorySize, 0, wxLEFT, 5);
		helpSizer->Add(helpSizer2, 0, wxEXPAND);
		l_pcSizer->Add(helpSizer, 0, wxALL, 5);




			m_rbStepping = new wxRadioBox(p_pcPage, SP_ID_RADIOBOX_STEPPING, wxT("Stepping"), wxDefaultPosition,
																	wxDefaultSize, 3, steppings, 1, wxRA_SPECIFY_ROWS);
			m_rbStepping->SetSelection(GetStepping(p_sNetClass) - 1);
			l_pcSizer->Add(m_rbStepping, 0, wxALL, 10);


	} else if (p_sNetClass.CmpNoCase(SP_DS_CONTINUOUSPED_CLASS) == 0) {

		wxBoxSizer *helpSizer, *helpSizer2;

		helpSizer = new wxBoxSizer(wxHORIZONTAL);

		helpSizer2 = new wxBoxSizer(wxVERTICAL);
		helpSizer2->Add(new wxStaticText(p_pcPage, -1, wxT("Step Size (ms): ")), 0,
										wxALL, 4);
		helpSizer2->Add(new wxStaticText(p_pcPage, -1, wxT("Round Number: ")), 0,
										wxALL, 4);
		helpSizer2->Add(new wxStaticText(p_pcPage, -1, wxT("Rel. Tolerance: ")), 0,
										wxALL, 4);
		helpSizer2->Add(new wxStaticText(p_pcPage, -1, wxT("Simulation Speed (ms): ")), 0,
										wxALL, 4);
		helpSizer->Add(helpSizer2);

		helpSizer2 = new wxBoxSizer(wxVERTICAL);
		m_scRoundSize = new wxSpinCtrl(p_pcPage, SP_ID_SPINCTRL_ROUNDSIZE, wxString::Format(wxT("%d"), GetRoundSize(p_sNetClass)),
						wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, INT_MAX, GetRefresh(p_sNetClass));
		helpSizer2->Add(m_scRoundSize, 0, wxLEFT, 5);
		m_scRounds = new wxSpinCtrl(p_pcPage, SP_ID_SPINCTRL_ROUNDS, wxString::Format(wxT("%d"), GetRounds(p_sNetClass)),
						wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, INT_MAX, GetDuration(p_sNetClass));
		helpSizer2->Add(m_scRounds, 0, wxLEFT, 5);
		m_tcTolerance = new wxTextCtrl(p_pcPage, SP_ID_TEXTCTRL_TOLERANCE, wxString::Format(wxT("%f"), GetTolerance(p_sNetClass)),
						wxDefaultPosition, wxDefaultSize, 0);
		helpSizer2->Add(m_tcTolerance, 0, wxLEFT, 5);
		m_scRefreshFrequ = new wxSpinCtrl(p_pcPage, SP_ID_SPINCTRL_REFRESHFREQU, wxString::Format(wxT("%d"), GetRefreshFrequ(p_sNetClass)),
						wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, HISTORY_MAX, GetHistorySize(p_sNetClass));
		helpSizer2->Add(m_scRefreshFrequ, 0, wxLEFT, 5);
		helpSizer->Add(helpSizer2, 0, wxEXPAND);
		l_pcSizer->Add(helpSizer, 0, wxALL, 5);



	} else /*DEFAULT*/ {
		l_pcSizer->Add(new wxStaticText(p_pcPage, -1, wxT("No editable animation preferences for this class.")), 0, wxALL, 5);
	}

	if (p_sNetClass.CmpNoCase(SP_DS_FAULTTREE_CLASS) == 0
		|| p_sNetClass.CmpNoCase(SP_DS_EXTFAULTTREE_CLASS) == 0) {

		wxStaticBoxSizer *l_pcSectionSizer;

		l_pcSectionSizer = new wxStaticBoxSizer(new wxStaticBox(
			p_pcPage, -1, wxT("FaultTree - Analysis")), wxVERTICAL);
		wxString analysisTypes[] = { wxT("qualitative"), wxT("quantitative")};

		m_analysisTypes = new wxRadioBox(p_pcPage, SP_ID_RADIOBOX_FTREE_ANALYSIS, wxT("Type"), wxDefaultPosition,
																	wxDefaultSize, 2, analysisTypes, 1, wxRA_SPECIFY_ROWS);
		m_analysisTypes->SetSelection(GetAnalysisType(p_sNetClass));
		l_pcSectionSizer->Add(m_analysisTypes, 0, wxEXPAND |wxALL, 5);



		wxString systemTypes[] = { wxT("repairablew"), wxT("irreparable")};

		m_systemTypes = new wxRadioBox(p_pcPage, -1, wxT("System"), wxDefaultPosition,
																	wxDefaultSize, 2, systemTypes, 1, wxRA_SPECIFY_COLS);
		m_systemTypes->SetSelection(GetSystemType(p_sNetClass));

		if (GetAnalysisType(p_sNetClass) == SP_FT_QUALITATIVE){
			m_systemTypes->Enable(false);
		}else{
			m_systemTypes->Enable(true);
		}

		l_pcSectionSizer->Add(m_systemTypes, 0, wxEXPAND |wxALL, 5);

		l_pcSizer->Add(l_pcSectionSizer, 0, wxEXPAND | wxALL, 5);

		/*m_showPath = new wxCheckBox(p_pcPage, -1, wxT("Show most critical path"));
		m_showPath->SetValue(ShowPath(p_sNetClass));
		l_pcSizer->Add(m_showPath, 0, wxEXPAND |wxALL, 10);*/
	}


	if (p_sNetClass.CmpNoCase(SP_DS_MUSICPN_CLASS) == 0)
	{
		wxStaticBoxSizer *l_pcSoundSectionSizer = new wxStaticBoxSizer(new wxStaticBox(p_pcPage, -1, wxT("Sound Library Path")), wxHORIZONTAL);

		m_tcSoundPath = new wxTextCtrl(p_pcPage, SP_ID_TEXTCTRL_SOUNDPATH, wxString::Format(wxT("%s"), GetSoundPath(p_sNetClass).c_str()),
						wxDefaultPosition, wxSize(220, 22), 0);
		l_pcSoundSectionSizer->Add(m_tcSoundPath, 0, wxLEFT, 5);

		l_pcSoundSectionSizer->Add( new wxButton( p_pcPage, SP_ID_BUTTON_SOUND, wxT("Browse")), 0, wxRIGHT | wxLEFT, 5);

		l_pcSizer->Add(l_pcSoundSectionSizer, 0, wxSHRINK | wxALL, 5);
	}

	p_pcPage->AddControl(l_pcSizer,0,wxEXPAND);

	return true;
}

bool
SP_GPR_Animation::OnDialogOk(const wxString& p_sNetClass) {

	if (p_sNetClass.CmpNoCase(SP_DS_PN_CLASS) == 0
		|| p_sNetClass.CmpNoCase(SP_DS_EXTPN_CLASS) == 0
		|| p_sNetClass.CmpNoCase(SP_DS_MODULOPN_CLASS) == 0
		|| p_sNetClass.CmpNoCase(SP_DS_FAULTTREE_CLASS) == 0
		|| p_sNetClass.CmpNoCase(SP_DS_MUSICPN_CLASS) == 0
		|| p_sNetClass.CmpNoCase(SP_DS_EXTFAULTTREE_CLASS) == 0)
	{
		long l_nRefresh = m_scRefresh->GetValue();
		long l_nDuration = m_scDuration->GetValue();
		if((l_nRefresh*4) > l_nDuration)
		{
			l_nDuration = l_nRefresh*4;
		}
		SetRefresh(p_sNetClass, l_nRefresh);
		SetDuration(p_sNetClass, l_nDuration);

		if (p_sNetClass.CmpNoCase(SP_DS_FAULTTREE_CLASS) != 0
			&& p_sNetClass.CmpNoCase(SP_DS_EXTFAULTTREE_CLASS) != 0) {
			SetStepping(p_sNetClass, (SP_ANIM_STEP_T)(m_rbStepping->GetSelection() + 1));
		}
		SetHistorySize(p_sNetClass, m_scHistorySize->GetValue());

		if (p_sNetClass.CmpNoCase(SP_DS_FAULTTREE_CLASS) == 0 ||
			p_sNetClass.CmpNoCase(SP_DS_EXTFAULTTREE_CLASS) == 0)
		{
			SetAnalysisType(p_sNetClass, (SP_FT_ANALYSIS_TYPE)(m_analysisTypes->GetSelection()));
			SetSystemType(p_sNetClass, (SP_FT_SYSTEM_TYPE)(m_systemTypes->GetSelection()));
		}


		if (p_sNetClass.CmpNoCase(SP_DS_MUSICPN_CLASS) == 0)
		{
			SetSoundPath(p_sNetClass, m_tcSoundPath->GetValue());
		}


	}
	else if (p_sNetClass.CmpNoCase(SP_DS_CONTINUOUSPED_CLASS) == 0)
	{
		SetRoundSize(p_sNetClass, m_scRoundSize->GetValue());
		SetRounds(p_sNetClass, m_scRounds->GetValue());
		wxString m_sDlgTolerance = m_tcTolerance->GetValue();
		double l_nDoubleVal;
		if (m_sDlgTolerance.ToDouble(&l_nDoubleVal) && (l_nDoubleVal > 0))
		{
			SetTolerance(p_sNetClass, l_nDoubleVal);
		}
		SetRefreshFrequ(p_sNetClass, m_scRefreshFrequ->GetValue());
	}
	return true;
}

bool
SP_GPR_Animation::OnKlick(const wxString& p_sNetClass)
{
	if (p_sNetClass.CmpNoCase(SP_DS_FAULTTREE_CLASS) == 0 || p_sNetClass.CmpNoCase(SP_DS_EXTFAULTTREE_CLASS) == 0)
	{
		if (m_analysisTypes->GetSelection() == SP_FT_QUANTITATIVE)
		{
			m_systemTypes->Enable(true);
		}
		else
		{
			m_systemTypes->Enable(false);
		}
	}
	return TRUE;
}

bool
SP_GPR_Animation::UpdateFromConfig(wxConfig& p_config) {
	//temps for reading values from config
	int tempInt;
	double tempDouble;
	wxString tempString;
	SP_SetString::iterator itNC;

	int l_nNCCount = SP_Core::Instance()->GetNetclassCount();
	for(int i = 0; i < l_nNCCount; i++) {
		m_lsNCNames.insert(SP_Core::Instance()->GetNetclassName(i));
	}

	for (itNC = m_lsNCNames.begin(); itNC != m_lsNCNames.end(); ++itNC) {
		const wxString l_rString = *itNC;
		if (l_rString.CmpNoCase(SP_DS_PN_CLASS) == 0 ||
			l_rString.CmpNoCase(SP_DS_EXTPN_CLASS) == 0 ||
			l_rString.CmpNoCase(SP_DS_MUSICPN_CLASS) == 0 ||
			l_rString.CmpNoCase(SP_DS_MODULOPN_CLASS) == 0)
		{
			p_config.Read(wxString::Format(wxT("%s//AnimRefresh"), l_rString.c_str()),
				    &tempInt, DEFAULT_REFRESH);
			m_mnRefresh[l_rString] = tempInt;
			p_config.Read(wxString::Format(wxT("%s//AnimDuration"), l_rString.c_str()),
				    &tempInt, DEFAULT_DURATION);
			m_mnDuration[l_rString] = tempInt;
			p_config.Read(wxString::Format(wxT("%s//AnimStepping"), l_rString.c_str()),
				    &tempInt, DEFAULT_STEPPING);
			m_meStepping[l_rString] = (SP_ANIM_STEP_T)tempInt;
			p_config.Read(wxString::Format(wxT("%s//AnimHistorySize"), l_rString.c_str()),
						&tempInt, HISTORY_MAX);
			m_mnHistorySize[l_rString] = tempInt;

			if (l_rString.CmpNoCase(SP_DS_FAULTTREE_CLASS) == 0 || l_rString.CmpNoCase(SP_DS_EXTFAULTTREE_CLASS) == 0)
			{
				p_config.Read(wxString::Format(wxT("%s//Type"), l_rString.c_str()),
				&tempInt, DEFAULT_ANALYSIS);
				m_analysisType[l_rString] = (SP_FT_ANALYSIS_TYPE)tempInt;

				p_config.Read(wxString::Format(wxT("%s//System"), l_rString.c_str()),
				&tempInt, DEFAULT_SYSTEM);
				m_systemType[l_rString] = (SP_FT_SYSTEM_TYPE)tempInt;
			}
		}
		if (l_rString.CmpNoCase(SP_DS_CONTINUOUSPED_CLASS) == 0)
		{
			p_config.Read(wxString::Format(wxT("%s//AnimRoundSize"), l_rString.c_str()),
				    &tempInt, DEFAULT_ROUNDSIZE);
			m_mnRoundSize[l_rString] = tempInt;
			p_config.Read(wxString::Format(wxT("%s//AnimRounds"), l_rString.c_str()),
				    &tempInt, DEFAULT_ROUNDS);
			m_mnRounds[l_rString] = tempInt;
			p_config.Read(wxString::Format(wxT("%s//AnimTolerance"), l_rString.c_str()),
				    &tempDouble, DEFAULT_TOLERANCE);
			m_mnTolerance[l_rString] = tempInt;
			p_config.Read(wxString::Format(wxT("%s//AnimRefreshFrequ"), l_rString.c_str()),
				    &tempInt, DEFAULT_REFRESHFREQU);
			m_mnRefreshFrequ[l_rString] = tempInt;
		}

		if (l_rString.CmpNoCase(SP_DS_MUSICPN_CLASS) == 0)
		{
			p_config.Read(wxString::Format(wxT("%s//SoundPath"), l_rString.c_str()),
				    &tempString, wxT(""));
			if(!tempString.IsEmpty())
			{
				m_mSoundPath[l_rString] = tempString;
			}
		}
	}
	return true;
}

bool
SP_GPR_Animation::WriteToConfig(wxConfig& p_config)
{
	SP_SetString::iterator itNC;

	for (itNC = m_lsNCNames.begin(); itNC != m_lsNCNames.end(); ++itNC)
	{
		const wxString l_rString = *itNC;
		if (l_rString.CmpNoCase(SP_DS_PN_CLASS) == 0 ||
			l_rString.CmpNoCase(SP_DS_EXTPN_CLASS) == 0 ||
			l_rString.CmpNoCase(SP_DS_MODULOPN_CLASS) == 0 ||
			l_rString.CmpNoCase(SP_DS_MUSICPN_CLASS) == 0)
		{
			p_config.Write(wxString::Format(wxT("%s//AnimRefresh"), l_rString.c_str()),
								(long)m_mnRefresh[l_rString]);
			p_config.Write(wxString::Format(wxT("%s//AnimDuration"), l_rString.c_str()),
								(long)m_mnDuration[l_rString]);
			p_config.Write(wxString::Format(wxT("%s//AnimStepping"), l_rString.c_str()),
								(long)m_meStepping[l_rString]);
			p_config.Write(wxString::Format(wxT("%s//AnimHistorySize"), l_rString.c_str()),
								(long)m_mnHistorySize[l_rString]);
		}
		if (l_rString.CmpNoCase(SP_DS_FAULTTREE_CLASS) == 0	||
			l_rString.CmpNoCase(SP_DS_EXTFAULTTREE_CLASS) == 0)
		{
			p_config.Write(wxString::Format(wxT("%s//Type"), l_rString.c_str()),
								(long)m_analysisType[l_rString]);

			p_config.Write(wxString::Format(wxT("%s//System"), l_rString.c_str()),
								(long)m_systemType[l_rString]);
		}
		if (l_rString.CmpNoCase(SP_DS_CONTINUOUSPED_CLASS) == 0)
		{
			p_config.Write(wxString::Format(wxT("%s//AnimRoundSize"), l_rString.c_str()),
				    (long)m_mnRoundSize[l_rString]);
			p_config.Write(wxString::Format(wxT("%s//AnimRounds"), l_rString.c_str()),
				    (long)m_mnRounds[l_rString]);
			p_config.Write(wxString::Format(wxT("%s//AnimTolerance"), l_rString.c_str()),
				    m_mnTolerance[l_rString]);
			p_config.Write(wxString::Format(wxT("%s//AnimRefreshFrequ"), l_rString.c_str()),
				    (long)m_mnRefreshFrequ[l_rString]);
		}

		if (l_rString.CmpNoCase(SP_DS_MUSICPN_CLASS) == 0)
		{
			p_config.Write(wxString::Format(wxT("%s//SoundPath"), l_rString.c_str()),
				    m_mSoundPath[l_rString]);
		}
	}
	return true;
}


void
SP_GPR_Animation::OnUpdateSound()
{
	const wxString& dir = wxDirSelector(wxT("Choose a folder"), m_tcSoundPath->GetValue());
	if (!dir.empty())
		m_tcSoundPath->SetValue(dir);
}
