//////////////////////////////////////////////////////////////////////
// $Source: SP_GPR_Canvas.cpp
// $Author: MD $
// $Version: $
// $Revision: $
// $Date: 2005/09/19 $
// Short Description: contains global canvas preferences
//////////////////////////////////////////////////////////////////////

#include "sp_core/SP_GPR_Canvas.h"

#include "sp_revision.h"
#include "snoopy.h"
#include "sp_core/SP_Core.h"

#include "sp_gui/dialogs/SP_DLG_GlobalPreferences.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"

#include <wx/spinctrl.h>

enum { //control id's
	SP_ID_SPINCTRL_GRID_SPACING,
	SP_ID_CHECKBOX_GRID_SNAPPING,
	SP_ID_SPINCTRL_GRID_SIZE,
	SP_ID_SPINCTRL_WINDOW_SIZE_X,
	SP_ID_SPINCTRL_WINDOW_SIZE_Y,
	SP_ID_CHECKBOX_LOG_VERBOSE,
	SP_ID_CHECKBOX_COMPRESS_FILE,
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SP_GPR_Canvas::SP_GPR_Canvas() {

}

SP_GPR_Canvas::~SP_GPR_Canvas() {

}


//////////////////////////////////////////////////////////////////////
// Setter
//////////////////////////////////////////////////////////////////////

void
SP_GPR_Canvas::SetLogVerbose(bool p_bVal) {

	m_bLogVerbose = p_bVal;
	wxLog::SetVerbose(p_bVal);
}

void
SP_GPR_Canvas::SetCompressFile(bool p_bVal) {

	m_bCompressFile = p_bVal;
}

void
SP_GPR_Canvas::SetGridSpacing(const wxString& p_sNetClass, int p_nVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		if (p_nVal >= SP_GRID_MIN && p_nVal <= SP_GRID_MAX) {
			m_mnGridSpacing[p_sNetClass] = p_nVal;
		} else m_mnGridSpacing[p_sNetClass] = SP_DEFAULT_GRID_SPACING;
	}
}

void
SP_GPR_Canvas::SetGridVisible(const wxString& p_sNetClass, bool p_bVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		m_mbGridVisible[p_sNetClass] = p_bVal;
	}
}

void
SP_GPR_Canvas::SetGridSnapping(const wxString& p_sNetClass, bool p_bVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		m_mbGridSnappingEnabled[p_sNetClass] = p_bVal;
	}
}

void
SP_GPR_Canvas::SetGridSize(const wxString& p_sNetClass, int p_nVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		if (p_nVal >= SP_GRID_SIZE_MIN && p_nVal <= SP_GRID_SIZE_MAX) {
			m_mnGridSize[p_sNetClass] = p_nVal;
		} else m_mnGridSize[p_sNetClass] = SP_DEFAULT_GRID_SIZE;
	}
}

void
SP_GPR_Canvas::SetWindowSizeX(const wxString& p_sNetClass, int p_nVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		if (p_nVal >= 100 && p_nVal <= 10000) {
			m_mnWindowSizeX[p_sNetClass] = p_nVal;
		} else m_mnWindowSizeX[p_sNetClass] = SP_WINDOW_SIZE_X;
	}
}

void
SP_GPR_Canvas::SetWindowSizeY(const wxString& p_sNetClass, int p_nVal) {

	if (m_lsNCNames.find(p_sNetClass) != m_lsNCNames.end()) {
		if (p_nVal >= 100 && p_nVal <= 10000) {
			m_mnWindowSizeY[p_sNetClass] = p_nVal;
		} else m_mnWindowSizeY[p_sNetClass] = SP_WINDOW_SIZE_Y;
	}
}

void
SP_GPR_Canvas::SetWindowOptions(const wxString& p_sKey, const SP_WindowOptions& p_WindowOptions)
{
	wxString l_sKey = SP_NormalizeString(p_sKey);
	m_mWindowOptions[l_sKey] = p_WindowOptions;
}

