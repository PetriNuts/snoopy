//////////////////////////////////////////////////////////////////////
// $Author: CR $
// $Version: 0.1 $
// $Date: 2013/05/21 $
// Short Description: export to ssc
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportColPN2SSC.h"
#include "sp_utilities.h"


#include "sp_ds/SP_DS_Edge.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"


#include <wx/tokenzr.h>

SP_ExportColPN2SSC::SP_ExportColPN2SSC()
{
}

SP_ExportColPN2SSC::~SP_ExportColPN2SSC()
{
}

bool SP_ExportColPN2SSC::AcceptsDoc(SP_MDI_Doc* p_doc)
{
	CHECK_POINTER(p_doc, return false);
	CHECK_POINTER(p_doc->GetGraph(), return false);
	wxString cName = p_doc->GetNetclassName();
	return (cName == SP_DS_COLSPN_CLASS);
}

bool SP_ExportColPN2SSC::DoWrite()
{
	m_sNetClass = m_graph->GetNetclass()->GetName();

	std::shared_ptr<SP_DS_ColPN_Unfolding> l_pcUnfolding = std::shared_ptr<SP_DS_ColPN_Unfolding>(new SP_DS_ColPN_Unfolding());

	if( !l_pcUnfolding->Unfolding() )
		return false;	

	m_file.Write(wxT("--# SSC-file for model ") + GetNetName(m_fileName) + wxT(" created by ") + SP_APP_LONG_NAME + wxT("\n"));
	m_file.Write(wxT("--# date: ") + ::wxNow() + wxT("\n\n"));
	m_file.Write(wxT("region World\n"));
	m_file.Write(wxT("  box width 1 height 1 depth 1\n"));
	m_file.Write(wxT("subvolume edge 1\n"));
	m_file.Write(wxT("\n"));

	//WriteParameters(l_pcUnfolding);

	WritePlaces(l_pcUnfolding);

	WriteTransitions(l_pcUnfolding);

	m_file.Write(wxT("\n"));

	return true;
}

bool SP_ExportColPN2SSC::WritePlaces(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding)
{
	m_file.Write(wxT("--# places:\n"));

	if(!p_Unfolding->GetUnfoldedDiscPlaces().empty())
	{
		WritePlaceClass(p_Unfolding, p_Unfolding->GetUnfoldedDiscPlaces());
	}

	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportColPN2SSC::WritePlaceClass(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding,
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

			wxString l_sBuffer;
			if(l_sNodeType == SP_DS_DISCRETE_PLACE)
			{
				long l_nToken = itMap2->second.m_anNetMarkings[l_nCurrentColumn];
				if(l_nToken > 0)
				{
					l_sBuffer << wxT("new ") << l_sColPlName << wxT("_") << l_sColor << wxT(" at ") << l_nToken << wxT("\n");
				}
			}
			else if(l_sNodeType == SP_DS_CONTINUOUS_PLACE)
			{
				double l_nToken = itMap2->second.m_adNetMarkings[l_nCurrentColumn];
				if(l_nToken > 0)
				{
					l_sBuffer << wxT("new ") << l_sColPlName << wxT("_") << l_sColor << wxT(" at ") << l_nToken << wxT("\n");
				}
			}
			l_sBuffer << wxT("record ") + l_sColPlName + wxT("_") + l_sColor;
			l_sBuffer << wxT("\n");
			m_file.Write( l_sBuffer );
		}
	}

	return true;
}

bool SP_ExportColPN2SSC::WriteTransitions(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding)
{
	m_file.Write(wxT("--# transitions:\n"));

	if(!p_Unfolding->GetUnfoldedStochTransions().empty())
	{
		WriteTransitionClass(p_Unfolding, p_Unfolding->GetUnfoldedStochTransions());
	}

	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportColPN2SSC::WriteTransitionClass(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding,
							const SP_CPN_UnfoldedTransitions& p_mmUnfoldedTransions)
{
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

			wxString l_sBuffer = wxT("--# ") + l_sColTRName + wxT("_") + l_sBinding + wxT("\n");

			SP_DS_ColListAttribute* l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >((*(m_graph->GetNodeclass(l_NodeType)->GetElements()->begin()))->GetAttribute(wxT("FunctionList")));
			unsigned l_nCurrentColumn = l_pcColList->GetActiveColumn();

			wxString l_sFunction =itMap2->second.m_anNetFunctions[l_nCurrentColumn];

			//write arcs
			// conditions && updates
			wxString l_sConditions;
			wxString l_sUpdates;
			WriteArcs(p_Unfolding, itMap2->second.m_vInputArcs,l_sConditions,l_sUpdates,true);
			WriteArcs(p_Unfolding, itMap2->second.m_vOutputArcs,l_sConditions,l_sUpdates,false);

			l_sBuffer << wxT("rxn") << l_sConditions << wxT(" at ") << l_sFunction << wxT(" -> ") << l_sUpdates << wxT("\n");
			m_file.Write(l_sBuffer);
		}
	}
	return TRUE;
}


bool SP_ExportColPN2SSC::WriteArcs(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding,
							const vector<SP_CPN_UnfoldedArcInfo>& p_vArcs,
							wxString& p_sConditions, wxString& p_sUpdates, bool p_bInput)
{
	wxString l_sSep = wxT("; ");

	for(unsigned i = 0; i < p_vArcs.size(); i++)
	{
		const SP_CPN_UnfoldedArcInfo& l_ArcInfo = p_vArcs[i];
		wxString l_sColoredPlName = l_ArcInfo.m_sColPlaceName;
		wxString l_sColor = l_ArcInfo.m_sColor;
		wxString l_sDiscContType = l_ArcInfo.m_sDiscContType;
		wxString l_sArcType = l_ArcInfo.m_sArcType ;

		long l_nMult = l_ArcInfo.m_nMultiplicity;

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
				for (long i = 0; i < l_nMult; ++i)
				{
					p_sUpdates << wxT("destroy ");
					p_sUpdates << l_sName;
					p_sUpdates << wxT("_") << i;

					p_sConditions << wxT(" ");
					p_sConditions << l_sName;
					p_sConditions << wxT("_") << i << wxT(":");
					p_sConditions << l_sName;
				}
			}
			else if(l_sArcType == SP_DS_READ_EDGE)
			{
				for (long i = 0; i < l_nMult; ++i)
				{
					p_sConditions << wxT(" ");
					p_sConditions << l_sName;
				}
			}
			else if(l_sArcType == SP_DS_INHIBITOR_EDGE )
			{

			}
			else if (l_sArcType == SP_DS_EQUAL_EDGE )
			{

			}
			else if (l_sArcType == SP_DS_RESET_EDGE )
			{

			}
			else if (l_sArcType == SP_DS_MODIFIER_EDGE )
			{

			}
		}
		else //output
		{
			if( !p_sUpdates.IsEmpty() )
			{
				p_sUpdates << l_sSep;
			}
			for (long i = 0; i < l_nMult; ++i)
			{
				p_sUpdates << wxT("new ");
				p_sUpdates << l_sName;
			}
		}
	}

	return TRUE;
}

bool SP_ExportColPN2SSC::WriteParameters(std::shared_ptr<SP_DS_ColPN_Unfolding> p_Unfolding)
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
