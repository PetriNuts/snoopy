//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_ANIMATIONPROPERTIES__
#define __SP_DLG_ANIMATIONPROPERTIES__

#include <wx/wx.h>
class SP_DS_Animation;

class SP_DLG_AnimationProperties: public wxDialog
{
private:
    DECLARE_EVENT_TABLE()
    //! the top level sizer
    wxBoxSizer* m_pcSizer;
    wxStaticText* m_pcError;
    SP_DS_Animation* m_pcAnimation;

    DECLARE_CLASS(SP_DLG_AnimationProperties)
protected:
public:
    SP_DLG_AnimationProperties(SP_DS_Animation* p_pcAnim,
        wxWindow* p_pcParent,
        const wxString& p_sTitle = wxT("Animation Properties"),
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

#endif // __SP_DLG_ANIMATIONPROPERTIES__

