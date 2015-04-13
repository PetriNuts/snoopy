//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2008/11/17 $
// Short Description: dialog to sort nodes by attribute
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_SORTNODES_H__
#define __SP_DLG_SORTNODES_H__

#include <wx/wx.h>
#include <wx/listctrl.h>
#include "sp_ds/SP_DS_Graph.h"

class SP_DLG_SortNodes: public wxDialog, public SP_Error
{
private:
DECLARE_EVENT_TABLE()
DECLARE_CLASS(SP_DLG_SortNodes)
protected:
	wxRadioBox* m_choice;
	wxRadioBox* m_sort;
	wxRadioBox* m_direction;

	wxString m_nclass;
	wxString m_sAttr;

	bool m_bDirection;

	SP_DS_Graph* m_graph;

	wxListCtrl* m_pcListCtrl;

	int* dragging;

	bool m_bListCtrlChanged;
	int m_nSel;

private:
	void LoadData(wxString p_sNodeClassName);

	void OnBeginDrag( wxListEvent& event );
	void OnLeftUp( wxMouseEvent& event );
	void OnMotion( wxMouseEvent& e );
	void swap( wxListCtrl* list, int index1, int index2 );

	void DoApply();
public:
	SP_DLG_SortNodes(SP_DS_Graph *p_graph, wxWindow *p_parent,
			const wxString& p_title = wxT("Sort Nodes"));
	virtual ~SP_DLG_SortNodes();

	void OnDlgOk(wxCommandEvent& p_cEvent);
	void OnDlgClose(wxCommandEvent& p_cEvent);
	void OnDlgApply( wxCommandEvent& p_cEvent );
	void OnRadio( wxCommandEvent& p_cEvent );

	void OnKeyDown(wxKeyEvent& p_cEvent);
	void OnKeyUp(wxKeyEvent& p_cEvent);
	void OnSelected( wxListEvent& event );

	void OnAdvanced(wxCommandEvent& p_cEvent);

	bool Sort(const wxString& p_sNode, const wxString& p_sAttribute, bool p_bDirection);
};

#endif // __SP_DLG_SORTNODES_H__
