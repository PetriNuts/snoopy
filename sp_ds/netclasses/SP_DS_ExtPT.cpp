//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/06/22 $
// Short Description: PT nets with extensions
//////////////////////////////////////////////////////////////////////

#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/netclasses/SP_DS_ExtPT.h"

#include "sp_ds/SP_DS_Edgeclass.h"
#include "sp_ds/SP_DS_Nodeclass.h"

#include "sp_ds/attributes/SP_DS_NameAttribute.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingDependentMultiplicity.h"


#include "sp_ds/extensions/SP_DS_Coarse.h"

#include "sp_gr/edges/SP_GR_ArrowEdge.h"
#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gr/attributes/SP_GR_MultiplicityAttribute.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogUnsigned.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogText.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"

#include "sp_ds/animators/SP_DS_ExtPTPlaceAnimator.h"
#include "sp_ds/animators/SP_DS_ExtPTTransAnimator.h"


SP_DS_ExtPT::SP_DS_ExtPT() :
	SP_DS_SimplePed(SP_DS_EXTPN_CLASS)
{
	m_sExtension = wxT("xpn");
	m_sFilter = wxT("*.xpn;*.spept");
}

SP_DS_ExtPT::SP_DS_ExtPT(const wxString& p_pchName) :
	SP_DS_SimplePed(p_pchName)
{
	m_sExtension = wxT("xpn");
	m_sFilter = wxT("*.xpn;*.spept");
}

SP_DS_Graph*
SP_DS_ExtPT::CreateGraph(SP_DS_Graph* p_graph)
{
	if (!SP_DS_SimplePed::CreateGraph(p_graph))
		return NULL;

	SP_DS_Nodeclass* nc;
	SP_DS_Edgeclass* ec;
	SP_DS_Attribute* attr;
	SP_Graphic* gr;

	//////////////////////////////////////////////////////////////////////////////
	nc = p_graph->GetNodeclass(wxT("Place"));
	nc->AddAnimator(new SP_DS_ExtPTPlaceAnimator(wxT("Marking")));

	nc = p_graph->GetNodeclass(wxT("Transition"));
	nc->AddAnimator(new SP_DS_ExtPTTransAnimator());
	//////////////////////////////////////////////////////////////////////////////





	///////////////////////////////////////////////////////////////////////////////
	/*
	 * remove and add new attributes for normal edge
	 */
	ec = p_graph->GetEdgeclass(wxT("Edge"));

	attr = ec->GetPrototype()->GetAttribute(wxT("Multiplicity"));
	if(attr->Remove()) wxDELETE(attr);

	attr=ec->AddAttribute(new SP_DS_MarkingDependentMultiplicity(wxT("Multiplicity"),wxT("1")));
	attr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	attr->SetGlobalShow();

	gr = attr->AddGraphic(new SP_GR_MultiplicityAttribute(attr));
	gr->SetOffsetX(20);
	gr->SetShow(TRUE);

	//////////////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////////////
	ec = p_graph->AddEdgeclass(new SP_DS_Edgeclass(p_graph, wxT("Read Edge")));
	ec->SetGraphic(new SP_GR_ArrowEdge(ec->GetPrototype(), ARROW_FILLED_CIRCLE, ARROW_POSITION_END, 7));
	ec->SetShortcut(wxT("R"));
	ec->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	attr=ec->AddAttribute(new SP_DS_MarkingDependentMultiplicity(wxT("Multiplicity"),wxT("1")));
	attr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	attr->SetGlobalShow();
	gr = attr->AddGraphic(new SP_GR_MultiplicityAttribute(attr));
	gr->SetOffsetX(20);
	gr->SetShow(TRUE);

	attr = ec->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	attr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	attr->SetGlobalShow();
	gr = attr->AddGraphic(new SP_GR_TextAttribute(attr));
	gr->SetOffsetX(40);

	//////////////////////////////////////////////////////////////////////////////
	ec = p_graph->AddEdgeclass(new SP_DS_Edgeclass(p_graph, wxT("Inhibitor Edge")));
	ec->SetGraphic(new SP_GR_ArrowEdge(ec->GetPrototype(), ARROW_HOLLOW_CIRCLE, ARROW_POSITION_END, 7));
	ec->SetShortcut(wxT("I"));
	ec->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	attr=ec->AddAttribute(new SP_DS_MarkingDependentMultiplicity(wxT("Multiplicity"),wxT("1")));
	attr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	attr->SetGlobalShow();
	gr = attr->AddGraphic(new SP_GR_MultiplicityAttribute(attr));
	gr->SetOffsetX(20);
	gr->SetShow(TRUE);

	attr = ec->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	attr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	attr->SetGlobalShow();
	gr = attr->AddGraphic(new SP_GR_TextAttribute(attr));
	gr->SetOffsetX(40);

	//////////////////////////////////////////////////////////////////////////////
	ec = p_graph->AddEdgeclass(new SP_DS_Edgeclass(p_graph, wxT("Reset Edge")));

	SP_GR_ArrowEdge * ae = new SP_GR_ArrowEdge(ec->GetPrototype(), ARROW_ARROW, ARROW_POSITION_END, 7);
	ae->AddAnotherArrow(ARROW_ARROW, ARROW_POSITION_END, 7);
	ec->SetGraphic(ae);

	ec->SetShortcut(wxT("Z"));
	ec->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	attr = ec->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	attr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	attr->SetGlobalShow();
	gr = attr->AddGraphic(new SP_GR_TextAttribute(attr));
	gr->SetOffsetX(40);

	//////////////////////////////////////////////////////////////////////////////
	ec = p_graph->AddEdgeclass(new SP_DS_Edgeclass(p_graph, wxT("Equal Edge")));

	ae = new SP_GR_ArrowEdge(ec->GetPrototype(), ARROW_FILLED_CIRCLE, ARROW_POSITION_END, 7);
	ae->AddAnotherArrow(ARROW_FILLED_CIRCLE, ARROW_POSITION_END, 7);
	ec->SetGraphic(ae);

	ec->SetShortcut(wxT("Q"));
	ec->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	attr=ec->AddAttribute(new SP_DS_MarkingDependentMultiplicity(wxT("Multiplicity"),wxT("1")));
	attr->RegisterDialogWidget(new SP_WDG_DialogText(wxT("General")));
	attr->SetGlobalShow();
	gr = attr->AddGraphic(new SP_GR_MultiplicityAttribute(attr));
	gr->SetOffsetX(20);
	gr->SetShow(TRUE);

	attr = ec->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	attr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	attr->SetGlobalShow();
	gr = attr->AddGraphic(new SP_GR_TextAttribute(attr));
	gr->SetOffsetX(40);
	//////////////////////////////////////////////////////////////////////////////

	return p_graph;
}

