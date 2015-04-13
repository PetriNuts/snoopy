//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2008/11/17 $
// Short Description: dialog to sort nodes by attribute
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_NETINFORMATIONS_H__
#define __SP_DLG_NETINFORMATIONS_H__

#include <wx/wx.h>
#include "sp_ds/SP_DS_Graph.h"

class SP_DLG_NetInformations: public wxDialog, public SP_Error
{
private:
DECLARE_EVENT_TABLE()
DECLARE_CLASS(SP_DLG_NetInformations)
protected:
	SP_DS_Graph* m_graph;
	const SP_ListNode* m_nodeList;
	SP_ListNode::const_iterator m_nodeIt;

public:
	SP_DLG_NetInformations(SP_DS_Graph *p_graph, wxWindow *p_parent,
			const wxString& p_title = wxT("Net Informations"));
	virtual ~SP_DLG_NetInformations();

	void OnDlgClose(wxCommandEvent& p_cEvent);

};

#endif // __SP_DLG_NETINFORMATIONS_H__
