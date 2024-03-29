//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_TRANSFORMGRAPHIC_H__
#define __SP_DLG_TRANSFORMGRAPHIC_H__

#include <wx/wx.h>
#include "sp_core/base/SP_Graphic.h"

class SP_GUI_Mainframe;

class SP_DLG_TransformGraphic: public wxDialog
{
private:
    DECLARE_EVENT_TABLE()
    //! the top level sizer
    wxBoxSizer* m_pcSizer;
    wxTextCtrl* m_pcRadiusCtrl;
    wxRadioBox* m_pcChoice;
    int m_nRadius;
    wxRealPoint m_cPosition;
    wxSize m_cSize;
    
    DECLARE_CLASS(SP_DLG_TransformGraphic)
protected:
    SP_ListGraphic* m_plGraphics;
    SP_MDI_View* m_pcView;

    void OnRadiobox(wxCommandEvent& p_cEvent);
    void DoAction();
public:
    SP_DLG_TransformGraphic(SP_GUI_Mainframe* p_pcParent,
        SP_MDI_View* p_pcView,
        SP_ListGraphic* p_plGraphics,
        wxRealPoint p_cPosition,
        wxSize p_cSize,
        const wxString& p_sTitle = wxT("Transform Graphics"),
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
};

#endif // __SP_DLG_TRANSFORMGRAPHIC_H__

