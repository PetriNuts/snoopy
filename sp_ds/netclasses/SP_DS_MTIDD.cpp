//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: r2 $
// $Version: 0.0 $
// $Revision: 1.30 $
// $Date: 2007/01/22 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/netclasses/SP_DS_MTIDD.h"
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
#include "sp_gr/shapes/SP_GR_NamedRectangle.h"
#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gr/attributes/SP_GR_CommentAttribute.h"
#include "sp_gr/attributes/SP_GR_MarkAttribute.h"



#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogBool.h"

#include "sp_utilities.h"

#include "snoopy.h"
#include "sp_core/SP_GPR_Elements.h"

SP_DS_MTIDD::SP_DS_MTIDD()
:SP_DS_SimpleGraph(SP_DS_MTIDD_CLASS)
{
    m_sExtension = wxT("spmtidd");
	m_sFilter = wxT("*.") + m_sExtension;
	//register node classes for editing their graphic properties
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Inner Node"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Terminal Node"));

}

SP_DS_MTIDD::SP_DS_MTIDD(const wxString& p_pchName)
:SP_DS_SimpleGraph(p_pchName)
{
    m_sExtension = wxT("spmtidd");
	m_sFilter = wxT("*.") + m_sExtension;
	//register node classes for editing their graphic properties
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Inner Node"));
	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Terminal Node"));

}

SP_DS_Graph*
SP_DS_MTIDD::CreateGraph(SP_DS_Graph* p_pcGraph,SP_MapString2Int p_mapAttributeValue )
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

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Value"), wxT("")));
    l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));


	l_pcGr = new SP_GR_NamedRectangle(l_pcNC->GetPrototype(), wxT(" "), twidth, theight);
    l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
    l_pcNC->SetGraphic(l_pcGr);
	l_pcNC->SetShortcut(wxT("T"));
    l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));


	///////////////////////////////////////////////////////////////////////////////

    l_pcEC = p_pcGraph->GetEdgeclass(wxT("Edge"));
	l_pcAttr = l_pcEC->AddAttribute(new SP_DS_TextAttribute( wxT("Interval"),wxT("0,0")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText( wxT("Interval") ));

    l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr,wxT("[%)")));

    l_pcEC->SetShortcut(wxT("E"));
	l_pcEC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	///////////////////////////////////////////////////////////////////////////////




    return p_pcGraph;
}



bool
SP_DS_MTIDD::EdgeRequirement(SP_DS_Edgeclass* p_pcClass,
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





	return TRUE;

}

bool
SP_DS_MTIDD::NodeRequirement(SP_DS_Node* p_pcNode)
{
    CHECK_POINTER(p_pcNode, return FALSE);

    return TRUE;
}

