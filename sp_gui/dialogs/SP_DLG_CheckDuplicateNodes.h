//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2008/11/17 $
// Short Description: dialog to sort nodes by attribute
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_CHECKDUPLICATENODES_H__
#define __SP_DLG_CHECKDUPLICATENODES_H__

#include <wx/wx.h>
#include "sp_ds/SP_DS_Graph.h"

class SP_DLG_CheckDuplicateNodes: public wxDialog, public SP_Error
{
private:
DECLARE_EVENT_TABLE()
DECLARE_CLASS(SP_DLG_CheckDuplicateNodes)
protected:
	SP_DS_Graph* m_graph;
	SP_ListNode* m_nodeList;
	SP_ListNode::const_iterator m_nodeIt;

public:
	SP_DLG_CheckDuplicateNodes(SP_DS_Graph *p_graph, wxWindow *p_parent,
			const wxString& p_title = wxT("Check Duplicate Nodes"));
	virtual ~SP_DLG_CheckDuplicateNodes();

	void OnDlgClose(wxCommandEvent& p_cEvent);

};

#endif // __SP_DLG_CHECKDUBLICATENODES_H__
