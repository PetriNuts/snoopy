//////////////////////////////////////////////////////////////////////
// $Author: liu $
// $Version: 0.1 $
// $Date: 2010/08/17 $
// Short Description: dialog to check a net
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_CheckNet_H__
#define __SP_DLG_CheckNet_H__

#include <wx/wx.h>
#include "sp_ds/SP_DS_Graph.h"
#include "sp_gui/mdi/SP_MDI_View.h"


class SP_DLG_CheckNet: public wxDialog, public SP_Error
{
private:
DECLARE_EVENT_TABLE()
DECLARE_CLASS(SP_DLG_CheckNet)
protected:
	SP_DS_Graph* m_graph;
	SP_ListNode* m_nodeList;
	SP_ListNode::const_iterator m_nodeIt;

	SP_MDI_View* m_pcView;

	wxString m_sNetClassName;

	wxCheckBox *m_cbSqueeze;
	wxCheckBox *m_cbDuplicate;
	wxCheckBox *m_cbSyntax;
	wxCheckBox *m_cbIntegrity;
	wxCheckBox *m_cbPosition;

public:
	SP_DLG_CheckNet(SP_MDI_View* p_pcView,wxWindow *p_parent,
			const wxString& p_title = wxT("Check a net"));
	virtual ~SP_DLG_CheckNet();

	void OnDlgClose(wxCommandEvent& p_cEvent);

	void OnCKBoxChange(wxCommandEvent& p_cEvent);

	void CheckDuplicateNodes();

	void OnDlgOk(wxCommandEvent& p_cEvent);

	bool Check();

	bool CheckRateFunction(wxString p_sFormula,SP_DS_Node* p_pcNode );

	bool CheckSPN();

	void Squeeze();

};

#endif // __SP_DLG_CheckNet_H__
