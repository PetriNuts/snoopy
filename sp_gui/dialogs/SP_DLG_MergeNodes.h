//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2015/10/08 $
// Short Description: dialog to merge nodes
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_MERGENODES_H__
#define __SP_DLG_MERGENODES_H__

#include "sp_gui/dialogs/SP_DLG_DuplicateNodes.h"

class SP_DLG_MergeNodes: public SP_DLG_DuplicateNodes
{
private:
DECLARE_EVENT_TABLE()
DECLARE_CLASS(SP_DLG_MergeNodes)
protected:
	virtual void LoadData(const wxString& p_sNodeClassName, int p_nCount);
public:
	SP_DLG_MergeNodes(SP_DS_Graph *p_graph, unsigned int p_netNumber, SP_ListGraphic* p_graphics, wxWindow *p_parent,
			const wxString& p_title = wxT("Merge Nodes"));
	virtual ~SP_DLG_MergeNodes();
};

#endif // __SP_DLG_MERGENODES_H__
