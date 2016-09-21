//////////////////////////////////////////////////////////////////////
// $Author: CR $
// $Version: 0.1 $
// $Date: 2013/05/21 $
// Short Description: export to andl
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportColPN2ANDL.h"
#include "sp_utilities.h"


#include "sp_ds/SP_DS_Edge.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"


#include <wx/tokenzr.h>

SP_ExportColPN2ANDL::SP_ExportColPN2ANDL()
{
}

SP_ExportColPN2ANDL::~SP_ExportColPN2ANDL()
{
}

bool SP_ExportColPN2ANDL::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_COLPN_CLASS ||
			cName == SP_DS_COLEPN_CLASS ||
			cName == SP_DS_COLSPN_CLASS ||
			cName == SP_DS_COLCPN_CLASS ||
			cName == SP_DS_COLHPN_CLASS);
}

bool SP_ExportColPN2ANDL::DoWrite()
{
	m_sNetClass = m_graph->GetNetclass()->GetName();

	std::shared_ptr<SP_DS_ColPN_Unfolding> l_pcUnfolding = std::shared_ptr<SP_DS_ColPN_Unfolding>(new SP_DS_ColPN_Unfolding());

	if( !l_pcUnfolding->Unfolding() )
		return false;	

	wxString l_sType = wxT("pn");
	if(m_doc->GetNetclassName() == SP_DS_COLEPN_CLASS)
	{
		l_sType = wxT("xpn");
	}
	else if(m_doc->GetNetclassName() == SP_DS_COLSPN_CLASS)
	{
		l_sType = wxT("spn");
		if(!m_graph->GetNodeclass(SP_DS_IMMEDIATE_TRANS)->GetElements()->empty())
		{
			l_sType = wxT("gspn");
		}
		if(!m_graph->GetNodeclass(SP_DS_DETERMINISTIC_TRANS)->GetElements()->empty()
			|| !m_graph->GetNodeclass(SP_DS_SCHEDULED_TRANS)->GetElements()->empty())
		{
			l_sType = wxT("xspn");
		}
	}
	else if(m_doc->GetNetclassName() == SP_DS_COLCPN_CLASS)
	{
		l_sType = wxT("cpn");
	}
	else if(m_doc->GetNetclassName() == SP_DS_COLHPN_CLASS)
	{
		l_sType = wxT("hpn");
	}
	m_file.Write(l_sType + wxT(" [") + GetNetName(m_fileName) + wxT("]\n") + wxT("{\n"));

	if( m_sNetClass ==  SP_DS_COLSPN_CLASS ||
		m_sNetClass ==  SP_DS_COLCPN_CLASS ||
		m_sNetClass ==  SP_DS_COLHPN_CLASS)
	{
		WriteParameters(l_pcUnfolding);
	}

	WritePlaces(l_pcUnfolding);
	WriteTransitions(l_pcUnfolding);



	m_file.Write(wxT("}\n"));

	return true;
}

