//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: George Assaf $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2020/06/30 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_IMPORTPROPERTIES__
#define __SP_DLG_IMPORTPROPERTIES__

#include <wx/wx.h>
#include <wx/filepicker.h>

#include "sp_gui/widgets/SP_WDG_Notebook.h"
#include "sp_gui/widgets/SP_WDG_NotebookPage.h"
#include "sp_ds/attributes/SP_DS_ColListAttribute.h"

//bysl
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "dssd/misc/net.h"
#include <wx/rearrangectrl.h>
class SP_ImportRoutine;
class SP_MDI_Doc;

class SP_DLG_ImportProperties : public wxDialog
{
private:
	DECLARE_EVENT_TABLE()
	//! the top level sizer
	wxBoxSizer* m_pcSizer;
	 
	wxBoxSizer* m_pcCheckBoxSizer;
	wxBoxSizer* m_pcCheckBoxSizerAll;
	wxBoxSizer* m_pcCheckBoxSizerSelectAll;
	//! the notebook to have multiple pages, if needed
	SP_WDG_Notebook* m_pcNotebook;
	SP_ImportRoutine* m_pcImport;
	//wxFilePickerCtrl* m_pcFilePickerCtrl;
 
	dssd::andl::Net_ptr m_pcDoc;
 
  
	DECLARE_CLASS(SP_DLG_ImportProperties)

protected:
	//void SaveData();
	//void AddConstantsToControl();
public:
	SP_DLG_ImportProperties(SP_ImportRoutine* p_pcAnim,
		wxWindow* p_pcParent,
		dssd::andl::Net_ptr p_Net,
		const wxString& p_sFilename,
		const wxString& p_sTitle = wxT("Import Definitions"),
		long p_nStyle = wxDEFAULT_DIALOG_STYLE);

	SP_WDG_NotebookPage* AddPage(const wxString& p_sLabel = wxT(""));
	/* EVENTS */
	/**	\brief	The OnDlgOk function

	\param	p_cEvent	the event object as generated by wx'

	This method is called, if the user clicks the 'OK' button
	*/
	void OnDlgOk(wxCommandEvent& p_cEvent);
	/**	\brief	The OnDlgCancel function

	\param p_cEvent	the event object as generated by wx'

	This method is called, if the user clicks the 'Cancel' button, pushes ESC
	or closes the dialog with the window-close methods of the ui-manager.
	*/
	void OnDlgCancel(wxCommandEvent& p_cEvent);

	void SetNotebookSize(int width, int height); //added by anjali (for export to latex)

	void AddCheckBox(wxCheckBox* l_pcCheckBox);

	void AddCheckBoxSelectAll(wxCheckBox* p_pcCheckBox);
												 /*const wxString& m_sFilename; */
	//wxString GetCurrentFileName() { return  m_pcFilePickerCtrl->GetPath(); }

//	bool GetIncludeToCurrentdocState();
};

#endif // __SP_DLG_IMPORTPROPERTIES__

