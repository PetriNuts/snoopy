//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_CHOSECOARSE_H__
#define __SP_DLG_CHOSECOARSE_H__

#include <wx/wx.h>
#include "sp_ds/SP_DS_Graph.h"

class SP_GUI_Mainframe;

class SP_DLG_ChoseCoarse: public wxDialog
{
private:
    DECLARE_EVENT_TABLE()
    //! the top level sizer
    wxBoxSizer* m_pcSizer;
    wxRadioBox* m_pcChoice;
    wxString m_sResultClass;
    
    DECLARE_CLASS(SP_DLG_ChoseCoarse)
protected:
public:
    SP_DLG_ChoseCoarse(SP_GUI_Mainframe* p_pcParent,
        const wxString& p_sTitle = wxT("Coarse Elements"),
        long p_nStyle = wxDEFAULT_DIALOG_STYLE);

    wxString GetResult() const { return m_sResultClass; }
    bool SetChoices(SP_DS_Graph* p_pcGraph);
    /* EVENTS */
    /**	\brief	The OnDlgOk function

	    \param	p_cEvent	the event object as generated by wx'

	    This method is called, if the user clicks the 'OK' button
    */
    void OnDlgOk(wxCommandEvent& p_cEvent);
    /**	\brief	The OnDlgCancel function

	    \param	p_cEvent	the event object as generated by wx'

	    This method is called, if the user clicks the 'Cancel' button, pushes ESC
        or closes the dialog with the window-close methods of the ui-manager.
    */
    void OnDlgCancel(wxCommandEvent& p_cEvent);
};

#endif // __SP_DLG_CHOSECOARSE_H__

