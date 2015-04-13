//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/07/05 $
// Short Description: dialog to search for nodes by id
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_SEARCHID_H__
#define __SP_DLG_SEARCHID_H__

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "sp_core/SP_List.h"
#include "sp_core/base/SP_Error.h"
#include "sp_ds/SP_DS_Graph.h"

class SP_DLG_SearchId: public wxDialog, public SP_Error
{
private:
DECLARE_EVENT_TABLE()
DECLARE_CLASS(SP_DLG_SearchId)
protected:
	wxRadioBox* m_choice;
	wxRadioBox* m_select;

	wxSpinCtrl* m_idSpinCtrl;
	wxTextCtrl* m_NameTextCtrl;

	//by sl
	wxString m_sConstant;
	wxString l_sAttribute;
	const SP_ListEdge* m_edgeList;
	SP_ListEdge::const_iterator m_edgeIt;

	SP_ListNodeclass::const_iterator l_itNC;
	SP_ListEdgeclass::const_iterator l_itEC;



	wxCheckBox* m_exactName;

	wxButton* m_pcFind;
	wxButton* m_pcNext;
	wxButton* m_pcPrev;

	wxString m_title;
	wxString m_titlePrefix;

	size_t m_nSelect;
	wxString m_sSelect;
	long m_id;
	wxString m_sName;
	wxString m_nclass;

	SP_DS_Graph* m_graph;
	const SP_ListNode* m_nodeList;
	SP_ListNode::const_iterator m_nodeIt;

	list<SP_ListGraphic*>::const_iterator m_grConstIt;
	list<SP_ListGraphic*> m_grConst;
	SP_ListGraphic* m_grList;
	SP_ListGraphic::const_iterator m_grIt;

public:
	SP_DLG_SearchId(SP_DS_Graph *p_graph, wxWindow *p_parent,
			const wxString& p_title = wxT("Search nodes"));
	virtual ~SP_DLG_SearchId();

	void OnDlgOk(wxCommandEvent& p_cEvent);
	void OnChangedSelect(wxCommandEvent& p_cEvent);
	void OnFind(wxCommandEvent& p_cEvent);
	void OnNext(wxCommandEvent& p_cEvent);
	void OnPrev(wxCommandEvent& p_cEvent);

	void SetTitlePrefix(const wxString& p_val);
	wxString GetTitlePrefix();

	bool ShowFound();
	bool FindNextNode();

	bool FindNextElement();
	bool FindNextEdge();
	bool SearchNodeClasses();
	bool SearchEdgeClasses();
	bool SearchAllElements();

};

#endif // __SP_DLG_SEARCHID_H__
