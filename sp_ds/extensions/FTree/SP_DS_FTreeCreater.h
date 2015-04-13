//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: akurth $
// $Version: 0.0 $
// $Revision: 0.0 $
// $Date: 2007/01/08 07:30:00 $
// Short Description: create a fault tree
//////////////////////////////////////////////////////////////////////
#ifndef __SP_DS_FTREECREATER_H__
#define __SP_DS_FTREECREATER_H__


#include <wx/wx.h>
#include "snoopy.h"
#include "sp_gui/windows/SP_GUI_Childframe.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_gui/management/SP_GM_Docmanager.h"
#include "sp_gui/management/SP_GM_DocTemplate.h"



	/**
	\brief Make function available which will be used to create a fault tree
	*/

class SP_DS_FTreeCreater
{
private:

protected:

	/**
	\brief Creates a new document 

	\returns	l_pcGraph	new fault tree
	*/
	SP_DS_Graph* CreateDocument();

	/**
	\brief Draws a new edge
	
	\param	source		source node of the edge
	\param	target		target node of the edge
	*/
	void drawEdge(SP_DS_Node* source, SP_DS_Node* target);

	/**
	\brief Update the frame
	
	\param	l_doc		document
	*/
	void RefreshFrame(SP_MDI_Doc* l_doc);

	/**
	\brief Starts visualisation of the fault tree
	The graph is represented as a tree
	*/
	void DoVisualization();

	/**
	\brief Returns parameters to represent the graph as a tree
	
	\param	m_lAttributes	parameters
	*/
	SP_MapString2String GetAttributes();

	/**
	\brief Copy of attributes of a node
	
	\param	sourceNode	node
	\param	copyNode	node to receives the attributes
	*/
	void CopyAttributes(SP_DS_Node* sourceNode, SP_DS_Node* copyNode);



public:
    SP_DS_FTreeCreater();

    ~SP_DS_FTreeCreater();

	const wxString m_Layout;
	const wxString m_Type;

	//new graph
	SP_DS_Graph* m_pcNewGraph;
	SP_GUI_Canvas* m_pcCanvas;
	SP_MDI_Doc* m_pcMyDoc;

};

#endif // __SP_DS_FTREECREATER_H__


