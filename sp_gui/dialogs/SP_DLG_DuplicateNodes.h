//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2015/10/08 $
// Short Description: dialog to duplicate nodes
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_SORTDUPLICATENODES_H__
#define __SP_DLG_SORTDUPLICATENODES_H__

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>
#include "sp_ds/SP_DS_Graph.h"

struct SP_ItemData
{
	SP_Graphic* pGraphic;
	int count;
	wxString name;

	SP_ItemData(SP_Graphic* gr, int c, wxString n)
			: pGraphic(gr), count(c), name(n)
	{}
};

int wxCALLBACK SP_CompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData);

class SP_DLG_DuplicateNodes: public wxDialog, public SP_Error
{
private:
DECLARE_EVENT_TABLE()
DECLARE_CLASS(SP_DLG_DuplicateNodes)
protected:
	wxStaticText* m_text;
	wxSpinCtrl* m_number;
	wxRadioBox* m_choice;
	wxRadioBox* m_sort;
	wxRadioBox* m_direction;

	wxString m_nclass;
	wxString m_sAttr;

	bool m_bDirection;

	SP_DS_Graph* m_graph;

	unsigned m_netNumber;

	SP_ListGraphic* m_graphics;

	wxListCtrl* m_pcListCtrl;

	int* dragging;

	bool m_bListCtrlChanged;
	int m_nSel;

	virtual void LoadData(const wxString& p_sNodeClassName, int p_nNrArcs);
	void Clear();
	void DoApply();
public:
	SP_DLG_DuplicateNodes(SP_DS_Graph *p_graph, unsigned int p_netNumber, SP_ListGraphic* p_graphics, wxWindow *p_parent,
			const wxString& p_title = wxT("Duplicate Nodes"));
	virtual ~SP_DLG_DuplicateNodes();

	void OnDlgOk(wxCommandEvent& p_cEvent);
	void OnDlgClose(wxCommandEvent& p_cEvent);
	void OnDlgApply( wxCommandEvent& p_cEvent );
	void OnRadio( wxCommandEvent& p_cEvent );
	void OnSpin( wxSpinEvent& p_cEvent );
	void OnColClick(wxListEvent& event);

};

#endif // __SP_DLG_SORTDUPLICATENODES_H__