const SP_WindowOptions&
SP_GPR_Canvas::GetWindowOptions(const wxString& p_sKey)
{
	wxString l_sKey = SP_NormalizeString(p_sKey);

	SP_MapString2WindowOptions::iterator it = m_mWindowOptions.find(l_sKey);
	if(it != m_mWindowOptions.end())
	{
		return it->second;
	}
	else
	{
		wxConfig config(SP_APP_SHORT_NAME, SP_APP_VENDOR);
		wxString l_Tmp;
		if(config.Read(wxString::Format(wxT("WindowOptions//%s"), l_sKey.c_str()),
				&l_Tmp))
		{
			SP_WindowOptions l_Options;
			if(wxFromString(l_Tmp, &l_Options))
			{
				m_mWindowOptions.insert(make_pair(l_sKey, l_Options));
			}
		}
	}
	return m_mWindowOptions[l_sKey];
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

bool
SP_GPR_Canvas::AddToDialogPage(const wxString& p_sNetClass, SP_WDG_NotebookPage *p_pcPage)
{
	wxSizer* l_pcSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer *elemsizer = new wxBoxSizer(wxHORIZONTAL);

	//add some space at top
	l_pcSizer->Add(0, 20, 0);

	//the following net classes will appear like this...
	//you can add your new net class to this list,
	//or you can create a new else-if-branch if you need a different appearance
	if (p_sNetClass.CmpNoCase(SP_DS_PN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_EXTPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_CONTINUOUSPED_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_SPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_COLSPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_COLEPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_COLPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_COLCPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_COLHPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_MODULOPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_REACHABILITY_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_FAULTTREE_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_EXTFAULTTREE_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_MTBDD_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_MUSICPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_TIMEPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_MTIDD_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_HYBRIDPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_FREESTYLE_CLASS) == 0)
	{
		elemsizer->Add(new wxStaticText(p_pcPage, -1, wxString::Format(wxT("Grid spacing (%d...%dpt): "),SP_GRID_MIN, SP_GRID_MAX)), 0,
										wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 10);
		m_scGridSpacing = new wxSpinCtrl(p_pcPage, SP_ID_SPINCTRL_GRID_SPACING, wxString::Format(wxT("%d"), GetGridSpacing(p_sNetClass)),
				wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, SP_GRID_MIN, SP_GRID_MAX, GetGridSpacing(p_sNetClass));
		elemsizer->Add(m_scGridSpacing);
		l_pcSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM, 10);

		m_cbGridVisible = new wxCheckBox(p_pcPage, -1, wxT("Show Grid"));
		m_cbGridVisible->SetValue(GridVisible(p_sNetClass));
		l_pcSizer->Add(m_cbGridVisible, 0, wxALL, 10);

		m_cbGridSnappingEnabled = new wxCheckBox(p_pcPage, SP_ID_CHECKBOX_GRID_SNAPPING, wxT("Snap elements to grid"));
		m_cbGridSnappingEnabled->SetValue(GridSnappingEnabled(p_sNetClass));
		l_pcSizer->Add(m_cbGridSnappingEnabled, 0, wxALL, 10);

		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		elemsizer->Add(new wxStaticText(p_pcPage, -1, wxString::Format(wxT("Grid size (%d...%dpt): "), SP_GRID_SIZE_MIN, SP_GRID_SIZE_MAX)), 0,
										wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 10);
		m_scGridSize = new wxSpinCtrl(p_pcPage, SP_ID_SPINCTRL_GRID_SIZE, wxString::Format(wxT("%d"), GetGridSize(p_sNetClass)),
				wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, SP_GRID_SIZE_MIN, SP_GRID_SIZE_MAX, GetGridSize(p_sNetClass));
		elemsizer->Add(m_scGridSize);
		l_pcSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM, 10);

		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		elemsizer->Add(new wxStaticText(p_pcPage, -1, wxString::Format(wxT("Window size X (%d...%d): "), 100, 10000)), 0,
										wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 10);
		m_scWindowSizeX = new wxSpinCtrl(p_pcPage, SP_ID_SPINCTRL_WINDOW_SIZE_X, wxString::Format(wxT("%d"), GetWindowSizeX(p_sNetClass)),
				wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 100, 10000, GetWindowSizeX(p_sNetClass));
		elemsizer->Add(m_scWindowSizeX);
		l_pcSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM, 10);

		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		elemsizer->Add(new wxStaticText(p_pcPage, -1, wxString::Format(wxT("Window size Y (%d...%d): "), 100, 10000)), 0,
										wxRIGHT | wxLEFT | wxALIGN_CENTER_VERTICAL, 10);
		m_scWindowSizeY = new wxSpinCtrl(p_pcPage, SP_ID_SPINCTRL_WINDOW_SIZE_Y, wxString::Format(wxT("%d"), GetWindowSizeY(p_sNetClass)),
				wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 100, 10000, GetWindowSizeY(p_sNetClass));
		elemsizer->Add(m_scWindowSizeY);
		l_pcSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM, 10);

		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		m_cbLogVerbose = new wxCheckBox(p_pcPage, SP_ID_CHECKBOX_LOG_VERBOSE, wxT("Show verbose log messages"));
		m_cbLogVerbose->SetValue(GetLogVerbose());
		elemsizer->Add(m_cbLogVerbose, 0, wxALL, 10);
		l_pcSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM, 10);

		elemsizer = new wxBoxSizer(wxHORIZONTAL);
		m_cbCompressFile = new wxCheckBox(p_pcPage, SP_ID_CHECKBOX_COMPRESS_FILE, wxT("Enable file compression"));
		m_cbCompressFile->SetValue(GetCompressFile());
		elemsizer->Add(m_cbCompressFile, 0, wxALL, 10);
		l_pcSizer->Add(elemsizer, 0, wxTOP | wxBOTTOM, 10);

	} else /*DEFAULT*/ {
		l_pcSizer->Add(new wxStaticText(p_pcPage, -1, wxT("No editable canvas preferences for this class.")), 0,
									wxALL | wxALIGN_CENTER_VERTICAL, 5);
	}

	p_pcPage->AddControl(l_pcSizer,0,wxEXPAND);

	return true;
}

