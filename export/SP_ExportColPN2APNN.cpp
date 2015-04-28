//////////////////////////////////////////////////////////////////////
// $Author: F. Liu $
// $Version: 0.1 $
// $Date: 2011/05/22 $
// Short Description: export to apnn
//////////////////////////////////////////////////////////////////////

#include "export/SP_ExportColPN2APNN.h"
#include "sp_defines.h"

#include "sp_ds/SP_DS_Edge.h"
#include "sp_gui/mdi/SP_MDI_Doc.h"
#include "sp_ds/SP_DS_Graph.h"
#include "sp_ds/attributes/SP_DS_NameAttribute.h"


#include <wx/tokenzr.h>

SP_ExportColPN2APNN::SP_ExportColPN2APNN()
{
}

SP_ExportColPN2APNN::~SP_ExportColPN2APNN()
{
}

bool SP_ExportColPN2APNN::AcceptsDoc(SP_MDI_Doc* p_doc)
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

bool SP_ExportColPN2APNN::DoWrite()
{
	m_sNetClass = m_graph->GetNetclass()->GetName();

	if( !Initialize() )
		return false;	

	m_file.Write(wxT("\\beginnet{") + GetNetName(m_fileName) + wxT("}\n\n"));

	if( m_sNetClass ==  SP_DS_COLSPN_CLASS ||
		m_sNetClass ==  SP_DS_COLCPN_CLASS ||
		m_sNetClass ==  SP_DS_COLHPN_CLASS)
	{
		WriteParameters();
	}

	WritePlaces();
	WriteTransitions();
	WriteArcs();	

	m_file.Write(wxT("\n"));
	m_file.Write(wxT("\\endnet\n"));
	return !m_bError;
}

