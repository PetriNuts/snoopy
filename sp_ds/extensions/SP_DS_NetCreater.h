//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: schroed $
// $Version: 0.0 $
// $Revision: 0.0 $
// $Date: 2007/01/08 07:30:00 $
// Short Description: create a net
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_NETCREATER_H__
#define __SP_DS_NETCREATER_H__


#include <wx/wx.h>
#include "snoopy.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"



	/**
	\brief Make function available which will be used to create a fault tree
	*/

class SP_DS_NetCreater
{
private:
	typedef std::pair<SP_DS_Node*, SP_DS_Node*> nodes_t;
	typedef std::pair<SP_DS_Edgeclass*, nodes_t> edge_t;
	typedef std::map<edge_t, SP_DS_Edge*> SP_MapEdges;

	SP_MapEdges m_Edges;

protected:

	/**
	\brief Creates a new document

	\returns	l_pcGraph	new fault tree
	*/
	SP_DS_Graph* CreateDocument(const wxString& netName);

	/**
	\brief Draws a new edge

	\param	source		source node of the edge
	\param	target		target node of the edge
	*/
	SP_DS_Edge* drawEdge(SP_DS_Node* source, SP_DS_Node* target, const wxString& EdgeClass, const wxString& stoichiometry);


	SP_DS_Edge* drawDpnEdge(SP_DS_Node* source, SP_DS_Node* target, const wxString& EdgeClass, const wxString& arcweight);

	/**
	\brief Update the frame

	\param	l_doc		document
	*/
	void RefreshFrame(SP_MDI_Doc* l_doc);

	/**
	\brief Starts visualisation
	*/
	void DoVisualization();



public:

    SP_DS_NetCreater();

    ~SP_DS_NetCreater();

	//new graph
	SP_DS_Graph* m_pcNewGraph;
	SP_GUI_Canvas* m_pcCanvas;
	SP_MDI_Doc* m_pcMyDoc;

	const wxString m_Layout;
	const wxString m_Type;
};

#endif // __SP_DS_NETCREATER_H__


