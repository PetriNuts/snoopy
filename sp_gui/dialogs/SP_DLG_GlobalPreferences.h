//////////////////////////////////////////////////////////////////////
// $Source: SP_DLG_GlobalPreferences.h
// $Author: MD $
// $Version: $
// $Revision: $
// $Date: 2005/09/19 $
// Short Description:  The dialog for the global preferences
//////////////////////////////////////////////////////////////////////

#if !defined(__SP_DLG_GLOBALPREFERENCES_H__)
#define __SP_DLG_GLOBALPREFERENCES_H__

#include <wx/wx.h>
#include <wx/spinctrl.h>

#include "sp_gui/widgets/SP_WDG_Notebook.h"
#include "sp_gui/widgets/SP_WDG_NotebookPage.h"

class SP_GPR_Canvas;
class SP_GPR_Animation;
class SP_GPR_Elements;
class SP_GPR_Fonts;


class SP_DLG_GlobalPreferences: public wxDialog {

private:
  DECLARE_CLASS(SP_DLG_GlobalPreferences)
	DECLARE_EVENT_TABLE()
protected:
	wxBoxSizer *m_pcMainSizer;
	wxBoxSizer *m_pcTopSizer;

	//left side of dialog
	wxRadioBox *m_rbNetClasses;
	//right side of dialog
	SP_WDG_Notebook *m_pcNotebook;

	SP_GPR_Animation *m_pcAnimationPrefs;
	SP_GPR_Canvas *m_pcCanvasPrefs;
	SP_GPR_Elements *m_pcElementPrefs;
	SP_GPR_Fonts *m_pcFontPrefs;

	wxArrayString m_sNetClasses;
	wxString m_sCurrentNetClass;

	void CreateNotebookPages();
	void UpdateCanvas();
	void UpdateAnimator();//by george 26.10.2021
protected:
	void OnDlgOk(wxCommandEvent& p_cEvent);
	void OnDlgCancel(wxCommandEvent& p_cEvent);
	void OnKlick(wxCommandEvent& p_cEvent);
	void OnUpdateSound(wxCommandEvent& p_cEvent);
	void OnUpdateNetClass(wxCommandEvent& p_cEvent);
	void OnUpdateTransShape(wxCommandEvent& p_cEvent);
	void OnUpdateTransWidth(wxSpinEvent& p_cEvent);
	void OnUpdateTransHeight(wxSpinEvent& p_cEvent);
	void OnUpdateFont(wxCommandEvent& p_cEvent);

public:
	SP_DLG_GlobalPreferences(wxWindow *p_pcParent);
	virtual ~SP_DLG_GlobalPreferences();

};

#endif // !defined(__SP_DLG_GLOBALPREFERENCES_H__)