bool SP_ExportColPN2APNN::WritePlaces()
{
	for(unsigned l_nPos = 0; l_nPos < m_msPlaceNames.size(); l_nPos++)
	{
		wxString l_sPlaceName = m_msPlaceNames[l_nPos];
		wxString l_sNewPlaceName = NormalizeString( l_sPlaceName );
		if (l_sPlaceName != l_sNewPlaceName)
		{
			SP_LOGWARNING(
					wxString::Format(wxT("place name %s was changed to %s"),
							l_sPlaceName.c_str(), l_sNewPlaceName.c_str()));
		}

		if( m_sNetClass ==  SP_DS_COLCPN_CLASS ||
			m_sNetClass ==  SP_DS_COLHPN_CLASS)
		{
			m_file.Write(wxString::Format(wxT("\\place{P_%u}{\\name{%s}")
				wxT("\\init{%f}}\n"), l_nPos, l_sNewPlaceName.c_str(), m_adInitialNetMarking[l_nPos]));
		}
		else
		{
			m_file.Write(wxString::Format(wxT("\\place{P_%u}{\\name{%s}")
				wxT("\\init{%ld}}\n"), l_nPos, l_sNewPlaceName.c_str(), m_anInitialNetMarking[l_nPos]));
		}
	}	
	

	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportColPN2APNN::WriteTransitions()
{
	for(unsigned l_nPos = 0; l_nPos < m_msTransitionNames.size(); l_nPos++)
	{
		wxString l_sTranName = m_msTransitionNames[l_nPos];
		wxString l_sNewTranName = NormalizeString( l_sTranName );
		if (l_sTranName != l_sNewTranName)
		{
			SP_LOGWARNING(
					wxString::Format(wxT("transition name %s was changed to %s"),
							l_sTranName.c_str(), l_sNewTranName.c_str()));
		}
		
		wxString l_sBuffer;
		l_sBuffer << wxT("\\transition{T_") << l_nPos << wxT("}{");
		l_sBuffer << wxT("\\name{") << l_sNewTranName << wxT("}");

		if( m_sNetClass ==  SP_DS_COLSPN_CLASS ||
			m_sNetClass ==  SP_DS_COLCPN_CLASS ||
			m_sNetClass ==  SP_DS_COLHPN_CLASS)
		{
			if(!m_msTransitionFunctions[l_nPos].IsEmpty())
			{
				wxString l_sFunction = m_msTransitionFunctions[l_nPos];
				l_sFunction.Replace(wxT(" "), wxT(""));
				l_sFunction.Replace(wxT("\t"), wxT(""));
				l_sFunction.Replace(wxT("\r"), wxT(""));
				l_sFunction.Replace(wxT("\n"), wxT(""));

				l_sBuffer << wxT(" \\function{") << l_sFunction << wxT("}");

			}
		}

		l_sBuffer << wxT("}\n");
		m_file.Write(l_sBuffer);
	}	

	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportColPN2APNN::WriteParameters()
{

	wxString l_sParameterName;
	SP_DS_Nodeclass* l_pcNodeclass;
	l_pcNodeclass= m_graph->GetNodeclass( SP_DS_PARAM );

	SP_ListNode::const_iterator l_itElem;
	SP_DS_ColListAttribute* l_pcColList;

	double l_nDouble;

	for(l_itElem = l_pcNodeclass->GetElements()->begin();
		l_itElem != l_pcNodeclass->GetElements()->end();
		++l_itElem )
	{
		l_sParameterName = dynamic_cast<SP_DS_NameAttribute*>((*l_itElem)->GetFirstAttributeByType(SP_ATTRIBUTE_TYPE::SP_ATTRIBUTE_NAME))->GetValue();

		l_pcColList = dynamic_cast< SP_DS_ColListAttribute* >( (*l_itElem)->GetAttribute( wxT("ParameterList") ) );

		l_pcColList->GetActiveCellValue( 1).ToDouble( &l_nDouble );


		wxString l_sBuffer = wxT("\\const{");
		l_sBuffer << wxT("\\type{double}");
		l_sBuffer << wxT("\\name{") << l_sParameterName << wxT("}");
		l_sBuffer << wxT("\\value{") << l_nDouble << wxT("}");
		l_sBuffer << wxT("}\n");
		m_file.Write(l_sBuffer);	
	}

	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportColPN2APNN::WriteArcs()
{
	unsigned aNum = 0;
	wxString atype;
	
	for(unsigned l_nPos = 0; l_nPos < m_vPreTransitionArc.size(); l_nPos++)
	{
		SP_CPN_ContinuousArc l_StructArc = m_vPreTransitionArc[l_nPos];
		wxString l_sEdgeClass = l_StructArc.m_sArcClass;

		atype = wxT("ordinary");
		if ( l_sEdgeClass == wxT("Read Edge"))
		{
			atype = wxT("readarc");
		}
		else if (l_sEdgeClass == wxT("Inhibitor Edge"))
		{
			atype = wxT("inhibitor");
		}
		else if (l_sEdgeClass == wxT("Reset Edge"))
		{
			atype = wxT("resetarc");
		}
		else if (l_sEdgeClass == wxT("Equal Edge"))
		{
			atype = wxT("equalarc");
		}
		else if (l_sEdgeClass == wxT("Modifier Edge"))
		{
			atype = wxT("modifierarc");
		}
		
		m_file.Write(wxString::Format(
						wxT("\\arc{A_%u}{\\from{P_%ld} \\to{T_%ld}")
							wxT(" \\weight{%s} \\type{%s}}\n"), aNum, l_StructArc.m_nPlaceID,
						l_StructArc.m_nTranPos, l_StructArc.m_sMultiplicity.c_str(), atype.c_str()));
		aNum++;

	}

	for(unsigned l_nPos = 0; l_nPos < m_vPostTransitionArc.size(); l_nPos++)
	{
		SP_CPN_ContinuousArc l_StructArc = m_vPostTransitionArc[l_nPos];
		wxString l_sEdgeClass = l_StructArc.m_sArcClass;

		atype = wxT("ordinary");
		
		m_file.Write(wxString::Format(
						wxT("\\arc{A_%u}{\\from{T_%ld} \\to{P_%ld}")
							wxT(" \\weight{%s} \\type{%s}}\n"), aNum, l_StructArc.m_nTranPos,
						l_StructArc.m_nPlaceID, l_StructArc.m_sMultiplicity.c_str(), atype.c_str()));
		aNum++;

	}


	m_file.Write(wxT("\n"));
	return TRUE;
}

bool SP_ExportColPN2APNN::Initialize()
{
	std::shared_ptr<SP_DS_ColPN_Unfolding> l_pcUnfolding = std::shared_ptr<SP_DS_ColPN_Unfolding>(new SP_DS_ColPN_Unfolding());

	if( ! l_pcUnfolding->Unfolding() )
		return false;

	m_msPlaceNames = *(l_pcUnfolding->GetPlaceNames());
	m_msTransitionNames = *(l_pcUnfolding->GetTransitionNames());


	//load current marking
	m_anInitialNetMarking.clear();
	m_adInitialNetMarking.clear();
	if( m_sNetClass ==  SP_DS_COLCPN_CLASS )
	{
		m_adInitialNetMarking  = *(l_pcUnfolding->GetCurCPNMarking());
	}
	else if( m_sNetClass ==  SP_DS_COLHPN_CLASS )
	{
		m_adInitialNetMarking  = *(l_pcUnfolding->GetCurHybPNMarking());
	}
	else
	{
		m_anInitialNetMarking  = *(l_pcUnfolding->GetCurColStMarking());
	}


	//load current rate functions
	if( m_sNetClass ==  SP_DS_COLSPN_CLASS ||
		m_sNetClass ==  SP_DS_COLCPN_CLASS ||
		m_sNetClass ==  SP_DS_COLHPN_CLASS)
	{
		m_msTransitionFunctions.clear();
		m_msTransitionFunctions  = *(l_pcUnfolding->GetCurRateFunction());
	}

	//get arcs
	m_vPreTransitionArc = *(l_pcUnfolding->GetArcs_Place2Transition());
	m_vPostTransitionArc = *(l_pcUnfolding->GetArcs_Transition2Place());

	return true;
}
