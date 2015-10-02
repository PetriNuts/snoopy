//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: slehrack $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/09/26 11:20:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_STDIRECTEXPORTPROPERTIES__
#define __SP_DLG_STDIRECTEXPORTPROPERTIES__

#include "sp_gui/dialogs/dia_SPN/SP_DLG_StSimulationResults.h"

#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/filepicker.h>

typedef enum {
  
  SP_ST_SIM_EXPORT_CSV_EDIT_DLG_DIRECT,
  SP_ST_SIM_EXPORT_CSV_EDIT_DLG_EXPLICIT

} SP_DS_StExportCSVEditDlgType;

class SP_DLG_StDirectExportProperties : public wxDialog
{

  private:
  
    SP_DS_StExportCSVEditDlgType m_eDlgType;

    wxChoice* m_pcSpacerChoice;
    wxBoxSizer* m_pcMainSizer;
    wxFilePickerCtrl* m_pcFilePickerCtrl;
    wxCheckBox* m_pcCompressCheckBox;

    wxString* m_psFilename;
    int* m_pnSpacer;
    bool* m_pbCompressExact;

    void LoadData();
    bool SaveData();

  protected:
  
  public:
  
    SP_DLG_StDirectExportProperties( SP_DS_StExportCSVEditDlgType p_eType, 
      wxDialog* p_pcParent,
      wxString* p_psFilename,
      int* p_pnSpacer,
      bool* p_pbCompressExact,
      const wxString& p_sTitle = wxT("Direct export properties"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );

    DECLARE_CLASS( SP_DLG_StDirectExportProperties )
    DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_StDirectExportProperties__

