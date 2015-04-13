//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: dbayer $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2008/09/01 11:20:00 $
// Short Description: Dialog class to choose algorithms from, that
//				      add sounds to petri nets automatically
//////////////////////////////////////////////////////////////////////

#ifndef __SP_DLG_APPLY_SOUNDS_H__
#define __SP_DLG_APPLY_SOUNDS_H__

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/grid.h>
#include "sp_core/SP_Vector.h"
#include "sp_ds/SP_DS_Node.h"

class SP_DS_ColListAttribute;
class SP_DS_Node;

class SP_DLG_ApplySound : public wxDialog
{

private:

	wxString m_sLibrary;
	wxWindow* m_pcParent;
	vector<SP_DS_ColListAttribute*> m_lAttrList;
	wxBoxSizer* m_pcSizer;

	// METHODS
	//net structure
	wxRadioButton* m_pcNetStructureRadioButton;

	//net structure options
	wxCheckBox* m_pcNetStructureConsumingCheckBox;
	wxCheckBox* m_pcNetStructureProducingCheckBox;
	wxCheckBox* m_pcNetStructurePreservingCheckBox;

	wxButton* m_pcNetStructureConsumingButton;
	wxButton* m_pcNetStructureProducingButton;
	wxButton* m_pcNetStructurePreservingButton;

	wxTextCtrl* m_pcNetStructureConsumingCtrl;
	wxTextCtrl* m_pcNetStructureProducingCtrl;
	wxTextCtrl* m_pcNetStructurePreservingCtrl;

	// general options
	wxRadioButton* m_pcApplyToAllRadioButton;
	wxRadioButton* m_pcApplyToSelectedRadioButton;

protected:

	/** \brief Invoked on clicking OK button

		\param p_cEvent the event object (button)

		This closes the dialog and eventually attaches the sounds
	t	he the nodes
	*/
	void OnDlgOk(wxCommandEvent& p_cEvent);

	/** Invoked on clicking Cancel button

		\param p_cEvent the event object (button)
		This only closes the dialog
	*/
	void OnDlgCancel(wxCommandEvent& p_cEvent);

	/** \brief updates UI
	*/
	void OnNetStructureCheckBoxClicked(wxCommandEvent& p_cEvent);

	/** \brief opens dir chooser
	*/
	void OnBrowseButtonClicked(wxCommandEvent& p_cEvent);

	/** \brief applies the sounds by netstructre algorithm (token consuming
		producing and preserving transitions)
	*/
	void ApplySoundsByNetStructure();

	/** \brief helper method that applies all the sounds from the given directory
		to the nodes in the list, regardless of the selected algorithm

		\param p_Transitions list of transitions to attach the sounds to
		\param p_sSoundDir the directory from which the sounds are taken
		\return true if successful, false of not (i.e the directory was empty)
	*/
	bool ApplySounds(SP_ListNode &p_Transitions, const wxString& p_sSoundDir);

	/** \brief saves all settings so the dialog can display them again next time
	*/
	void SaveSettings();

	/** \brief loads all settings from config
	*/
	void LoadSettings();

	/** \brief checks if a  file is supported by the audio library
	*/
	bool IsSupportedFile(wxFileName& p_fileName);

public:

	SP_DLG_ApplySound(const wxString& p_sAttrName, wxWindow* p_pcParent,
			const wxString& p_sTitle = wxT("Apply Sounds"),
			long p_nStyle = wxDEFAULT_DIALOG_STYLE);

DECLARE_EVENT_TABLE()

};

#endif // __SP_DLG_APPLY_SOUNDS_H__
