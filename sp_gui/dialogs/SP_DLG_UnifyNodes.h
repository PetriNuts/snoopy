//////////////////////////////////////////////////////////////////////
// $Author: crohr $
// $Version: 0.1 $
// $Date: 2015/10/08 $
// Short Description: dialog to unify nodes
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DLG_SORTUNIFYNODES_H__
#define __SP_DLG_SORTUNIFYNODES_H__

#include "sp_gui/dialogs/SP_DLG_DivideNodes.h"

class SP_DLG_UnifyNodes: public SP_DLG_DivideNodes
{
private:
DECLARE_EVENT_TABLE()
DECLARE_CLASS(SP_DLG_UnifyNodes)
protected:
	virtual void LoadData(const wxString& p_sNodeClassName, int p_nNrArcs);
public:
	SP_DLG_UnifyNodes(SP_DS_Graph *p_graph, unsigned int p_netNumber, SP_ListGraphic* p_graphics, wxWindow *p_parent,
			const wxString& p_title = wxT("Unify Nodes"));
	virtual ~SP_DLG_UnifyNodes();
};

#endif // __SP_DLG_SORTUNIFYNODES_H__
