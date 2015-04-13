//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: schroed $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/04/30 11:55:00 $
// Short Description: Dialog for SBML Import to extended Petrinets
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_IMPORTSBML2EXTPN_H__
#define __SP_DLG_IMPORTSBML2EXTPN_H__

#include <wx/wx.h>

class SP_GUI_Mainframe;


class SP_DLG_ImportSBML2extPN: public wxDialog
{
private:
    DECLARE_EVENT_TABLE()
		//! the top level sizer
		wxBoxSizer* m_pcSizer;
		wxCheckBox *m_cbShowReactionReverseReaction;
    DECLARE_CLASS(SP_DLG_ImportSBML2extPN)
protected:

public:
    SP_DLG_ImportSBML2extPN(SP_GUI_Mainframe* p_pcParent, 
							const wxString& p_sTitle = wxT("SBML Import"),
							long p_nStyle = wxDEFAULT_DIALOG_STYLE);

    void OnDlgOk(wxCommandEvent& p_cEvent);
    void OnDlgCancel(wxCommandEvent& p_cEvent);
	bool GetShowReverseReactions();
};

#endif // __SP_DLG_IMPORTSBML2EXTPN_H__

