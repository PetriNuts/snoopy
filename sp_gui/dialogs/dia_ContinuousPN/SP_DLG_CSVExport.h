/*
 * SP_DLG_CSVExport.h
 * $Author: M.Herajy
 * $Version: 0.0 $
 * $Revision: 0.0 $
 * $Date: 14.5.2016
 * @Description
 */
//=============================================================*/
#ifndef SP_GUI_DIALOGS_DIA_CONTINUOUSPN_SP_DLG_CSVEXPORT_H_
#define SP_GUI_DIALOGS_DIA_CONTINUOUSPN_SP_DLG_CSVEXPORT_H_

/*
 * A customized export dialog for continuous Petri nets
 */
class SP_DLG_StDirectExportProperties;

class SP_DLG_CSVExport: public SP_DLG_StDirectExportProperties
{
protected:

	     wxCheckBox* m_pcChkReplaceValues;

	     wxTextCtrl* m_pcTxtBoxReplacedValues;

protected:

	     bool m_bReplaceValues;

	     double m_nReplacedValues;

protected:

	     virtual void LoadData();

	     virtual bool SaveData();

public:
	   SP_DLG_CSVExport( SP_DS_StExportCSVEditDlgType p_eType,
						  wxDialog* p_pcParent,
						  wxString* p_psFilename,
						  int* p_pnSpacer,
						  bool* p_pbCompressExact,
						  const bool& p_bReplaceValues,
						  const double& p_nReplacedValues,
						  const wxString& p_sTitle = wxT("Direct export properties"),
						  long p_nStyle = wxDEFAULT_DIALOG_STYLE
						  );


	   bool GetReplaceValue(){return m_bReplaceValues;}
	   double GetReplacedValues(){return m_nReplacedValues;}

};




#endif /* SP_GUI_DIALOGS_DIA_CONTINUOUSPN_SP_DLG_CSVEXPORT_H_ */
