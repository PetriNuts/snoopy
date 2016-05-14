/*
 * SP_DLG_CSVExport.cpp
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 14.5.2016
 * @Description
 */
 //======================================================================================

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StDirectExportProperties.h"
#include "sp_gui/dialogs/dia_continuouspn/sp_dlg_csvexport.h"

SP_DLG_CSVExport::SP_DLG_CSVExport( SP_DS_StExportCSVEditDlgType p_eType,
						  wxDialog* p_pcParent,
						  wxString* p_psFilename,
						  int* p_pnSpacer,
						  bool* p_pbCompressExact,
						  const bool& p_bReplaceValues,
						  const double& p_nReplacedValues,
						  const wxString& p_sTitle,
						  long p_nStyle
						  ):
						  SP_DLG_StDirectExportProperties(p_eType,p_pcParent,
								                          p_psFilename,p_pnSpacer,
														  p_pbCompressExact,p_sTitle,
														  p_nStyle),
m_bReplaceValues(p_bReplaceValues),
m_nReplacedValues(p_nReplacedValues)
{
	    wxSizer* l_pcRowSizer = new wxBoxSizer( wxHORIZONTAL );

	    m_pcChkReplaceValues = new wxCheckBox( this, wxID_ANY, wxT("Replace with zero values less than") );

	    l_pcRowSizer->Add( m_pcChkReplaceValues, 1, wxLEFT | wxRIGHT | wxTOP, 5 );

	    m_pcTxtBoxReplacedValues=new wxTextCtrl( this, wxID_ANY,wxT("0"));

	    l_pcRowSizer->Add( m_pcTxtBoxReplacedValues, 0, wxLEFT | wxRIGHT | wxTOP, 5 );

	    m_pcPropertiesSizer->Add( l_pcRowSizer, 0, wxALL, 5 );

		SetSizerAndFit( m_pcMainSizer );
}

void SP_DLG_CSVExport::LoadData()
{
	SP_DLG_StDirectExportProperties::LoadData();

	m_pcChkReplaceValues->SetValue(m_bReplaceValues);

	m_pcTxtBoxReplacedValues->SetValue(wxString::Format(wxT("%g"),m_nReplacedValues));
}
bool SP_DLG_CSVExport::SaveData()
{
	SP_DLG_StDirectExportProperties::LoadData();

	m_bReplaceValues=m_pcChkReplaceValues->IsChecked();

	wxString l_sValue=m_pcTxtBoxReplacedValues->GetValue();

	return l_sValue.ToDouble(&m_nReplacedValues);
}



