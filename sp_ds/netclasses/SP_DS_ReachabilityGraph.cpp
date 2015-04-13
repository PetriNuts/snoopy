//////////////////////////////////////////////////////////////////////
// $Source: $
// $Author: r2 $
// $Version: 0.0 $
// $Revision: 1.40 $
// $Date: 2003/09/12 11:55:00 $
// Short Description:
//////////////////////////////////////////////////////////////////////
#include "sp_ds/netclasses/SP_DS_ReachabilityGraph.h"
#include "sp_ds/netclasses/SP_DS_SimpleGraph.h"

#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_ListAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/SP_DS_Graph.h"
//added by MD
#include "sp_ds/attributes/SP_DS_IdAttribute.h"

#include "sp_gr/shapes/SP_GR_Rectangle.h"
#include "sp_gr/edges/SP_GR_ArrowEdge.h"
#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gr/attributes/SP_GR_CommentAttribute.h"
//added by MD
#include "sp_gr/attributes/SP_GR_NumberAttribute.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogList.h"
//added by MD
#include "sp_gui/widgets/dialogs/SP_WDG_DialogShowOnly.h"

#include "sp_defines.h"
#include "snoopy.h"
#include "sp_core/SP_GPR_Elements.h"

SP_DS_ReachabilityGraph::SP_DS_ReachabilityGraph()
:SP_DS_SimpleGraph(SP_DS_REACHABILITY_CLASS)
{
    m_sExtension = wxT("sprgraph");
	m_sFilter = wxT("*.") + m_sExtension;

	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Node"));
}

SP_DS_ReachabilityGraph::SP_DS_ReachabilityGraph(const wxString& p_pchName)
:SP_DS_SimpleGraph(p_pchName)
{
    m_sExtension = wxT("sprgraph");
	m_sFilter = wxT("*.") + m_sExtension;

	wxGetApp().GetElementPrefs()->RegisterNodeclass(GetName(), wxT("Node"));
}

SP_DS_Graph*
SP_DS_ReachabilityGraph::CreateGraph(SP_DS_Graph* p_pcGraph)
{
  if (!SP_DS_SimpleGraph::CreateGraph(p_pcGraph))
        return NULL;

  SP_DS_Nodeclass* l_pcNC;
  SP_DS_Edgeclass* l_pcEC;
	SP_DS_Attribute* l_pcAttr;
  SP_Graphic* l_pcGrAttr;
	SP_GR_Node* l_pcGr;

	l_pcNC = p_pcGraph->GetNodeclass(wxT("Node"));
	l_pcNC->SetShortcut(wxT("N"));

	int width = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), wxT("Node"));
	if (width <= 0) width = 100;
	int height = wxGetApp().GetElementPrefs()->GetNodeHeight(GetName(), wxT("Node"));
	if (height <= 0) height = 30;

	l_pcGr = new SP_GR_Rectangle(l_pcNC->GetPrototype(), width, height, height/2);
    l_pcGr->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), l_pcNC->GetName()));
  l_pcNC->SetGraphic(l_pcGr);

	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_NameAttribute(wxT("Name"), wxT("")));
  l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
  l_pcAttr->SetGlobalShow();
  l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
  l_pcGrAttr->SetOffsetX(25);
  l_pcGrAttr->SetOffsetY(8);
  l_pcGrAttr->SetShow(FALSE);
	//added by MD
	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_IdAttribute(wxT("ID")));
  l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogShowOnly(wxT("General")));
  l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_NumberAttribute(l_pcAttr, wxT("_S%_")));
  l_pcGrAttr->SetShow(FALSE);
  l_pcAttr->SetGlobalShow();
	//------------------------------------------------------------------------------
	//changed by MD
	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_ListAttribute(wxT("Places"), wxT(""), false));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogList(wxT("General")));
	//------------------------------------------------------------------------------
  l_pcAttr->SetGlobalShow();
  l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));

	l_pcNC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
	l_pcAttr = l_pcNC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
  l_pcAttr->SetGlobalShow();
  l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
  l_pcGrAttr->SetOffsetY(0);


    ///////////////////////////////////////////////////////////////////////////////
	l_pcEC = p_pcGraph->GetEdgeclass(wxT("Edge"));
	l_pcEC->SetShortcut(wxT("E"));
	l_pcEC->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));
	l_pcAttr = l_pcEC->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"),wxT("")));
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	l_pcAttr->SetGlobalShow();
	l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
	l_pcGrAttr->SetOffsetX(40);
	//changed by MD
	l_pcAttr = l_pcEC->AddAttribute(new SP_DS_TextAttribute(wxT("Transition"), wxT("")));
	//-----------------------------------------------------------------------------
	l_pcAttr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	l_pcAttr->SetGlobalShow();
  l_pcGrAttr = l_pcAttr->AddGraphic(new SP_GR_TextAttribute(l_pcAttr));
  l_pcGrAttr->SetOffsetX(25);
  l_pcGrAttr->SetOffsetY(8);
  l_pcGrAttr->SetShow(TRUE);



  return p_pcGraph;
}



bool
SP_DS_ReachabilityGraph::EdgeRequirement(SP_DS_Edgeclass* p_pcClass,
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

	return TRUE;
}
