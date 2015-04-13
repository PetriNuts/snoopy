//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: fei liu $
// $Version: 0.0 $
// $Revision: 1.0 $
// $Date: 2009/10/11 11:20:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_FunctionAssistent__
#define __SP_DLG_FunctionAssistent__

#include "sp_gui/dialogs/dia_CPN/SP_DLG_ExpressionAssistent.h"


class SP_DLG_FunctionAssistent : public SP_DLG_ExpressionAssistent
{

  protected:

    DECLARE_EVENT_TABLE()    
   
    virtual void LoadVariable();
	virtual void LoadFunctions();

	vector<wxString> m_vParameter;

    DECLARE_CLASS( SP_DLG_FunctionAssistent )

  public:


    SP_DLG_FunctionAssistent(vector<wxString> p_vParameter, wxString p_psColorsetName, SP_CPN_ColorSetClass& p_pcColorSetClass,
      wxString p_psReturnText, wxWindow* p_pcParent, wxString p_sHelpText = wxT(""),
      const wxString& p_sTitle = wxT("Assistent"),
      long p_nStyle = wxDEFAULT_DIALOG_STYLE );

    void OnDlgOk( wxCommandEvent& p_cEvent );
    void OnDlgCancel( wxCommandEvent& p_cEvent );

    virtual void OnCheckFormula( wxCommandEvent& p_cEvent );

    
};

#endif // __SP_DLG_FunctionAssistent__