bool
SP_GPR_Canvas::OnDialogOk(const wxString& p_sNetClass) {

	if (p_sNetClass.CmpNoCase(SP_DS_PN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_EXTPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_CONTINUOUSPED_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_SPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_COLSPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_COLEPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_COLPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_COLCPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_COLHPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_MODULOPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_REACHABILITY_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_FAULTTREE_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_EXTFAULTTREE_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_MTBDD_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_MUSICPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_TIMEPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_MTIDD_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_HYBRIDPN_CLASS) == 0
			|| p_sNetClass.CmpNoCase(SP_DS_FREESTYLE_CLASS) == 0){
		SetGridSpacing(p_sNetClass, m_scGridSpacing->GetValue());
		SetGridVisible(p_sNetClass, m_cbGridVisible->IsChecked());
		SetGridSnapping(p_sNetClass, m_cbGridSnappingEnabled->IsChecked());
		SetGridSize(p_sNetClass, m_scGridSize->GetValue());
		SetWindowSizeX(p_sNetClass, m_scWindowSizeX->GetValue());
		SetWindowSizeY(p_sNetClass, m_scWindowSizeY->GetValue());
		SetLogVerbose(m_cbLogVerbose->IsChecked());
		SetCompressFile(m_cbCompressFile->IsChecked());
	}
	return true;
}

bool
SP_GPR_Canvas::UpdateFromConfig(wxConfig& p_config) {
	//temps for reading values from config
	int tempInt;
	bool tempBool;
	SP_SetString::iterator itNC;

	int l_nCount = SP_Core::Instance()->GetNetclassCount();
	for(int i = 0; i < l_nCount; i++) {
		m_lsNCNames.insert(SP_Core::Instance()->GetNetclassName(i));
	}

	for (itNC = m_lsNCNames.begin(); itNC != m_lsNCNames.end(); ++itNC) {
		const wxString l_rString = *itNC;
		p_config.Read(wxString::Format(wxT("%s//GridSpacing"), l_rString.c_str()),
			    &tempInt, SP_DEFAULT_GRID_SPACING);
		m_mnGridSpacing[l_rString] = tempInt;
		p_config.Read(wxString::Format(wxT("%s//GridVisible"), l_rString.c_str()),
			    &tempBool, SP_DEFAULT_GRID_VISIBLE);
		m_mbGridVisible[l_rString] = tempBool;
		p_config.Read(wxString::Format(wxT("%s//GridSnappingEnabled"), l_rString.c_str()),
			    &tempBool, SP_DEFAULT_GRID_SNAPPING);
		m_mbGridSnappingEnabled[l_rString] = tempBool;
		p_config.Read(wxString::Format(wxT("%s//GridSize"), l_rString.c_str()),
			    &tempInt, SP_DEFAULT_GRID_SIZE);
		m_mnGridSize[l_rString] = tempInt;
		p_config.Read(wxString::Format(wxT("%s//WindowSizeX"), l_rString.c_str()),
			    &tempInt, SP_WINDOW_SIZE_X);
		m_mnWindowSizeX[l_rString] = tempInt;
		p_config.Read(wxString::Format(wxT("%s//WindowSizeY"), l_rString.c_str()),
			    &tempInt, SP_WINDOW_SIZE_Y);
		m_mnWindowSizeY[l_rString] = tempInt;
	}

	p_config.Read(wxString::Format(wxT("LogVerbose")),
		    &tempBool, SP_DEFAULT_LOG_VERBOSE);
	m_bLogVerbose = tempBool;
	p_config.Read(wxString::Format(wxT("CompressFile")),
		    &tempBool, SP_DEFAULT_COMPRESS_FILE);
	m_bCompressFile = tempBool;

	return true;
}