bool SP_ExportColPN2ANDL::WritePlaces(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding)
{
	m_file.Write(wxT("places:\n"));

	if( m_sNetClass == SP_DS_COLPN_CLASS ||
		m_sNetClass == SP_DS_COLEPN_CLASS ||
		m_sNetClass == SP_DS_COLSPN_CLASS )
	{
		if(!p_Unfolding->GetUnfoldedDiscPlaces().empty())
		{
			//m_file.Write(wxT("  discrete:\n"));
			WritePlaceClass(p_Unfolding, p_Unfolding->GetUnfoldedDiscPlaces());
		}
	}
	else if( m_sNetClass == SP_DS_COLCPN_CLASS )
	{
		if(!p_Unfolding->GetUnfoldedContPlaces().empty())
		{
			//m_file.Write(wxT("  continous:\n"));
			WritePlaceClass(p_Unfolding, p_Unfolding->GetUnfoldedContPlaces());
		}
	}
	else if( m_sNetClass == SP_DS_COLHPN_CLASS )
	{
		if(!p_Unfolding->GetUnfoldedDiscPlaces().empty())
		{
			m_file.Write(wxT("  discrete:\n"));
			WritePlaceClass(p_Unfolding, p_Unfolding->GetUnfoldedDiscPlaces());
		}
		if(!p_Unfolding->GetUnfoldedContPlaces().empty())
		{
			m_file.Write(wxT("  continous:\n"));
			WritePlaceClass(p_Unfolding, p_Unfolding->GetUnfoldedContPlaces());
		}
	}

	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportColPN2ANDL::WritePlaceClass(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding,
							const SP_CPN_UnfoldedPlaces& p_pmmUnfoldedPlaces)
{
	SP_DS_ColListAttribute* l_pcColList = NULL;
	unsigned l_nCurrentColumn =	0;
	SP_CPN_UnfoldedPlaces::const_iterator itMap1;
	for(itMap1 = p_pmmUnfoldedPlaces.begin(); itMap1 != p_pmmUnfoldedPlaces.end(); itMap1++)
	{
		wxString l_sColPlName = itMap1->first;
		SP_CPN_UnfoldedPlace::const_iterator itMap2;
		for(itMap2 = itMap1->second.begin(); itMap2 != itMap1->second.end(); itMap2++ )
		{
			wxString l_sColor = itMap2->first;
			wxString l_sNodeType = itMap2->second.m_sNodeType;

			if(!l_pcColList)
			{
				l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(m_graph->GetNodeclass(l_sNodeType)->GetElements()->begin()))->GetAttribute(wxT("MarkingList")));
				l_nCurrentColumn = l_pcColList->GetActiveColumn();
			}

			p_Unfolding->ModifyName(l_sColor);

			wxString l_sBuffer = wxT("    ") + l_sColPlName + wxT("_") + l_sColor;

			if(l_sNodeType == SP_DS_DISCRETE_PLACE)
			{
				l_sBuffer << wxT(" = ") << itMap2->second.m_anNetMarkings[l_nCurrentColumn];
			}
			if(l_sNodeType == SP_DS_CONTINUOUS_PLACE)
			{
				l_sBuffer << wxT(" = ") << itMap2->second.m_adNetMarkings[l_nCurrentColumn];
			}
			l_sBuffer << wxT(";\n");
			m_file.Write( l_sBuffer );
		}
	}

	return true;
}

