//////////////////////////////////////////////////////////////////////
// $Author: kw $
// $Version: 0.1 $
// $Date: 2005/12/28 $
// Short Description: dialog to search for nodes by id
////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_CHOOSEINVARIANT_H__
#define __SP_DLG_CHOOSEINVARIANT_H__

#include <wx/wx.h>
#include <wx/clrpicker.h>
#include "sp_core/SP_List.h"
#include "sp_core/base/SP_Error.h"
#include "sp_ds/SP_DS_NodeVectorSystem.h"
#include "sp_ds/SP_DS_Visitor.h"
#include "sp_core/tools/SP_NodeColourizer.h"

class SP_DS_Graph;
class wxSpinCtrl;
class SP_DS_Node;
class SP_Graphic;

/**	\brief	Dialog to choose and highlight an Invariant

 Used to display all Invariants loaded from an Invariant file.
 Changes to the graph are temporary until the next save.
 Changes to the names of invariants and their comments are replayed
 to the invariants file.
 */
class SP_DLG_ChooseInvariant : public wxDialog, public SP_Error,
	public SP_DS_Visitor
{
private:
	wxString m_sIntersection;
	wxString m_sUnion;

DECLARE_EVENT_TABLE()
DECLARE_CLASS(SP_DLG_ChooseInvariant)

	SP_NodeColourizer *m_pcNodeColourizer;
	bool m_colorize;
	bool m_bChanged;

	wxString m_fileName;

	wxListBox* m_pcListBox;
	wxArrayString m_pcListBoxValues;

	wxCheckBox* m_pcCheckBoxGradientColouring;
	wxCheckBox* m_pcKeepInvariant;
	wxCheckBox* m_pcSelectNodes;
	wxColourPickerCtrl* m_pcButtonColour;

	// every entry in the List is mapped to the right NodeVectorid in nvs
	SP_MapLong2Long *m_pcMapListNum2NodeVectorNum;

	void InitGUI();
	void UpdateList();

	// Array to handle multiple selections
	wxArrayInt m_aVectorNumbers;

protected:
	wxRadioBox* m_pcChoice;
	wxSpinCtrl *m_idSpinCtrl;
	wxString m_title;
	wxString m_titlePrefix;

	// number of selected item in the list of nodevectors
	long m_id;
	wxString m_nclass;

	SP_DS_Graph* m_pcGraph;
	SP_ListNode* m_nodeList;
	SP_ListNode::const_iterator m_nodeIt;
	SP_DS_NodeVectorSystem* m_pcNvs;

public:
	SP_DLG_ChooseInvariant(SP_DS_Graph *m_pcGraph, wxWindow *p_parent,
			const wxString& p_title = wxT("Colour Node Vector"));

	virtual ~SP_DLG_ChooseInvariant();

	void OnUpdateUI(wxUpdateUIEvent& p_event);
	void OnDlgOk(wxCommandEvent& p_cEvent);
	void OnChoosingInvariant();
	bool LoadInvariantFile();

	void DiscardColours();
	// choose first entry
	void OnFirst(wxCommandEvent& p_cEvent);
	void OnLast(wxCommandEvent& p_cEvent);

	// choose next entry
	void OnNext(wxCommandEvent& p_cEvent);

	// choose  previous entry
	void OnPrev(wxCommandEvent& p_cEvent);

	//colour selected entry(s)
	void OnKlick(wxCommandEvent& p_cEvent);

	void OnColour(wxColourPickerEvent& p_cEvent);
	void RefreshColour();

	//open dialog with more informations about the selected entry
	void OnDoubleKlick(wxCommandEvent& p_cEvent);

	void OnKeyPressed(wxCommandEvent& p_cEvent);

	void SetTitlePrefix(const wxString& p_val);
	wxString GetTitlePrefix();

	bool Visit(SP_DS_NodeVector * p_pcNodeVector);
};

#endif // __SP_DLG_CHOOSEINVARIANT_H__
