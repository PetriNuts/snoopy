//////////////////////////////////////////////////////////////////////
// $Author: alextov $
// $Version: 0.1 $
// $Date: 2005/06/22 $
// Short Description: PT nets with extensions
//////////////////////////////////////////////////////////////////////

#include "sp_ds/SP_DS_Graph.h"

#include "sp_ds/netclasses/SP_DS_ExtPT.h"

#include "sp_ds/attributes/SP_DS_BoolAttribute.h"
#include "sp_ds/attributes/SP_DS_MultiplicityAttribute.h"
#include "sp_ds/attributes/SP_DS_TextAttribute.h"
#include "sp_ds/attributes/SP_DS_MarkingDependentMultiplicity.h"


#include "sp_ds/extensions/SP_DS_Coarse.h"

#include "sp_gr/shapes/SP_GR_ExtendedCircle.h"
#include "sp_gr/edges/SP_GR_ArrowEdge.h"
#include "sp_gr/attributes/SP_GR_TextAttribute.h"
#include "sp_gr/attributes/SP_GR_MultiplicityAttribute.h"

#include "sp_gui/widgets/dialogs/SP_WDG_DialogBool.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogUnsigned.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogText.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogGraphic.h"
#include "sp_gui/widgets/dialogs/SP_WDG_DialogMultiline.h"

#include "sp_ds/animators/SP_DS_ExtPTPlaceAnimator.h"
#include "sp_ds/animators/SP_DS_ExtPTTransAnimator.h"