bool
SP_GPR_Canvas::WriteToConfig(wxConfig& p_config) {
  SP_SetString::iterator itNC;

	for (itNC = m_lsNCNames.begin(); itNC != m_lsNCNames.end(); ++itNC) {
		const wxString l_rString = *itNC;
		p_config.Write(wxString::Format(wxT("%s//GridSpacing"), l_rString.c_str()),
			     (long)m_mnGridSpacing[l_rString]);
		p_config.Write(wxString::Format(wxT("%s//GridVisible"), l_rString.c_str()),
			     m_mbGridVisible[l_rString]);
		p_config.Write(wxString::Format(wxT("%s//GridSnappingEnabled"), l_rString.c_str()),
			     m_mbGridSnappingEnabled[l_rString]);
		p_config.Write(wxString::Format(wxT("%s//GridSize"), l_rString.c_str()),
				(long)m_mnGridSize[l_rString]);
		p_config.Write(wxString::Format(wxT("%s//WindowSizeX"), l_rString.c_str()),
				(long)m_mnWindowSizeX[l_rString]);
		p_config.Write(wxString::Format(wxT("%s//WindowSizeY"), l_rString.c_str()),
				(long)m_mnWindowSizeY[l_rString]);
	}
	p_config.Write(wxT("LogVerbose"), m_bLogVerbose);
	p_config.Write(wxT("CompressFile"), m_bCompressFile);

	SP_MapString2WindowOptions::iterator it;
	for(it = m_mWindowOptions.begin(); it != m_mWindowOptions.end(); ++it)
	{
		const wxString& l_Key = it->first;
		const SP_WindowOptions& l_Option = it->second;
		p_config.Write(wxString::Format(wxT("WindowOptions//%s"), l_Key.c_str()),
				wxToString(l_Option));
	}

	return true;
}

void
SP_GPR_Canvas::LoadWindowOptions(wxFrame* window)
{
    const SP_WindowOptions& l_Options = GetWindowOptions(window->GetTitle());
    int l_PosX = l_Options.PosX;
    int l_PosY = l_Options.PosY;
	wxSize l_DisplaySize = wxGetDisplaySize();
    if( l_PosX <= 0 || l_PosY <= 0
    	|| l_PosX >= l_DisplaySize.GetX()
    	|| l_PosY >= l_DisplaySize.GetY() )
    {
    	l_DisplaySize.Scale(0.3,0.3);
    	l_PosX = l_DisplaySize.GetX();
    	l_PosY = l_DisplaySize.GetY();
    }
    window->SetPosition(wxPoint(l_PosX, l_PosY));
    window->SetSize(l_Options.SizeX, l_Options.SizeY);
    if(l_Options.isMaximized)
    {
    	window->Maximize();
    }
}

void
SP_GPR_Canvas::SaveWindowOptions(wxFrame* window)
{
    SP_WindowOptions l_Options;
    int l_X = 0;
    int l_Y = 0;
    window->GetPosition(&l_X, &l_Y);
    l_Options.PosX = l_X;
    l_Options.PosY = l_Y;

    window->GetSize(&l_X, &l_Y);
    l_Options.SizeX = l_X;
    l_Options.SizeY = l_Y;

    l_Options.isMaximized = window->IsMaximized();

	SetWindowOptions(window->GetTitle(), l_Options);
}

wxString wxToString(const SP_WindowOptions& p_WindowOptions)
{
	wxString l_Ret;
	l_Ret << p_WindowOptions.PosX << wxT(";");
	l_Ret << p_WindowOptions.PosY << wxT(";");
	l_Ret << p_WindowOptions.SizeX << wxT(";");
	l_Ret << p_WindowOptions.SizeY << wxT(";");
	l_Ret << (p_WindowOptions.isMaximized ? wxT("1") : wxT("0"));

	return l_Ret;
}

bool wxFromString(const wxString& p_String, SP_WindowOptions* p_WindowOptions)
{
	wxArrayString l_Token = wxStringTokenize(p_String, wxT(";"));
	if(l_Token.GetCount() != 5)
	{
		return false;
	}
	long l_PosX = 0;
	if(!l_Token[0].ToLong(&l_PosX))
	{
		return false;
	}
	long l_PosY = 0;
	if(!l_Token[1].ToLong(&l_PosY))
	{
		return false;
	}
	long l_SizeX = 0;
	if(!l_Token[2].ToLong(&l_SizeX))
	{
		return false;
	}
	long l_SizeY = 0;
	if(!l_Token[3].ToLong(&l_SizeY))
	{
		return false;
	}
	long l_Max = 0;
	if(!l_Token[4].ToLong(&l_Max))
	{
		return false;
	}

	p_WindowOptions->PosX = l_PosX;
	p_WindowOptions->PosY = l_PosY;
	p_WindowOptions->SizeX = l_SizeX;
	p_WindowOptions->SizeY = l_SizeY;
	p_WindowOptions->isMaximized = l_Max;

	return true;
}
