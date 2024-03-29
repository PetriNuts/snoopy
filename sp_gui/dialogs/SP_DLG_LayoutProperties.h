//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: cr $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2005/08/22 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_LAYOUTPROPERTIES_H__
#define __SP_DLG_LAYOUTPROPERTIES_H__

#include <wx/wx.h>
#include "sp_gui/mdi/SP_MDI_Doc.h"


enum SP_LAYOUT_VIEWS
{
	SP_LAYOUT_ACTUAL_VIEW,
	SP_LAYOUT_ALL_VIEWS,
};

class SP_GUI_Mainframe;


class SP_DLG_LayoutProperties: public wxDialog
{
private:
    DECLARE_EVENT_TABLE()
    //! the top level sizer
    wxBoxSizer* m_pcSizer;
    wxTextCtrl* m_pcNameCtrl;
    wxTextCtrl* m_pcValueCtrl;
	wxRadioBox* m_pcChooseViewChoice;
	wxChoice*	m_pcLayoutChoice;

	int m_nRadius;
    wxRealPoint m_cPosition;
    wxSize m_cSize;

	SP_MDI_Doc* m_pcDoc;
    map<wxCheckBox*, SP_DS_Attribute*> m_mAttributes;
	SP_MapString2String m_lAttributes;

    DECLARE_CLASS(SP_DLG_LayoutProperties)
protected:

public:
    SP_DLG_LayoutProperties(SP_GUI_Mainframe* p_pcParent,
        SP_MDI_Doc* p_pcDoc,
        const wxString& p_sTitle = wxT("Layout Properties"),
        long p_nStyle = wxDEFAULT_DIALOG_STYLE);

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

	SP_MapString2String GetAttributes();
	wxString GetLayout();
	SP_LAYOUT_VIEWS GetView();

	/*set layout algo by passing algo number, mandatory for the command line layout*/
	void SetLayoutAlgo(int l_nAlgo);//by george


};

#endif // __SP_DLG_LAYOUTPROPERTIES_H__