#include "snoopy.h"
#include "sp_core/SP_GPR_Elements.h"
#include "sp_ds/attributes/SP_DS_NodeTypeAttribute.h"//Added by george assaf
#include "sp_gui/widgets/dialogs/SP_WDG_DialogChoice.h"

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
SP_DS_ExtPT::CreateGraph(SP_DS_Graph* p_graph,SP_MapString2Int p_mapAttribute2Value )
{
	if (!SP_DS_SimplePed::CreateGraph(p_graph))
		return NULL;

	SP_DS_Nodeclass* nc;
	SP_DS_Edgeclass* ec;
	SP_DS_Attribute* attr;
	SP_Graphic* gr;
    SP_GR_Node* gn;

	//////////////////////////////////////////////////////////////////////////////
	nc = p_graph->GetNodeclass(wxT("Place"));
	 //adjust positioning offset of name graphic att of  Place
		for(auto itMap=p_mapAttribute2Value.begin();itMap!= p_mapAttribute2Value.end();++itMap)
		{
			wxString l_sNetClass = (itMap)->first.BeforeFirst('|');
			wxString l_sGraphicAttribute= (itMap)->first.AfterLast(wxChar('|'));
			wxString l_sNodeType = (itMap)->first.AfterFirst(wxChar('|'));
			l_sNodeType = l_sNodeType.BeforeFirst(wxT('|'));

			if(l_sNetClass.IsSameAs(SP_DS_EXTPN_CLASS) && l_sNodeType.IsSameAs(wxT("Place")))
			 {
			   if(!nc) break;

			   SP_ListGraphic* pc_ListAtt= nc->GetPrototype()->GetAttribute(wxT("Name"))->GetGraphics();

			  if(!pc_ListAtt) break;

				for(auto it=pc_ListAtt->begin();it!=pc_ListAtt->end();++it)
				{
					if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosX")))
					{
						(*it)->SetOffsetX((itMap)->second);
						//wxString l_slog;
						//l_slog<<wxT("offset on x of  Place is")<<(itMap)->second;
						//SP_LOGMESSAGE(l_slog);

					}
					else if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosY")))
					{
						(*it)->SetOffsetY((itMap)->second);
						//wxString l_slog;
						// l_slog<<wxT("offset on y of  Place is")<<(itMap)->second;
						//SP_LOGMESSAGE(l_slog);
					}

				}
				}
		}


	///////////////////////
	attr = nc->AddAttribute(new SP_DS_BoolAttribute(wxT("Fixed"), FALSE));//Fixed
	attr->RegisterDialogWidget(new SP_WDG_DialogBool(wxT("General")));

    int pwidth = wxGetApp().GetElementPrefs()->GetNodeWidth(GetName(), SP_DS_DISCRETE_PLACE);

    gn = new SP_GR_ExtendedCircle(nc->GetPrototype(), pwidth);
    gn->SetFixedSize(wxGetApp().GetElementPrefs()->GetNodeFixed(GetName(), nc->GetName()));
    nc->SetGraphic(gn);

	nc->AddAnimator(new SP_DS_ExtPTPlaceAnimator(wxT("Marking")));

	attr = nc->AddAttribute(new SP_DS_NodeTypeAttribute(wxT("Node Type")));//by george
	attr->RegisterDialogWidget(new SP_WDG_DialogChoice(wxT("General")));

	nc = p_graph->GetNodeclass(wxT("Transition"));
	//adjust positioning offset of name graphic att of  Transition
	for(auto itMap=p_mapAttribute2Value.begin();itMap!= p_mapAttribute2Value.end();++itMap)
	{
		wxString l_sNetClass = (itMap)->first.BeforeFirst('|');
		wxString l_sGraphicAttribute= (itMap)->first.AfterLast(wxChar('|'));
		wxString l_sNodeType = (itMap)->first.AfterFirst(wxChar('|'));
		l_sNodeType = l_sNodeType.BeforeFirst(wxT('|'));

		if(l_sNetClass.IsSameAs(SP_DS_EXTPN_CLASS) && l_sNodeType.IsSameAs(wxT("Transition")))
		 {
		   if(!nc) break;

		   SP_ListGraphic* pc_ListAtt= nc->GetPrototype()->GetAttribute(wxT("Name"))->GetGraphics();

		  if(!pc_ListAtt) break;

			for(auto it=pc_ListAtt->begin();it!=pc_ListAtt->end();++it)
			{
				if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosX")))
				{
					(*it)->SetOffsetX((itMap)->second);
					//wxString l_slog;
					//l_slog<<wxT("offset on x of  Transition is")<<(itMap)->second;
					//SP_LOGMESSAGE(l_slog);

				}
				else if(l_sGraphicAttribute.IsSameAs(wxT("NameAttPosY")))
				{
					(*it)->SetOffsetY((itMap)->second);
					//wxString l_slog;
					// l_slog<<wxT("offset on y of  Transition is")<<(itMap)->second;
					//SP_LOGMESSAGE(l_slog);
				}

			}
			}
	}


		///////////////////////
	nc->AddAnimator(new SP_DS_ExtPTTransAnimator());

	attr = nc->AddAttribute(new SP_DS_NodeTypeAttribute(wxT("Node Type")));//by george
	attr->RegisterDialogWidget(new SP_WDG_DialogChoice(wxT("General")));
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
    ec->SetDisplayName(wxT("Read Arc"));
    ec->SetAbbreviation(wxT("RA"));
    ec->SetShortcut(wxT("R"));
	ec->SetGraphic(new SP_GR_ArrowEdge(ec->GetPrototype(), ARROW_FILLED_CIRCLE, ARROW_POSITION_END, 7));
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
    ec->SetDisplayName(wxT("Inhibitor Arc"));
    ec->SetAbbreviation(wxT("IA"));
    ec->SetShortcut(wxT("I"));
	ec->SetGraphic(new SP_GR_ArrowEdge(ec->GetPrototype(), ARROW_HOLLOW_CIRCLE, ARROW_POSITION_END, 7));
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
    ec->SetDisplayName(wxT("Reset Arc"));
    ec->SetAbbreviation(wxT("ZA"));
    ec->SetShortcut(wxT("Z"));

	SP_GR_ArrowEdge * ae = new SP_GR_ArrowEdge(ec->GetPrototype(), ARROW_ARROW, ARROW_POSITION_END, 7);
	ae->AddAnotherArrow(ARROW_ARROW, ARROW_POSITION_END, 7);
	ec->SetGraphic(ae);

	ec->RegisterGraphicWidget(new SP_WDG_DialogGraphic(wxT("Graphic")));

	attr = ec->AddAttribute(new SP_DS_TextAttribute(wxT("Comment"), wxT("")));
	attr->RegisterDialogWidget(new SP_WDG_DialogMultiline(wxT("General")));
	attr->SetGlobalShow();
	gr = attr->AddGraphic(new SP_GR_TextAttribute(attr));
	gr->SetOffsetX(40);

	//////////////////////////////////////////////////////////////////////////////
	ec = p_graph->AddEdgeclass(new SP_DS_Edgeclass(p_graph, wxT("Equal Edge")));
    ec->SetDisplayName(wxT("Equal Arc"));
    ec->SetAbbreviation(wxT("EA"));
    ec->SetShortcut(wxT("Q"));

	ae = new SP_GR_ArrowEdge(ec->GetPrototype(), ARROW_FILLED_CIRCLE, ARROW_POSITION_END, 7);
	ae->AddAnotherArrow(ARROW_FILLED_CIRCLE, ARROW_POSITION_END, 7);
	ec->SetGraphic(ae);

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