bool SP_ExportColPN2ANDL::WriteTransitions(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding)
{
	m_file.Write(wxT("transitions:\n"));

	if( m_sNetClass == SP_DS_COLPN_CLASS ||
		m_sNetClass == SP_DS_COLEPN_CLASS )
	{
		WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedStochTransions());
	}
	else if( m_sNetClass == SP_DS_COLSPN_CLASS )
	{
		if(!p_Unfolding->GetUnfoldedStochTransions().empty())
		{
			m_file.Write(wxT("  stochastic:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedStochTransions());
		}
		if(!p_Unfolding->GetUnfoldedImmTransions().empty())
		{
			m_file.Write(wxT("  immediate:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedImmTransions());
		}
		if(!p_Unfolding->GetUnfoldedDetTransions().empty())
		{
			m_file.Write(wxT("  deterministic:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedDetTransions());
		}
		if(!p_Unfolding->GetUnfoldedSchedTransions().empty())
		{
			m_file.Write(wxT("  scheduled:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedSchedTransions());
		}
	}
	else if( m_sNetClass == SP_DS_COLCPN_CLASS )
	{
		if(!p_Unfolding->GetUnfoldedContTransions().empty())
		{
			m_file.Write(wxT("  continous:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedContTransions());
		}
	}
	else if( m_sNetClass == SP_DS_COLHPN_CLASS )
	{
		if(!p_Unfolding->GetUnfoldedStochTransions().empty())
		{
			m_file.Write(wxT("  stochastic:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedStochTransions());
		}
		if(!p_Unfolding->GetUnfoldedContTransions().empty())
		{
			m_file.Write(wxT("  continous:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedContTransions());
		}
		if(!p_Unfolding->GetUnfoldedImmTransions().empty())
		{
			m_file.Write(wxT("  immediate:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedImmTransions());
		}
		if(!p_Unfolding->GetUnfoldedDetTransions().empty())
		{
			m_file.Write(wxT("  deterministic:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedDetTransions());
		}
		if(!p_Unfolding->GetUnfoldedSchedTransions().empty())
		{
			m_file.Write(wxT("  scheduled:\n"));
			WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedSchedTransions());
		}
	}

	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportColPN2ANDL::WriteTransitionClass(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding,
							const SP_CPN_UnfoldedTransitions& p_mmUnfoldedTransions)
{
	SP_DS_ColListAttribute* l_pcColList = NULL;
	unsigned l_nCurrentColumn =	0;
	SP_CPN_UnfoldedTransitions::const_iterator itMap1;
	for(itMap1 = p_mmUnfoldedTransions.begin(); itMap1 != p_mmUnfoldedTransions.end(); itMap1++)
	{
		wxString l_sColTRName = itMap1->first;
		SP_CPN_UnfoldedTransition::const_iterator itMap2;

		for(itMap2 = itMap1->second.begin(); itMap2 != itMap1->second.end(); itMap2++ )
		{
			wxString l_sBinding = itMap2->first;
			wxString l_NodeType = itMap2->second.m_sType;
			wxString l_sAnimName = itMap2->second.m_sAnimTransInstName;

			p_Unfolding->ModifyName(l_sBinding);

			wxString l_sBuffer = wxT("    ") + l_sColTRName + wxT("_") + l_sBinding;

			wxString l_sFunction;
			if( m_sNetClass ==  SP_DS_COLSPN_CLASS ||
				m_sNetClass ==  SP_DS_COLCPN_CLASS ||
				m_sNetClass ==  SP_DS_COLHPN_CLASS)
			{
				if(!l_pcColList)
				{
					if( l_NodeType == SP_DS_CONTINUOUS_TRANS ||
						l_NodeType == SP_DS_STOCHASTIC_TRANS ||
						l_NodeType == SP_DS_IMMEDIATE_TRANS)
					{
						l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(m_graph->GetNodeclass(l_NodeType)->GetElements()->begin()))->GetAttribute(wxT("FunctionList")));
					}
					else if( l_NodeType == SP_DS_DETERMINISTIC_TRANS)
					{
						l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(m_graph->GetNodeclass(l_NodeType)->GetElements()->begin()))->GetAttribute(wxT("DelayList")));
					}
					else if( l_NodeType == SP_DS_SCHEDULED_TRANS)
					{
						l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(m_graph->GetNodeclass(l_NodeType)->GetElements()->begin()))->GetAttribute(wxT("PeriodicList")));
					}
					l_nCurrentColumn = l_pcColList->GetActiveColumn();
				}
				l_sFunction << wxT("\n        : ") << itMap2->second.m_anNetFunctions[l_nCurrentColumn];
			}
			//write arcs
			// conditions && updates
			wxString l_sConditions;
			wxString l_sUpdates;
			//attention: output arcs first, because of reset arcs
			WriteArcs(p_Unfolding, itMap2->second.m_vOutputArcs,l_sConditions,l_sUpdates,false);
			WriteArcs(p_Unfolding, itMap2->second.m_vInputArcs,l_sConditions,l_sUpdates,true);

			l_sBuffer << wxT("\n        : ") << l_sConditions << wxT("\n        : ") << l_sUpdates << l_sFunction << wxT(";\n");
			m_file.Write(l_sBuffer);
		}
	}
	return TRUE;
}


bool SP_ExportColPN2ANDL::WriteArcs(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding,
							const vector<SP_CPN_UnfoldedArcInfo>& p_vArcs,
							wxString& p_sConditions, wxString& p_sUpdates, bool p_bInput)
{
	wxString l_sSep = wxT(" & ");

	for(unsigned i = 0; i < p_vArcs.size(); i++)
	{
		const SP_CPN_UnfoldedArcInfo& l_ArcInfo = p_vArcs[i];
		wxString l_sColoredPlName = l_ArcInfo.m_sColPlaceName;
		wxString l_sColor = l_ArcInfo.m_sColor;
		wxString l_sDiscContType = l_ArcInfo.m_sDiscContType;
		wxString l_sArcType = l_ArcInfo.m_sArcType ;

		wxString l_sMult;
		if(l_sDiscContType == wxT("Place"))
		{
			l_sMult << l_ArcInfo.m_nMultiplicity;
		}
		else
		{
			l_sMult << l_ArcInfo.m_dMultiplicity;
		}

		p_Unfolding->ModifyName(l_sColor);

		wxString l_sName = l_sColoredPlName + wxT("_") + l_sColor;

		if(p_bInput)
		{
			if (l_sArcType == SP_DS_EDGE)
			{
				if( !p_sUpdates.IsEmpty() )
				{
					p_sUpdates << l_sSep;
				}
				p_sUpdates << wxT("[");
				p_sUpdates << l_sName;
				p_sUpdates << wxT(" - ") << l_sMult;
				p_sUpdates << wxT("]");
			}
			else if (l_sArcType == SP_DS_RESET_EDGE )
			{
				if(p_sUpdates.Replace(l_sName + wxT(" + "), l_sName + wxT(" = ")) == 0)
				{
					if( !p_sUpdates.IsEmpty() )
					{
						p_sUpdates << l_sSep;
					}
					p_sUpdates << wxT("[");
					p_sUpdates << l_sName;
					p_sUpdates << wxT(" = 0");
					p_sUpdates << wxT("]");
				}
			}
			else if(l_sArcType == SP_DS_READ_EDGE)
			{
				if( !p_sConditions.IsEmpty() )
				{
					p_sConditions << l_sSep;
				}
				p_sConditions << wxT("[");
				p_sConditions << l_sName;
				p_sConditions << wxT(" >= ");
				p_sConditions << l_sMult;
				p_sConditions << wxT("]");
			}
			else if(l_sArcType == SP_DS_INHIBITOR_EDGE )
			{
				if( !p_sConditions.IsEmpty() )
				{
					p_sConditions << l_sSep;
				}
				p_sConditions << wxT("[");
				p_sConditions << l_sName;
				p_sConditions << wxT(" < ") << l_sMult;
				p_sConditions << wxT("]");
			}
			else if (l_sArcType == SP_DS_EQUAL_EDGE )
			{
				if( !p_sConditions.IsEmpty() )
				{
					p_sConditions << l_sSep;
				}
				p_sConditions << wxT("[");
				p_sConditions << l_sName;
				p_sConditions << wxT(" = ") << l_sMult;
				p_sConditions << wxT("]");
			}
			else if (l_sArcType == SP_DS_MODIFIER_EDGE )
			{
				if( !p_sConditions.IsEmpty() )
				{
					p_sConditions << l_sSep;
				}
				p_sConditions << wxT("[");
				p_sConditions << l_sName;
				p_sConditions << wxT("]");
			}
		}
		else
		{
			if( !p_sUpdates.IsEmpty() )
			{
				p_sUpdates << l_sSep;
			}
			p_sUpdates << wxT("[");
			p_sUpdates << l_sName;
			p_sUpdates << wxT(" + ") << l_sMult;
			p_sUpdates << wxT("]");
		}
	}

	return TRUE;
}

bool SP_ExportColPN2ANDL::WriteParameters(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding)
{
	m_file.Write(wxT("constants:\n"));

	wxString l_sParameterName;
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass= m_graph->GetNodeclass( SP_DS_PARAM );

	SP_ListNode::const_iterator l_itElem;
	for(l_itElem = l_pcNodeclass->GetElements()->begin();
		l_itElem != l_pcNodeclass->GetElements()->end();
		++l_itElem )
	{
		SP_DS_Node* l_pcNode = (*l_itElem);
		wxString l_sBuffer = wxT("    ");
		l_sBuffer << wxT("double ");
		l_sBuffer << dynamic_cast<SP_DS_NameAttribute*>(l_pcNode->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();
		l_sBuffer << wxT(" = ");
		SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >(l_pcNode->GetAttribute(wxT("ParameterList")));
		l_sBuffer << l_pcColList->GetActiveCellValue(1);
		l_sBuffer << wxT(";\n");
		m_file.Write(l_sBuffer);
	}

	m_file.Write(wxT("\n"));
	return TRUE;
}
