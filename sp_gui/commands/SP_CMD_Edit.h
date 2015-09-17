//////////////////////////////////////////////////////////////////////
// $Source: SP_CMD_Edit.h $
// $Author: MD $
// $Version: 0.0 $
// $Revision: 1.10 $
// $Date: 2006/09/29 $
// Short Description: this class (derived from wxCommand) combines edit capabilities
//										such as copy, cut, delete and paste
//////////////////////////////////////////////////////////////////////
// NOTE:	so far there is only the DELETE command implemented.
//				all other commands still need to be integrated into the app,
//				which would then provide undo-capability for them
//////////////////////////////////////////////////////////////////////

#if !defined(__SP_CMD_EDIT_H__)
#define __SP_CMD_EDIT_H__

#include "sp_core/SP_List.h"
#include "sp_core/base/SP_Graphic.h"

#include <wx/wx.h>
#include <wx/cmdproc.h>

// command id's
enum {
	SP_CMD_COPY = 1,
	SP_CMD_CUT,
	SP_CMD_PASTE,
	SP_CMD_DELETE,
	SP_CMD_DELETE_ALL,
	SP_CMD_HIDE,
	SP_CMD_HIDE_ALL,
	SP_CMD_UNHIDE,
	SP_CMD_UNHIDE_ALL,
};


class SP_CMD_Edit : public wxCommand {



private:
	//m_pcSource: the original graph, m_pcDelGraph: the graph were we move deleted data to
	SP_DS_Graph *m_pcSource, *m_pcDelGraph;
	//holds references to all coarse nodes which have been deleted
	list<SP_DS_Coarse*> m_lCoarseNodes;
	//holds "coarse graphics" which are removed from the graph and whose parent belongs to the upper net
	//we need the association to their parent so we can re-add them in Undo()
	map<SP_Graphic*, SP_Data*> m_mGraphic2Parent;

	//next functions are used for SP_CMD_DELETE
	//this takes care of possibly loose edges after deleting nodes
	//if the second param is passed, only graphics connected to it are checked
	void AppendEdges(const SP_ListEdge *p_pcEdges, SP_Graphic *p_pcGrNode = 0);
	//this takes care of subelements within coarse nodes
	void AppendCoarse(SP_DS_Coarse *p_pcCoarse);
	//used by AppendToDeleteList(...) to check for already deleted edges
	bool EdgeInGraph(SP_DS_Edge *p_pcEdge);
	//these remove deleted graphics from canvas
	void RemoveGraphicsFromCanvas(SP_Data *p_pcData);
	void RemoveGraphicFromCanvas(SP_Graphic *p_pcGraphic);
	//this function removes coarse nodes from coarse tree and deletes their documents
	void CleanUpCoarseNodes();

	void AppendHiddenEdges(SP_Graphic *p_pcGrNode);
	void AppendHiddenEdges(SP_Data *p_pcNode);

protected:
	//list of graphics to delete
	SP_ListGraphic m_lcGraphics;
	//the command's id
	int m_nCmdId;

	//self-explanatory
	bool DoDelete(bool p_bDeleteAll = false);
	bool UndoDelete();
	bool DoHide(bool p_bHideAll = false);
	bool UndoHide();
	bool DoUnHide(bool p_bUnHideAll = false);
	bool UndoUnHide();

public:
	SP_CMD_Edit(int p_nCmdId, SP_ListGraphic *p_graphics, const wxString& name = wxT("Edit Command <Default>"));
	SP_CMD_Edit(int p_nCmdId, SP_Graphic *p_graphic, const wxString& name = wxT("Edit Command <Default>"));
	virtual ~SP_CMD_Edit();

	//implementing the wxCommand interface
	bool Do();
	bool Undo();

};

#endif // !defined(__SP_CMD_EDIT_H__)
