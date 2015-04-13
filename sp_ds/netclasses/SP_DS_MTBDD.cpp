//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: r2 $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/01/22 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/netclasses/SP_DS_MTBDD.h"
#include "sp_ds/netclasses/SP_DS_SimpleGraph.h"
//TODO: check if all includes are necessary
#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_NumberAttribute.h"

#include "sp_ds/SP_DS_Graph.h"

#include "sp_gr/shapes/SP_GR_Ellipse.h"
#include "sp_gr/shapes/SP_GR_Rectangle.h"
#include "sp_gr/edges/SP_GR_ArrowEdge.h"
#include "sp_gr/edges/SP_GR_ExtendedEdge.h"
#include "sp_gr/shapes/SP_GR_NamedRectangle.h"
#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gr/attributes/SP_GR_CommentAttribute.h"
#include "sp_gr/attributes/SP_GR_MarkAttribute.h"



#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogBool.h"

#include "sp_defines.h"
#include "snoopy.h"
#include "sp_core/SP_GPR_Elements.h"

SP_DS_MTBDD::SP_DS_MTBDD()
:SP_DS_SimpleGraph(SP_DS_MTBDD_CLASS)
{
    m_sExtension = wxT("spmtbdd");
	m_sFilter = wxT("*.") + m_sExtension;
	//register node classes for editing their graphic properties
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Inner Node"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Terminal Node"));

}

SP_DS_MTBDD::SP_DS_MTBDD(const wxString& p_pchName)
:SP_DS_SimpleGraph(p_pchName)
{
    m_sExtension = wxT("spmtbdd");
	m_sFilter = wxT("*.") + m_sExtension;
	//register node classes for editing their graphic properties
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Inner Node"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Terminal Node"));

}

SP_DS_Graph*
SP_DS_MTBDD::CreateGraph(SP_DS_Graph* p_pcGraph)
{
    if (!SP_DS_SimpleGraph::CreateGraph(p_pcGraph))
        return NULL;

    SP_DS_Nodeclass* l_pcNC;
    SP_DS_Edgeclass* l_pcEC;
	SP_DS_Attribute* l_pcAttr;
    SP_Graphic* l_pcGrAttr;
	SP_GR_Node* l_pcGr;

	l_pcNC = p_pcGraph->RenameNodeclass(wxT("Node"), wxT("Inner Node"));
	l_pcNC->SetShortcut(wxT("N"));
	l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));


	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Variable"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetX(30);
    l_pcGrAttr->SetOffsetY(0);
    l_pcGrAttr->SetShow(TRUE);

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetY(0);

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NumberAttribute(wxT("IsRoot"), 0));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));
    l_pcAttr->SetGlobalShow();
    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_MarkAttribute(l_pcAttr));
    l_pcGrAttr->SetOffsetY(0);


	//////////////////////////////////////////////////////////////////////////////
    l_pcNC = p_pcGraph->AddNodeclass(new SP_DS_Nodeclass(p_pcGraph, wxT("Terminal Node")));

	int twidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), wxT("Terminal Node"));
	int theight = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), wxT("Terminal Node"));

	l_pcAttr = l_pcNC->
		AddAttribute(new SP_DS_TextAttribute(wxT("Value"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));


	l_pcGr = new SP_GR_NamedRectangle(l_pcNC->GetPrototype(), wxT(" "), twidth, theight);
    l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
    l_pcNC->SetGraphic(l_pcGr);
	l_pcNC->SetShortcut(wxT("T"));
    l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	///////////////////////////////////////////////////////////////////////////////
 	l_pcEC = p_pcGraph->RenameEdgeclass(wxT("Edge"), wxT("High"));
	l_pcEC->SetShortcut(wxT("H"));
	l_pcEC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	///////////////////////////////////////////////////////////////////////////////
	l_pcEC = p_pcGraph->AddEdgeclass(new SP_DS_Edgeclass(p_pcGraph, wxT("Low")));
	l_pcEC->SetShortcut(wxT("L"));
    l_pcEC->SetGraphic(new SP_GR_ExtendedEdge(l_pcEC->GetPrototype(),SP_EXTENDED_TYPE_DASHED_EDGE,1,
								SP_EXTENDED_ARROW_TYPE_NONE_EDGE,10,SP_EXTENDED_ARROW_TYPE_ARROW_EDGE,10) );
	l_pcEC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));



    return p_pcGraph;
}



bool
SP_DS_MTBDD::EdgeRequirement(SP_DS_Edgeclass* p_pcClass,
								 SP_Data* p_pcSrc,
								 SP_Data* p_pcTrg)
{

    if (!SP_DS_Netclass::EdgeRequirement(p_pcClass, p_pcSrc, p_pcTrg))
        return FALSE;

    if (p_pcSrc->GetElementType() != SP_ELEMENT_NODE ||
        p_pcTrg->GetElementType() != SP_ELEMENT_NODE)
        return FALSE;

	if (p_pcSrc->GetClassName()== wxT("Comment"))
        return FALSE;
    if (p_pcTrg->GetClassName()== wxT("Comment"))
        return FALSE;

	if (p_pcSrc->GetClassName()== wxT("Terminal Node") ) {
      	SP_MESSAGEBOX(wxT("Terminal Nodes have no outgoing edge."),
			wxT("Error"), wxOK | wxICON_ERROR);
		return FALSE;
	}


	if (  p_pcSrc ==  p_pcTrg ){
	  	SP_MESSAGEBOX(wxT("Source und Target must not be identical"),
			wxT("Error"), wxOK | wxICON_ERROR);
		return FALSE;

	}


SP_ListEdge::const_iterator eIt;
  const SP_ListEdge* edges;

    edges = (dynamic_cast<SP_DS_Node*>(p_pcSrc))->GetSourceEdges();

    //check if there are another arc of the same type between these nodes
    for (eIt = edges->begin(); eIt != edges->end(); ++eIt) {
		if ((*eIt)->GetClassName()== p_pcClass->GetName()) {

			SP_ListGraphic* sGraphic = (*eIt)->GetGraphics();
			SP_ListGraphic::iterator sIt;
	        sIt = sGraphic->begin();
			if (  NULL != (*sIt)->GetCanvas() ) {
				SP_MESSAGEBOX(wxString::Format(wxT("Only one outgoing edge of this type \"%s\" ")
				      wxT("is allowed from a inner node"),
				      (*eIt)->GetClassName().c_str()),
					  wxT("Error"), wxOK | wxICON_ERROR);
				return false;
			} //end if
		} // end if
    } // end if


	return TRUE;

}

bool
SP_DS_MTBDD::NodeRequirement(SP_DS_Node* p_pcNode)
{
    CHECK_POINTER(p_pcNode, return FALSE);

    return TRUE;
}
