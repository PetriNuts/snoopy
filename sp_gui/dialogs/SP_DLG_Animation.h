//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: mf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2003/09/12 11:55:00 $
// Short Description: 
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_ANIMATION__
#define __SP_DLG_ANIMATION__

#include <wx/wx.h>
#include "sp_core/base/SP_Error.h"

class SP_GUI_Mainframe;
class SP_DS_Animation;

class SP_DLG_Animation: public wxDialog	, public SP_Error
{
private:
    DECLARE_EVENT_TABLE()
    //! the top level sizer
    wxBoxSizer* m_pcSizer;
    SP_DS_Animation* m_pcAnimation;
	wxWindow* m_pcParent;

		//controls
		wxBitmap m_bitmaps[6];
		wxBitmapButton *m_buPlayBwd, *m_buStepBwd, *m_buReset, *m_buStepFwd, *m_buPlayFwd;

		//true, if backstepping is allowed - false, otherwise
		bool m_bBackStepping;

    DECLARE_CLASS(SP_DLG_Animation)
protected:
    void OnMyClose(wxCloseEvent& p_cEvent);
    void OnOptions(wxCommandEvent& p_cEvent);

    void OnUpdateUI(wxUpdateUIEvent& p_cEvent);
public:
    SP_DLG_Animation(SP_DS_Animation* p_pcAnim,
        SP_GUI_Mainframe* p_pcParent);

    bool CleanUp();
	wxWindow* getWindow();
    /* EVENTS */
		void OnAnimPlayBwd(wxCommandEvent& p_cEvent);
    void OnAnimStepBwd(wxCommandEvent& p_cEvent);
		void OnReset(wxCommandEvent& p_pcEvent);
    void OnAnimStepFwd(wxCommandEvent& p_cEvent);
    void OnAnimPlayFwd(wxCommandEvent& p_cEvent);
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

		//to reset the bitmap if animation is not stopped by a button click
		void ResetPlayButtons();
		//back stepping must be explicitly enabled by the animation itself
		inline void EnableBackStepping(bool p_bVal) { m_bBackStepping = p_bVal; }
};

#endif // __SP_DLG_ANIMATION__