bool SP_DS_ExtPT::EdgeRequirement(SP_DS_Edgeclass* p_class, SP_Data* p_src, SP_Data* p_dst)
{
	SP_LOGDEBUG(wxString::Format(wxT("EdgeRequirement %p->%p %s"), p_src, p_dst, p_class->GetName().c_str()));

	if (!SP_DS_SimplePed::EdgeRequirement(p_class, p_src, p_dst))
		return false;

	SP_ListEdge::const_iterator eIt;
	const SP_ListEdge* edges;

	if (p_class->GetName() == wxT("Read Edge") ||
		p_class->GetName() == wxT("Inhibitor Edge") ||
		p_class->GetName() == wxT("Reset Edge") ||
		p_class->GetName() == wxT("Equal Edge"))
	{

		if (!p_src->GetClassName().Contains(wxT("Place")) &&
			!p_dst->GetClassName().Contains(wxT("Transition")))
		{
			SP_MESSAGEBOX(wxString(p_class->GetName()) + wxT(" must connect places and transitions"), wxT("Error"), wxOK | wxICON_ERROR);
			return false;
		}

		edges = (dynamic_cast<SP_DS_Node*> (p_src))->GetSourceEdges();

		//check if there are another arc of the same type between these nodes
		for (eIt = edges->begin(); eIt != edges->end(); ++eIt)
		{
			SP_LOGDEBUG(wxString::Format(wxT("%p->%p (%p), %s"), (*eIt)->GetSource(), (*eIt)->GetTarget(), p_dst, (*eIt)->GetClassName().c_str()));

			if ((*eIt)->GetTarget() == p_dst &&
				(((*eIt)->GetClassName() == wxT("Read Edge") && p_class->GetName() == wxT("Equal Edge")) ||
				((*eIt)->GetClassName() == wxT("Inhibitor Edge") && p_class->GetName() == wxT("Equal Edge")) ||
				((*eIt)->GetClassName() == wxT("Equal Edge") && p_class->GetName() == wxT("Read Edge")) ||
				((*eIt)->GetClassName() == wxT("Equal Edge") && p_class->GetName() == wxT("Inhibitor Edge"))))
			{
				SP_MESSAGEBOX(wxString::Format(wxT("Only one edge of the type \"%s\" ")
				wxT("is allowed between these nodes"), (*eIt)->GetClassName().c_str()), wxT("Error"), wxOK | wxICON_ERROR);
				return false;
			}
		}
	}

	return true;
}

